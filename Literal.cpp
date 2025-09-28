#include "Literal.h"
#include <iostream>
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

