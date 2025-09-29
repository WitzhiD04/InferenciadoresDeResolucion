 #include "Literal.h"
#include <iostream>
#include <sstream>
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

string Literal::getNombre() {
    return this->nombre;
}

bool Literal::esNegado() {
    return this->negado;
}

const vector<string>& Literal::getArgumentos() {
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

