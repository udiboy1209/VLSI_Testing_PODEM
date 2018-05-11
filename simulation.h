#include <vector>
#include<iostream>
#include "gates.h"

using namespace std;

void simulate(Gate* gatelist, Node* nodelist,
                vector<int>* inputs, vector<int>* outputs, 
                char* inputvec,
                int gates, int nodes);
void simulateATPG(Gate* gatelist, Node* nodelist,
                vector<int>* inputs, vector<int>* outputs, 
                int gates, int nodes);

void checkFault(Circuit& circuit, Node* node);
void trueValueSim(Circuit& circuit);
