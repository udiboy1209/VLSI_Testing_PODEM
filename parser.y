%{
#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>
#include "gates.h"

using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;

extern "C" void parse(const char*, Node*, Gate*, int*, int*, vector<int>*, vector<int>*);
 
void yyerror(const char *s);

int num_gates = 0; 
int num_nodes = 0; 
int input_nodes = 0;
int output_nodes = 0;

Node* g_nodelist;
Gate* g_gatelist;
vector<int>* g_inputs;
vector<int>* g_outputs;

int last_gate;
int last_node;
int out_node;

vector<int> t_nodelist;
%}

%union {
    char node;
    char gate;
    int index;
}

%token INPUTS;
%token END;
%token <node> NODE;
%token <gate> GATE;
%token <index> INDEX;

%%

atpg:
    inputdec assignments END
inputdec:
    INPUTS '(' nodedeclist ')' {t_nodelist.clear();};
assignments:
    assignments assignment
    | assignment;
assignment:
    outnode gatedec '(' nodelist ')'  {
        g_gatelist[last_gate].setOutput(&g_nodelist[out_node]);
        for(int i : t_nodelist)
            g_gatelist[last_gate].addInput(&g_nodelist[i]);
        t_nodelist.clear();
        };
outnode:
    nodedec '='  {
        out_node = last_node;
        };
nodedec:
    NODE INDEX  {
        //if(num_nodes < $2) num_nodes = $2;
        num_nodes ++;
        if ($1 == 'i') { input_nodes ++; g_inputs->push_back($2); }
        if ($1 == 'o') { output_nodes ++; g_outputs->push_back($2); }

        new(&g_nodelist[$2]) Node($1, $2);
        last_node = $2;

        // cout << g_nodelist[$2] << endl;
        };
node:
    NODE INDEX { last_node = $2; };
gatedec:
    GATE INDEX  {
        num_gates ++;

        createGateFromName(&g_gatelist[$2], $1, $2);
        last_gate = $2;

        // cout << "Gate: " << $1 << "-" << $2 << endl; 
        };
nodedeclist:
    nodedeclist nodedec { 
        t_nodelist.push_back(last_node);
        }
    | nodedec {
        t_nodelist.push_back(last_node);
        };
nodelist:
    nodelist node {
        t_nodelist.push_back(last_node);
        }
    | node {
        t_nodelist.push_back(last_node);
        };
%%

void parse(const char* filename, Node* nodes, Gate* gates, 
                                 int* nodessize, int* gatessize,
                                 vector<int>* inputs, vector<int>* outputs) {
    yyin = fopen(filename, "r");

    g_nodelist = nodes;
    g_gatelist = gates;
    g_inputs = inputs;
    g_outputs = outputs;

    do {
        cout << "Parsing..." << endl;
        yyparse();
    } while (!feof(yyin));
    
    cout << "Parse done!" << endl;
    cout << "Total nodes: " << num_nodes 
         << ", Total gates: " << num_gates << endl;
    
    *nodessize = num_nodes;
    *gatessize = num_gates;
}

void yyerror(const char *s) {
    cout << "EEK, parse error!  Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}
