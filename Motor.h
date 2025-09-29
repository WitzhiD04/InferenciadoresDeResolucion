#ifndef MOTOR_H
#define MOTOR_H

#include <iostream>
#include <string>
#include <vector>
#include "Clausula.h"
using namespace std;

class Motor {
private:
    vector <Clausula> clausulas;
    int contadorVariables;
public:
    Motor(vector<Clausula> clausulas);
    void agregarClausula(Clausula c);
    void imprimir();
    bool resolver(Literal objetivo); 
    Clausula* comparacion(Clausula c1, Clausula c2);
};

#endif