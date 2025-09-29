 #include "Literal.h"
#include <iostream>
#include <sstream>
#include <map>
#include <string>
using namespace std;

Literal::Literal(string nombre, bool negado, vector<string> argumentos)  {
    this->nombre = nombre;
    this->negado = negado;
    this->argumentos = argumentos;
}

Literal Literal::inverso() {
    return Literal(this->nombre, !this->negado, this->argumentos);
}

void Literal::imprimir() {
    if (this->negado) {
        cout << "¬";
    }
    cout << this->nombre;
    if (!this->argumentos.empty()) {
        cout << "(";
        for (int i = 0; i < this->argumentos.size(); i++) {
            cout << this->argumentos[i];
            if (i < this->argumentos.size() - 1) {
                cout << ", ";
            }
        }
        cout << ")";
    }
    cout << endl;
}


string Literal::getNombre() const {
    return this->nombre;
}

bool Literal::esNegado() const {
    return this->negado;
}

const vector<string>& Literal::getArgumentos() const {
    return this->argumentos;
}

bool Literal::operator==(const Literal &o) const {
    return negado == o.negado && nombre == o.nombre && argumentos == o.argumentos;
}


bool Literal::esVariable(const string &s) {
    return !s.empty() && (s[0] == '?' || (s[0] >= 'A' && s[0] <= 'Z'));
}


std::string Literal::toString() const {
    std::ostringstream os;
    if (negado) os << "¬";
    os << nombre;
    if (!argumentos.empty()) {
        os << "(";
        for (size_t i = 0; i < argumentos.size(); ++i) {
            if (i) os << ", ";
            os << argumentos[i];
        }
        os << ")";
    }
    return os.str();
}

Literal Literal::aplicarSustitucion(const map<string,string>& sus) const{
    vector<string> nuevosArgs;
    for(const string& arg : argumentos){
        map<string,string>::const_iterator it = sus.find(arg);
        if(it != sus.end()){
            nuevosArgs.push_back(it->second);
        } 
        else {
            nuevosArgs.push_back(arg);
        }
    }
    return Literal(nombre,negado,nuevosArgs);
}

// Función auxiliar para dereferenciar (find) con compresión de paths
string find(const string& term, map<string, string>& sub) {
    if (!Literal::esVariable(term)) return term;
    map<string,string>::const_iterator it = sub.find(term);
    if (it == sub.end()) return term;
    string valor = find(it->second, sub);
    sub[term] = valor;  // Compresión
    return valor;
}

// Función auxiliar para unir dos términos bajo sub actual
bool unir(string t, string u, map<string, string>& sub) {
    t = find(t, sub);
    u = find(u, sub);
    if (t == u) return true;
    if (Literal::esVariable(t) && Literal::esVariable(u)) {
        sub[t] = u;
        return true;
    } else if (Literal::esVariable(t)) {
        // Occurs check: como términos atómicos, solo falla si t == u (pero ya chequeado)
        sub[t] = u;
        return true;
    } else if (Literal::esVariable(u)) {
        sub[u] = t;
        return true;
    }
    return false;  // Ambos constantes pero diferentes
}

bool Literal::unificar(const vector<string>& args1, const vector<string>& args2, map<string, string>& sub) {
    if (args1.size() != args2.size()) return false;
    for (size_t i = 0; i < args1.size(); ++i) {
        if (!unir(args1[i], args2[i], sub)) return false;
    }
    return true;
}

