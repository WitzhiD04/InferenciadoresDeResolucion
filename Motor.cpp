#include "Motor.h"
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

// ============================
// Implementación de Motor de Resolución por Refutación
// - Mantiene un conjunto de cláusulas (CNF).
// - Dado un objetivo (literal), agrega su negación a la KB
//   y busca derivar la cláusula vacía mediante resolución.
// ============================

Motor::Motor(vector<Clausula> clausulas) : contadorVariables(0) {
    // Construye el motor con una base inicial de cláusulas.
    // contadorVariables se usa para renombrar variables frescas.
    this->clausulas = clausulas;
}

void Motor::agregarClausula(Clausula c) {
    // Agrega una cláusula a la base interna (no se usa en resolver,
    // pero útil si quieres ir extendiendo la KB).
    this->clausulas.push_back(c);
}

void Motor::imprimir() {
    // Imprime todas las cláusulas de la KB
    for (int i = 0; i < this->clausulas.size(); i++) {
        this->clausulas[i].imprimir();
        cout << endl; // línea extra (opcional/estético)
    }
}


// ========imprime sustitución σ de forma compacta ========
void Motor::imprimirSigma(const map<string,string>& s) const {
    if (s.empty()) { cout << "σ = {}"; return; }
    cout << "σ = { ";
    bool first=true;
    for (auto &kv : s) {
        if (!first) cout << ", ";
        first=false;
        cout << kv.first << "/" << kv.second;
    }
    cout << " }";
}

// ========reconstruye y muestra SOLO la cadena de prueba ========
void Motor::imprimirPrueba(int /*idVacia*/) const {
    // Imprime solo la cadena de pasos estrictamente necesarios para la derivación de la cláusula vacía
    if (pasos.empty() || !pasos.back().resolvente.esVacia()) {
        cout << "No se derivó la cláusula vacía.\n";
        return;
    }

    // Recolectar índices de pasos usados en la prueba mínima (hacia atrás)
    std::vector<size_t> usados;
    std::vector<bool> visitado(pasos.size(), false);
    std::function<void(size_t)> recolectar;
    recolectar = [&](size_t idx) {
        if (idx >= pasos.size() || visitado[idx]) return;
        visitado[idx] = true;
        usados.push_back(idx);
        // Buscar padres en pasos (si existen)
        // Como snapshots, buscamos por igualdad de resolvente
        for (size_t j = 0; j < pasos.size(); ++j) {
            if (!visitado[j] && (pasos[j].resolvente.getLiterales() == pasos[idx].padreA_snapshot.getLiterales() ||
                                 pasos[j].resolvente.getLiterales() == pasos[idx].padreB_snapshot.getLiterales())) {
                recolectar(j);
            }
        }
    };
    recolectar(pasos.size() - 1); // desde la vacía

    // Invertir para imprimir en orden de derivación
    std::reverse(usados.begin(), usados.end());

    cout << "=== PRUEBA (solo pasos necesarios) ===\n";
    for (size_t k = 0; k < usados.size(); ++k) {
        const Step &st = pasos[usados[k]];
        cout << "[A] "; st.padreA_snapshot.imprimir();
        cout << "[B] "; st.padreB_snapshot.imprimir();
        cout << "=> R: ";
        st.resolvente.imprimir();
        cout << "     [resolviendo: ";
        st.litA_snapshot.imprimir();
        cout << "     con     ";
        st.litB_snapshot.imprimir();
        cout << "     σ = ";
        imprimirSigma(st.sigma);
        cout << "\n";
    }
    cout << "⟂  (cláusula vacía)\n";
}

bool Motor::resolver(Literal objetivo) {
    cout << "Resolviendo para el objetivo: ";
    objetivo.imprimir();

    // 1) Refutación: agregar ¬objetivo
    objetivo = objetivo.inverso();

    // 2) Inicializar ‘todas’ con KB y luego ir metiendo resolventes
    todas = clausulas;
    pasos.clear();

    
    vector<Clausula> nuevasClausulas = clausulas;
    nuevasClausulas.push_back(Clausula({objetivo}));
    todas.push_back(Clausula({objetivo}));
    int idxObj = (int)todas.size() - 1;

    bool nuevaAgregada = true;

    while (nuevaAgregada) {
        nuevaAgregada = false;

        // solo derivar y guardar la traza
        for (int i = 0; i < (int)nuevasClausulas.size(); i++) {
            for (int j = i + 1; j < (int)nuevasClausulas.size(); ++j) {
                Clausula c1 = nuevasClausulas[i];
                Clausula c2 = nuevasClausulas[j];

                Clausula pA, pB;
                Literal lA("", false, {}), lB("", false, {});
                map<string,string> sigma;
                Clausula* resultado = comparacion(c1, c2, pA, pB, lA, lB, sigma);
                if (resultado) {
                    pasos.push_back(Step{ *resultado, pA, pB, lA, lB, sigma });
                    if (resultado->esVacia()) {
                        todas.push_back(*resultado);
                        imprimirPrueba(0);
                        cout << "Se ha derivado la clausula vacía. Objetivo demostrado.\n";
                        delete resultado;
                        return true;
                    }
                    // Evitar duplicados exactos
                    bool existe = false;
                    for (int k = 0; k < (int)nuevasClausulas.size(); k++) {
                        if (nuevasClausulas[k].getLiterales() == resultado->getLiterales()) {
                            existe = true; break;
                        }
                    }
                    if (!existe) {
                        nuevasClausulas.push_back(*resultado);
                        todas.push_back(*resultado);
                        nuevaAgregada = true;
                    }
                    delete resultado;
                }
            }
        }
    }

    cout << "No se pudo derivar una nueva clausula, no se llegó a clausula vacía\n";
    return false;
}

Clausula* Motor::comparacion(Clausula c1, Clausula c2,
                             Clausula &outPadreA, Clausula &outPadreB,
                             Literal &outLitA, Literal &outLitB,
                             map<string,string> &outSigma) {
    // Estandarizar c2 para evitar choques de variables
    int tempContador = contadorVariables;
    c2 = c2.renombrarVariables(tempContador);
    contadorVariables = tempContador;

    for (int i = 0; i < c1.getLiteralesSize(); i++) {
        for (int j = 0; j < c2.getLiteralesSize(); j++) {
            Literal l1 = c1.getLiteral(i);
            Literal l2 = c2.getLiteral(j);

            if (l1.getNombre() == l2.getNombre() && l1.esNegado() != l2.esNegado()) {
                map<string, string> sub;
                if (Literal::unificar(l1.getArgumentos(), l2.getArgumentos(), sub)) {

                    vector<Literal> nuevosLiterales;
                    for (int k = 0; k < c1.getLiteralesSize(); k++)
                        if (k != i) nuevosLiterales.push_back(c1.getLiteral(k).aplicarSustitucion(sub));
                    for (int k = 0; k < c2.getLiteralesSize(); k++)
                        if (k != j) nuevosLiterales.push_back(c2.getLiteral(k).aplicarSustitucion(sub));

                    // Snapshots exactos de lo usado
                    outPadreA = c1;
                    outPadreB = c2;
                    outLitA   = l1;
                    outLitB   = l2;
                    outSigma  = sub;

                    return new Clausula(nuevosLiterales);
                }
            }
        }
    }
    return nullptr;
}