#include "Motor.h"
#include <iostream>

using namespace std;

int main() {

    cout << "Cuantas clausulas tendra: " << endl;
    int n;
    cin >> n;

    vector<Clausula> clausulas;

    for (int i = 0; i < n; i++) {
        cout << "Cuantos literales tendra la clausula " << i + 1 << ": " << endl;
        int m;
        cin >> m;

        vector<Literal> literales;
        for (int j = 0; j < m; j++) {
            cout << "Nombre del literal " << j + 1 << ": " << endl;
            string nombre;
            cin >> nombre;

            cout << "El literal esta negado? (1 para si, 0 para no): " << endl;
            bool negado;
            cin >> negado;

            cout << "Cuantos argumentos tiene el literal " << j + 1 << ": " << endl;
            int k;
            cin >> k;

            vector<string> argumentos;
            for (int l = 0; l < k; ++l) {
                cout << "Argumento " << l + 1 << ": " << endl;
                string arg;
                cin >> arg;
                argumentos.push_back(arg);
            }
            literales.push_back(Literal(nombre, negado, argumentos));
        }
        clausulas.push_back(Clausula(literales));
    }

    Motor motor(clausulas);

    cout << "Ingrese la pregunta a resolver:" << endl;
    cout << "Nombre del literal objetivo: " << endl;
    string nombre;
    cin >> nombre;

    cout << "El literal objetivo esta negado? (1 para si, 0 para no): " << endl;
    bool negado;
    cin >> negado;
    cout << "Cuantos argumentos tiene el literal objetivo: " << endl;
    int k;
    cin >> k;
    vector<string> argumentos;
    for (int i = 0; i < k; i++) {
        cout << "Argumento " << i + 1 << ": " << endl;
        string arg;
        cin >> arg;
        argumentos.push_back(arg);
    }

    Literal objetivo(nombre, negado, argumentos);
    
    motor.resolver(objetivo);


    

    return 0;
}