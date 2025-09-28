#ifndef LITERAL_H
#define LITERAL_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Literal {

private:
    string nombre;
    bool negado;
    vector <string> argumentos;
public:
    Literal(string nombre, bool negado, vector<string> argumentos);
    Literal inverso();
    void imprimir();
    string getNombre();
    bool esNegado();
    const vector<string>& getArgumentos();

};

#endif