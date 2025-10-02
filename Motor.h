#ifndef MOTOR_H
#define MOTOR_H

// Motor.h
#pragma once
#include "Clausula.h"
#include <vector>
#include <map>
#include <string>

class Motor {

    private:
    std::vector<Clausula> clausulas;   // KB inicial
    int contadorVariables;

    // ======== NUEVO: Estructura para guardar la prueba ========
    struct Step {
        Clausula resolvente;                      // resolvente obtenido
        Clausula padreA_snapshot;                 // padre A tal como se usó (ya renombrado)
        Clausula padreB_snapshot;                 // padre B tal como se usó (ya renombrado)
        Literal  litA_snapshot;                   // literal de A resuelto
        Literal  litB_snapshot;                   // literal de B resuelto
        std::map<std::string,std::string> sigma;  // sustitución usada
    };

    // ‘todas’ = KB inicial + todos los resolventes generados en orden
    std::vector<Clausula> todas;
    std::vector<Step> pasos; // sólo para resolventes (no incluye KB)

    // Helpers de impresión (solo al final si se prueba el objetivo)
    void imprimirSigma(const std::map<std::string,std::string>& s) const;
    void imprimirPrueba(int idVacia) const; // id en 'todas' donde quedó la vacía

public:
    Motor(std::vector<Clausula> clausulas);
    void agregarClausula(Clausula c);
    void imprimir();

    bool resolver(Literal objetivo);


    // Devuelve nullptr si no resolubles. Si hay resolvente, rellena info.
    Clausula* comparacion(Clausula c1, Clausula c2,
                          Clausula &outPadreA, Clausula &outPadreB,
                          Literal &outLitA, Literal &outLitB,
                          std::map<std::string,std::string> &outSigma);
};


#endif