#ifndef CLAUSULA_H
#define CLAUSULA_H

#include <iostream>
#include <string>
#include <vector>
#include "Literal.h"
using namespace std;

class Clausula {

private:
    vector <Literal> literales;
public:
    Clausula(vector<Literal> literales);
    void imprimir();
    bool esVacia();
    int getLiteralesSize();
    Literal getLiteral(int indice);
    string getNombre(int indice);
    bool getNegado(int indice);
    const vector<Literal>& getLiterales();

};

#endif