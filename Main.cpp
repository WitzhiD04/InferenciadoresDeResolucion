#include "Motor.h"
#include "Clausula.h"
#include "Literal.h"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;

/* ---------- util ---------- */
static inline string trim(string s){
    auto notsp = [](int ch){ return !isspace(ch); };
    s.erase(s.begin(), find_if(s.begin(), s.end(), notsp));
    s.erase(find_if(s.rbegin(), s.rend(), notsp).base(), s.end());
    return s;
}
static inline void split(const string& s, char sep, vector<string>& out){
    out.clear();
    string cur; 
    int depth = 0; // para no partir dentro de ( )
    for(char c: s){
        if(c=='(') depth++;
        if(c==')') depth = max(0, depth-1);
        if(c==sep && depth==0){ out.push_back(trim(cur)); cur.clear(); }
        else cur.push_back(c);
    }
    if(!cur.empty()) out.push_back(trim(cur));
}

/* LITERAL: "~P(?x, a)"  o "R" (sin args) */
static Literal parseLiteral(const string& token){
    string t = trim(token);
    bool neg = false;
    if(!t.empty() && (t[0]=='~')){ neg=true; t=t.substr(1); t=trim(t); }

    // nombre y args
    string nombre;
    vector<string> args;
    size_t p = t.find('(');
    if(p==string::npos){
        nombre = trim(t);
    } else {
        nombre = trim(t.substr(0, p));
        size_t q = t.rfind(')');
        if(q==string::npos || q < p) throw runtime_error("Paréntesis no balanceados en literal: " + token);
        string inside = t.substr(p+1, q-p-1);
        vector<string> parts;
        split(inside, ',', parts);
        for(string &a: parts) args.push_back(trim(a));
    }
    return Literal(nombre, neg, args);
}

/* CLAUSULA: "~P(?x) v Q(?x) v R(a)" */
static Clausula parseClause(const string& line){
    string s = trim(line);
    if(s.empty()) return Clausula(vector<Literal>{}); // vacía si llega línea vacía
    string tmp = s;
    // normaliza separadores a 'v'
    replace(tmp.begin(), tmp.end(), '|', 'v');
    for(char &ch: tmp) if(ch=='V') ch='v';

    vector<string> lits;
    split(tmp, 'v', lits);
    vector<Literal> vec; vec.reserve(lits.size());
    for(string &tk : lits){
        if(trim(tk).empty()) continue;
        vec.push_back(parseLiteral(tk));
    }
    return Clausula(vec);
}

static void printBanner(){
    cout << "=== Motor de Resolucion (entrada rapida) ===\n";
    cout << "Formato de clausula por linea (CNF):\n";
    cout << "  ~P(?x) v Q(?x)\n";
    cout << "  P(a)\n";
    cout << "  R  (sin argumentos)\n";
    cout << "Termina KB con una linea vacia.\n\n";
}

/* -------- main ---------- */
int main(){

    printBanner();

    vector<Clausula> base;
    cout << "Ingresa la base de conocimiento (una clausula por linea):\n";
    cout << "Ejemplo:\n";
    cout << "  P(a)\n";
    cout << "  ~P(?x) v Q(?x)\n";
    cout << "(linea vacia para terminar)\n\n";

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    

    string line;
    while (true){
        cout << "KB> ";
        if(!std::getline(cin, line)) break;
        line = trim(line);
        if(line.empty()) break;
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

    // Mostrar KB resumida
    cout << "\nKB cargada (" << base.size() << " clausulas):\n";
    for(size_t i=0;i<base.size();++i){
        cout << "  " << i+1 << ") ";
        base[i].imprimir(); 
    }

    cout << "\n";


    

    Motor motor(base);

    
    while(true){
        cout << "ASK> ";
        if(!std::getline(cin, line)) break;
        line = trim(line);
        if(line.empty()) break;
        try{
            Clausula qC = parseClause(line);
            // forzamos que sea un solo literal (consulta tipo literal)
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
