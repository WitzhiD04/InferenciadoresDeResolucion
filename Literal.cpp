#include "Literal.h"
#include <iostream>
#include <sstream>
#include <map>
#include <string>
using namespace std;

// ========================================
// Implementación de la clase Literal
// Representa un átomo lógico (ej: P(a,b)) 
// o su negación (¬P(a,b)).
// ========================================

Literal::Literal(string nombre, bool negado, vector<string> argumentos)  {
    // Constructor: recibe el nombre del predicado,
    // si está negado y la lista de argumentos.
    this->nombre = nombre;
    this->negado = negado;
    this->argumentos = argumentos;
}

Literal Literal::inverso() {
    // Devuelve el literal contrario (cambia el valor de "negado")
    // Ejemplo: P(a) -> ¬P(a), ¬Q(x) -> Q(x)
    return Literal(this->nombre, !this->negado, this->argumentos);
}

void Literal::imprimir() {
    // Imprime el literal en consola en formato legible
    if (this->negado) {
        cout << "¬"; // símbolo de negación
    }
    cout << this->nombre;
    if (!this->argumentos.empty()) {
        cout << "(";
        for (int i = 0; i < this->argumentos.size(); i++) {
            cout << this->argumentos[i]; // imprime cada argumento
            if (i < this->argumentos.size() - 1) {
                cout << ", "; // separador entre argumentos
            }
        }
        cout << ")";
    }
    cout << endl; // salto de línea
}

// ====== Getters ======
string Literal::getNombre() const {
    return this->nombre;
}

bool Literal::esNegado() const {
    return this->negado;
}

const vector<string>& Literal::getArgumentos() const {
    return this->argumentos;
}

// Comparador de igualdad: dos literales son iguales
// si tienen el mismo nombre, negación y mismos argumentos
bool Literal::operator==(const Literal &o) const {
    return negado == o.negado && nombre == o.nombre && argumentos == o.argumentos;
}

// Chequea si un string representa una variable.
// Convención: empieza por "?" o por mayúscula (ej: ?x, X, Y, Z)
bool Literal::esVariable(const string &s) {
    return !s.empty() && (s[0] == '?' || (s[0] >= 'A' && s[0] <= 'Z'));
}

// Convierte un literal a string legible (para logs, pruebas, etc.)
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

// Aplica una sustitución (map var->valor) a los argumentos.
// Ejemplo: P(?x, a) con {?x -> marco} -> P(marco, a)
Literal Literal::aplicarSustitucion(const map<string,string>& sus) const{
    vector<string> nuevosArgs;
    for(const string& arg : argumentos){
        map<string,string>::const_iterator it = sus.find(arg);
        if(it != sus.end()){
            nuevosArgs.push_back(it->second); // reemplazar variable
        } 
        else {
            nuevosArgs.push_back(arg); // mantener constante
        }
    }
    return Literal(nombre,negado,nuevosArgs);
}

// ========================================
// Funciones auxiliares para UNIFICACIÓN
// ========================================

// find: resuelve a qué término equivale una variable bajo una sustitución.
// Implementa "path compression" (compresión de caminos) para optimizar.
// Ejemplo: ?x -> ?y, ?y -> a  => find(?x) = a
string find(const string& term, map<string, string>& sub) {
    if (!Literal::esVariable(term)) return term; // constante, no se busca
    map<string,string>::const_iterator it = sub.find(term);
    if (it == sub.end()) return term; // no está sustituida aún
    string valor = find(it->second, sub); // recursivo
    sub[term] = valor;  // compresión de camino
    return valor;
}

// unir: intenta unificar dos términos bajo la sustitución actual.
// Retorna true si es posible, false si no.
// Maneja casos variable-variable, variable-constante, constante-constante.
bool unir(string t, string u, map<string, string>& sub) {
    t = find(t, sub); // normalizar t
    u = find(u, sub); // normalizar u
    if (t == u) return true; // ya son iguales
    if (Literal::esVariable(t) && Literal::esVariable(u)) {
        sub[t] = u; // dos variables: asigna una a la otra
        return true;
    } else if (Literal::esVariable(t)) {
        sub[t] = u; // variable unida a constante
        return true;
    } else if (Literal::esVariable(u)) {
        sub[u] = t; // variable unida a constante
        return true;
    }
    return false;  // constante ≠ constante -> falla
}

// Unificación de listas de argumentos (ej: P(x,y) con P(a,b))
// Recorre cada argumento e intenta unificarlos con "unir".
bool Literal::unificar(const vector<string>& args1, const vector<string>& args2, map<string, string>& sub) {
    if (args1.size() != args2.size()) return false; // distinto aridad
    for (size_t i = 0; i < args1.size(); ++i) {
        if (!unir(args1[i], args2[i], sub)) return false; // falla en un arg
    }
    return true; // todos los args unificados
}
