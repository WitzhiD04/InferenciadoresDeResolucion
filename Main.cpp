#include "Motor.h"
#include "Clausula.h"
#include "Literal.h"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;

/* ---------- Funciones utilitarias ---------- */

// trim: elimina espacios en blanco al inicio y al final de un string
static inline string trim(string s){
    auto notsp = [](int ch){ return !isspace(ch); };
    s.erase(s.begin(), find_if(s.begin(), s.end(), notsp));
    s.erase(find_if(s.rbegin(), s.rend(), notsp).base(), s.end());
    return s;
}

// split: divide un string en tokens usando un separador (sep)
// Tiene en cuenta paréntesis: no parte dentro de ellos.
static inline void split(const string& s, char sep, vector<string>& out){
    out.clear();
    string cur; 
    int depth = 0; // profundidad de paréntesis
    for(char c: s){
        if(c=='(') depth++;
        if(c==')') depth = max(0, depth-1);
        if(c==sep && depth==0){ 
            out.push_back(trim(cur)); 
            cur.clear(); 
        }
        else cur.push_back(c);
    }
    if(!cur.empty()) out.push_back(trim(cur));
}

/* ---------- Parseo de literales ---------- */
// Ejemplos válidos: "~P(?x, a)"  o "R" (sin args)
static Literal parseLiteral(const string& token){
    string t = trim(token);
    bool neg = false;
    if(!t.empty() && (t[0]=='~')){ // ~ indica negación
        neg=true; 
        t=t.substr(1); 
        t=trim(t); 
    }

    // nombre del predicado y sus argumentos
    string nombre;
    vector<string> args;
    size_t p = t.find('(');
    if(p==string::npos){
        // no hay paréntesis -> predicado sin argumentos
        nombre = trim(t);
    } else {
        nombre = trim(t.substr(0, p));
        size_t q = t.rfind(')');
        if(q==string::npos || q < p) 
            throw runtime_error("Paréntesis no balanceados en literal: " + token);
        string inside = t.substr(p+1, q-p-1);
        vector<string> parts;
        split(inside, ',', parts); // separar args por coma
        for(string &a: parts) args.push_back(trim(a));
    }
    return Literal(nombre, neg, args);
}

/* ---------- Parseo de cláusulas ---------- */
// Ejemplo: "~P(?x) v Q(?x) v R(a)"
static Clausula parseClause(const string& line){
    string s = trim(line);
    if(s.empty()) return Clausula(vector<Literal>{}); // devuelve vacía

    string tmp = s;
    // normaliza separadores a 'v'
    replace(tmp.begin(), tmp.end(), '|', 'v');
    for(char &ch: tmp) if(ch=='V') ch='v';

    vector<string> lits;
    split(tmp, 'v', lits); // separar literales
    vector<Literal> vec; vec.reserve(lits.size());
    for(string &tk : lits){
        if(trim(tk).empty()) continue;
        vec.push_back(parseLiteral(tk));
    }
    return Clausula(vec);
}

/* ---------- Banner de bienvenida ---------- */
static void printBanner(){
    cout << "=== Motor de Resolucion (entrada rapida) ===\n";
    cout << "Formato de clausula por linea (CNF):\n";
    cout << "  ~P(?x) v Q(?x)\n";
    cout << "  P(a)\n";
    cout << "  R  (sin argumentos)\n";
    cout << "Termina KB con una linea vacia.\n\n";
}

/* ---------- Programa principal ---------- */
int main(){

    printBanner(); // mostrar instrucciones

    vector<Clausula> base; // base de conocimiento
    cout << "Ingresa la base de conocimiento (una clausula por linea):\n";
    cout << "Ejemplo:\n";
    cout << "  P(a)\n";
    cout << "  ~P(?x) v Q(?x)\n";
    cout << "(linea vacia para terminar)\n\n";

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // --------- Lectura de la KB ----------
    string line;
    while (true){
        cout << "KB> ";
        if(!std::getline(cin, line)) break;
        line = trim(line);
        if(line.empty()) break; // línea vacía termina la KB
        try{
            Clausula c = parseClause(line);
            if(c.getLiterales().empty()){
                cerr << "  [Aviso] Clausula vacia ignorada.\n";
                continue;
            }
            base.push_back(c);
        } catch (const exception& e){
            cerr << "  [Error] " << e.what() << "\n";
        }
    }

    if(base.empty()){
        cout << "\n[Info] KB vacia. Saliendo.\n";
        return 0;
    }

    cout << "Ingresa consultas (literal por linea). Ej: Q(a)\n";
    cout << "(linea vacia para salir)\n\n";

    // Mostrar la KB cargada
    cout << "\nKB cargada (" << base.size() << " clausulas):\n";
    for(size_t i=0;i<base.size();++i){
        cout << "  " << i+1 << ") ";
        base[i].imprimir(); 
    }
    cout << "\n";

    // --------- Creación del motor ----------
    Motor motor(base);

    // --------- Bucle de consultas ----------
    while(true){
        cout << "ASK> ";
        if(!std::getline(cin, line)) break;
        line = trim(line);
        if(line.empty()) break; // salir si línea vacía
        try{
            Clausula qC = parseClause(line);
            vector<Literal> lits = qC.getLiterales();
            if(lits.empty()){
                cerr << "  [Error] Consulta vacia.\n";
                continue;
            }
            if(lits.size()>1){
                cerr << "  [Aviso] Toma solo el primer literal de la linea para la consulta.\n";
            }
            Literal objetivo = lits[0];
            cout << "Resolviendo para el objetivo: ";
            objetivo.imprimir();
            motor.resolver(objetivo);
            cout << "\n";
        } catch (const exception& e){
            cerr << "  [Error] " << e.what() << "\n";
        }
    }

    cout << "Fin.\n";
    return 0;
}
