#include "Clausula.h"
#include <iostream>
#include <map>
using namespace std;

// ============================
// Implementación de la clase Clausula
// Una cláusula es una disyunción (OR) de literales.
// ============================

Clausula::Clausula(vector<Literal> literales) {
    // Constructor: recibe un vector de literales y lo guarda
    this->literales = literales;
}

void Clausula::imprimir() {
    // Imprime la cláusula en forma legible: L1 V L2 V L3
    for (int i = 0; i < this->literales.size(); i++) {
        this->literales[i].imprimir();   // imprime cada literal
        if (i < this->literales.size() - 1) {
            cout << " V ";               // separador "OR"
        }
    }
    cout << endl; // salto de línea al final
}

bool Clausula::esVacia() {
    // Retorna true si la cláusula no tiene literales (cláusula vacía)
    return this->literales.empty();
}

int Clausula::getLiteralesSize() {
    // Devuelve cuántos literales tiene la cláusula
    return this->literales.size();
}

Literal Clausula::getLiteral(int indice) {
    // Devuelve el literal en la posición dada, si existe
    if (indice >= 0 && indice < this->literales.size()) {
        return this->literales[indice];
    }
    // Si el índice es inválido, devuelve un literal "vacío"
    return Literal("", false, {});
}

string Clausula::getNombre(int indice) {
    // Devuelve el nombre (predicado) de un literal en la posición dada
    if (indice >= 0 && indice < this->literales.size()) {
        return this->literales[indice].getNombre();
    }
    return "";
}

bool Clausula::getNegado(int indice) {
    // Devuelve true si el literal en la posición está negado
    if (indice >= 0 && indice < this->literales.size()) {
        return this->literales[indice].esNegado();
    }
    return false; // Si índice no válido, devuelve falso por defecto
}

const vector<Literal>& Clausula::getLiterales() {
    // Retorna la referencia al vector de literales completo
    return this->literales;
}

Clausula Clausula::aplicarSustitucion(const map<string,string>& sub) const {
    // Aplica una sustitución (map de variables -> términos) a cada literal
    vector<Literal> nuevosLiterales;
    for (const Literal& lit : literales) {
        // Cada literal se transforma según el mapa "sub"
        nuevosLiterales.push_back(lit.aplicarSustitucion(sub));
    }
    // Devuelve una nueva cláusula con los literales sustituidos
    return Clausula(nuevosLiterales);
}

Clausula Clausula::renombrarVariables(int& contadorVars) const {
    // Crea una nueva cláusula donde las variables se renombran
    // para evitar colisiones entre cláusulas distintas durante la resolución.

    map<string,string> renombre;          // almacena qué variable vieja corresponde a cuál nueva
    vector<Literal> nuevosLiterales;

    for (const Literal& lit : literales) {
        vector<string> args = lit.getArgumentos();
        for (string& arg : args) {
            // Si el argumento es una variable (ejemplo: "?x")
            if (Literal::esVariable(arg)) {
                auto it = renombre.find(arg);
                if (it == renombre.end()) {
                    // Variable no vista aún: crear nombre nuevo ?vN
                    string nuevaVar = "?v" + to_string(contadorVars++);
                    renombre[arg] = nuevaVar;
                    arg = nuevaVar;
                } else {
                    // Variable ya vista: usar la misma renombrada
                    arg = it->second;
                }
            }
        }
        // Construir un nuevo literal con los argumentos posiblemente renombrados
        nuevosLiterales.push_back(Literal(lit.getNombre(), lit.esNegado(), args));
    }
    // Devolver nueva cláusula con las variables renombradas
    return Clausula(nuevosLiterales);
}
