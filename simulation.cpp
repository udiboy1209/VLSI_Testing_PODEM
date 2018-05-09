#include "simulation.h"

void simulate(Gate *gatelist, Node *nodelist,
                vector<int> *inputs, vector<int> *outputs,
                char* inputvec,
                int gates, int nodes) {

    for(int i = 0; inputvec[i] != '\0'; i++) {
        if(inputvec[i] == ONE) {
            nodelist[(*inputs)[i]].value = ONE;
        } else {
            nodelist[(*inputs)[i]].value = ZERO;
        }
    }

    // clear previous computation's faults
    for(int i=0; i<nodes; i++) nodelist[i].faultset.clear();

    // Run true value propagation
    for(int i=0; i<gates; i++) {
        gatelist[i].compute();
    }

    // set input faults
    for(int i: *inputs) {
        if (nodelist[i].value == ONE)
            nodelist[i].faultset.insert(i*2);
        else
            nodelist[i].faultset.insert(i*2+1);
    }

    // Run fault propagation
    for(int i=0; i<gates; i++) {
        gatelist[i].computeSet();
    }
}
