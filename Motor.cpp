#include "Motor.h"
#include <iostream>
using namespace std;

Motor::Motor(vector<Clausula> clausulas) : contadorVariables(0) {
    this->clausulas = clausulas;
}

void Motor::agregarClausula(Clausula c) {
    this->clausulas.push_back(c);
}

void Motor::imprimir() {
    for (int i = 0; i < this->clausulas.size(); i++) {
        this->clausulas[i].imprimir();
        cout<<endl;
    }
}

bool Motor::resolver(Literal objetivo) {
    cout << "Resolviendo para el objetivo: ";
    objetivo.imprimir();

    objetivo = objetivo.inverso();

    vector<Clausula> nuevasClausulas = this->clausulas;
    nuevasClausulas.push_back(Clausula({objetivo}));

    bool nuevaAgregada = true;

    while (nuevaAgregada) {
        nuevaAgregada = false;
        
        for (int i = 0; i < nuevasClausulas.size(); i++) {
            for (int j = i + 1; j < nuevasClausulas.size(); ++j) {
                Clausula c1 = nuevasClausulas[i];
                Clausula c2 = nuevasClausulas[j];

                Clausula* resultado = comparacion(c1, c2);

            if (resultado != nullptr) {
                    if (resultado->esVacia()) {
                        cout << "Se ha derivado la clausula vacía. Objetivo demostrado." << endl;
                        delete resultado;
                        return true; 
                    } 
                    bool existe = false;
                    for (int k = 0; k < nuevasClausulas.size(); k++) {
                        if (nuevasClausulas[k].getLiterales() == resultado->getLiterales()) {
                            existe = true;
                            break;
                        }
                    }
                        
                    if (!existe) {
                        nuevasClausulas.push_back(*resultado);
                        cout << "Agregada nueva clausula." << endl;
                        nuevaAgregada = true;
                    }
                    delete resultado;
                }
            }
        }
    }

    cout << "No se pudo derivar una nueva clausula, no se llegó a clausula vacía" << endl;
    return false;
    
}

Clausula* Motor::comparacion(Clausula c1, Clausula c2) {
    // Renombrar vars en c2 para estandarizar
    int tempContador = contadorVariables;
    c2 = c2.renombrarVariables(tempContador);
    contadorVariables = tempContador;  // Actualizar global si se usaron nuevas vars

    for (int i = 0; i < c1.getLiteralesSize(); i++) {
        for (int j = 0; j < c2.getLiteralesSize(); j++) {
            Literal l1 = c1.getLiteral(i);
            Literal l2 = c2.getLiteral(j);

            if (l1.getNombre() == l2.getNombre() && l1.esNegado() != l2.esNegado() && l1.getArgumentos() == l2.getArgumentos()) {
                map<string, string> sub;
                if (Literal::unificar(l1.getArgumentos(), l2.getArgumentos(), sub)) {
                vector<Literal> nuevosLiterales;
                for (int k = 0; k < c1.getLiteralesSize(); k++) {
                    if (k != i) {
                        nuevosLiterales.push_back(c1.getLiteral(k).aplicarSustitucion(sub));
                    } 
                }

                for (int k = 0; k < c2.getLiteralesSize(); k++) {
                    if (k != j) {
                        nuevosLiterales.push_back(c2.getLiteral(k).aplicarSustitucion(sub));
                    } 
                }
                    
                Clausula* nuevaClausula = new Clausula(nuevosLiterales);
                cout << "Nueva clausula derivada: ";
                nuevaClausula->imprimir();
                return nuevaClausula;
            }
        }
    }
}
    return nullptr; 
}
