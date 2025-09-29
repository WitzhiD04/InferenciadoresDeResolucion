#include "Clausula.h"
#include <iostream>
#include <map>
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

Clausula Clausula::aplicarSustitucion(const map<string,string>& sub) const {
    vector<Literal> nuevosLiterales;
    for (const Literal& lit : literales) {
        nuevosLiterales.push_back(lit.aplicarSustitucion(sub));
    }
    return Clausula(nuevosLiterales);
}

Clausula Clausula::renombrarVariables(int& contadorVars) const {
    map<string,string> renombre;
    vector<Literal> nuevosLiterales;
    for (const Literal& lit : literales) {
        vector<string> args = lit.getArgumentos();
        for (string& arg : args) {
            if (Literal::esVariable(arg)) {
                map<string,string>::const_iterator it = renombre.find(arg);
                if (it == renombre.end()) {
                    string nuevaVar = "?v" + to_string(contadorVars++);
                    renombre[arg] = nuevaVar;
                    arg = nuevaVar;
                } else {
                    arg = it->second;
                }
            }
        }
        nuevosLiterales.push_back(Literal(lit.getNombre(), lit.esNegado(), args));
    }
    return Clausula(nuevosLiterales);
}