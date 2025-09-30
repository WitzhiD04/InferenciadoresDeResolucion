#include "Motor.h"
#include <iostream>
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

bool Motor::resolver(Literal objetivo) {
    // Entrada: literal que se desea demostrar a partir de la KB.
    // Método: refutación. Se agrega ¬objetivo y se intenta derivar la vacía.

    cout << "Resolviendo para el objetivo: ";
    objetivo.imprimir();

    // Por refutación: trabajamos con la negación del objetivo
    objetivo = objetivo.inverso();

    // Copia de la KB para esta corrida y añadimos la cláusula {¬objetivo}
    vector<Clausula> nuevasClausulas = this->clausulas;
    nuevasClausulas.push_back(Clausula({objetivo}));

    // Bandera para saber si en la última pasada se agregó algo nuevo
    bool nuevaAgregada = true;

    // Bucle principal de resolución: intenta todos los pares de cláusulas,
    // generando resolventes nuevos hasta que no aparezcan más (o salga vacía).
    while (nuevaAgregada) {
        nuevaAgregada = false;
        
        // Explora todas las parejas (i,j), i < j, para evitar repetir pares.
        for (int i = 0; i < nuevasClausulas.size(); i++) {
            for (int j = i + 1; j < nuevasClausulas.size(); ++j) {
                Clausula c1 = nuevasClausulas[i];
                Clausula c2 = nuevasClausulas[j];

                // Intenta resolver c1 con c2 (si hay literales complementarios unificables)
                Clausula* resultado = comparacion(c1, c2);

                if (resultado != nullptr) {
                    // Si el resolvente es la cláusula vacía, ¡refutación lograda!
                    if (resultado->esVacia()) {
                        cout << "Se ha derivado la clausula vacía. Objetivo demostrado." << endl;
                        delete resultado;
                        return true; 
                    } 

                    // Evitar duplicados exactos: si ya existe, no la agregamos.
                    bool existe = false;
                    for (int k = 0; k < nuevasClausulas.size(); k++) {
                        if (nuevasClausulas[k].getLiterales() == resultado->getLiterales()) {
                            existe = true;
                            break;
                        }
                    }
                        
                    if (!existe) {
                        // Agrega el nuevo resolvente y marca que hubo progreso
                        nuevasClausulas.push_back(*resultado);
                        cout << "Agregada nueva clausula." << endl;
                        nuevaAgregada = true;
                    }
                    delete resultado; // liberar memoria del resolvente creado con new
                }
            }
        }
    }

    // Si sale del while sin derivar la vacía, no se pudo probar el objetivo.
    cout << "No se pudo derivar una nueva clausula, no se llegó a clausula vacía" << endl;
    return false;
}

Clausula* Motor::comparacion(Clausula c1, Clausula c2) {
    // Intenta resolver c1 y c2:
    // 1) Estandariza variables de c2 (renombrado) para evitar colisiones.
    // 2) Busca un par de literales con mismo predicado y negación opuesta.
    // 3) Si sus argumentos unifican, aplica la sustitución a las cláusulas
    //    y construye el resolvente eliminando los dos literales resueltos.

    // Renombrar variables en c2 para estandarizar-aparte (standardize apart)
    int tempContador = contadorVariables;
    c2 = c2.renombrarVariables(tempContador);
    contadorVariables = tempContador;  // Actualiza el contador global si se usaron nuevas vars

    // Recorre todos los pares de literales (uno en c1, otro en c2)
    for (int i = 0; i < c1.getLiteralesSize(); i++) {
        for (int j = 0; j < c2.getLiteralesSize(); j++) {
            Literal l1 = c1.getLiteral(i);
            Literal l2 = c2.getLiteral(j);

            // Candidatos a resolver: mismo nombre y negaciones opuestas
            if (l1.getNombre() == l2.getNombre() && l1.esNegado() != l2.esNegado() ) {

                // Intento de unificación de los argumentos
                map<string, string> sub;
                if (Literal::unificar(l1.getArgumentos(), l2.getArgumentos(), sub)) {

                    // Construcción del resolvente:
                    // - Tomar todos los literales de c1 excepto l1, aplicando σ
                    // - Tomar todos los literales de c2 excepto l2, aplicando σ
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
                    
                    // Nota: aquí no se eliminan tautologías ni duplicados dentro de la misma cláusula.
                    // (Se podría mejorar más adelante)
                    Clausula* nuevaClausula = new Clausula(nuevosLiterales);

                    // Traza para el usuario: muestra el resolvente generado
                    cout << "Nueva clausula derivada: ";
                    nuevaClausula->imprimir();

                    return nuevaClausula; // Devuelve el resolvente
                }
            }
        }
    }
    // Si no se encontró ningún par resoluble, retorna nullptr
    return nullptr; 
}
