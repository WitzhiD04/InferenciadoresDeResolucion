#ifndef CLAUSULA_H
#define CLAUSULA_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Literal.h"
using namespace std;

class Clausula {

private:
    vector <Literal> literales;
public:
    Clausula();
    Clausula(vector<Literal> literales);
    void imprimir() const;
    bool esVacia() const;
    int getLiteralesSize();
    Literal getLiteral(int indice) const;
    string getNombre(int indice);
    bool getNegado(int indice);
    const vector<Literal>& getLiterales() const;

    Clausula aplicarSustitucion(const map<string,string>& sub) const;
    Clausula renombrarVariables(int& contadorVars) const;
};

#endif