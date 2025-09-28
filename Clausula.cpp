#include "Clausula.h"
#include <iostream>
using namespace std;

Clausula::Clausula(vector<Literal> literales) {
    this->literales = literales;
}

void Clausula::imprimir() {
    for (int i = 0; i < this->literales.size(); i++) {
        this->literales[i].imprimir();
        if (i < this->literales.size() - 1) {
            cout << " V ";
        }
    }
    cout << endl;
}

bool Clausula::esVacia() {
    return this->literales.empty();
}

int Clausula::getLiteralesSize() {
    return this->literales.size();
}

Literal Clausula::getLiteral(int indice) {
    if (indice >= 0 && indice < this->literales.size()) {
        return this->literales[indice];
    }
    return Literal("", false, {});
}

string Clausula::getNombre(int indice) {
    if (indice >= 0 && indice < this->literales.size()) {
        return this->literales[indice].getNombre();
    }
    return "";
}

bool Clausula::getNegado(int indice) {
    if (indice >= 0 && indice < this->literales.size()) {
        return this->literales[indice].esNegado();
    }
    return false; // Revisar si dejar asi
}

const vector<Literal>& Clausula::getLiterales() {
    return this->literales;
}