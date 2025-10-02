#ifndef LITERAL_H
#define LITERAL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

class Literal {

private:
    string nombre;
    bool negado;
    vector <string> argumentos;
public:
    Literal(string nombre, bool negado, vector<string> argumentos);
    Literal inverso();
    void imprimir() const;
    string getNombre() const;
    bool esNegado() const;
    const vector<string>& getArgumentos() const;
    bool operator==(const Literal &o) const;
    static bool esVariable(const string &s);
    string toString() const;
    Literal aplicarSustitucion(const map<string,string>& sus) const;

    static bool unificar(const vector<string>& args1, const vector<string>& args2, map<string, string>& sub);    
};

#endif