#include <vector>
#include "gates.h"

using namespace std;

void simulate(Gate* gatelist, Node* nodelist,
                vector<int>* inputs, vector<int>* outputs, 
                char* inputvec,
                int gates, int nodes);
