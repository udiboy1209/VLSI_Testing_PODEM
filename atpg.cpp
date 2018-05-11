#include <iostream>
#include <string>
#include "gates.h"
#include "simulation.h"
#include "podem.h"
#include <memory>
#include <random>

using namespace std;

extern "C" void parse(const char*, Node*, Gate*, int *, int*, vector<int>*, vector<int>*);

int main (int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <graph-file>" << endl;
        return 1;
    }

    // Initialise circuit
    string filename(argv[1]);
    struct Circuit myCircuit;

    myCircuit.nodelist = (Node*) malloc(sizeof(Node) * 100);
    myCircuit.gatelist = (Gate*) malloc(sizeof(Gate) * 30);
    myCircuit.inputs.reserve(30);
    myCircuit.outputs.reserve(30);

    // Parse the circuit file
    parse(filename.c_str(), myCircuit.nodelist, myCircuit.gatelist, &(myCircuit.nodes), &(myCircuit.gates), &(myCircuit.inputs), &(myCircuit.outputs));

    // Print out decoded circuit
    for(int i=0; i<myCircuit.gates; i++) {
        cout << myCircuit.gatelist[i] << endl;
    }

    setlevels(myCircuit.gatelist, myCircuit.gates);

    /* === start ATPG === */

    bool allfaults[200];
    int faultcount = myCircuit.nodes*2;
    int untested;
    Value faultval;

    while (faultcount > 0) { // Keep trying till all faults tested or determined untestable
        untested = -1;

        // Search for untested fault
        for(int i=0; i<myCircuit.nodes*2; i++) {
            if(allfaults[i] == false) {
                untested = i;
                faultval = i%2 == 0 ? ONE : ZERO;
                break;
            }
        }

        myCircuit.fault = &myCircuit.nodelist[untested/2];
        myCircuit.faultval = faultval;

        cout << "Testing fault " << untested << " at: " << *myCircuit.fault << ", val:" << myCircuit.faultval << endl;

        // Reset circuit true values
        for(int i=0; i<myCircuit.nodes; i++) myCircuit.nodelist[i].value = XVAL;

        // Run PODEM
        bool succ = podemRec(myCircuit);

        if(!succ) { // Fault is untestable
            cout << "Fault untestable: " << untested << "-" << faultval << endl;
            faultcount --;
            allfaults[untested] = true;
            continue;
        }

        // Output test vector i.e. values stored at inputs
        cout << "Test vec:";
        for(int i : myCircuit.inputs) {
            Node *n = &myCircuit.nodelist[i];

            if(n->value == DVAL) n->value = ONE;
            if(n->value == DBAR) n->value = ZERO;
            cout << n->value;
            if(n->value == XVAL)
                n->value = rand()%2 == 0 ? ZERO : ONE;
        }
        cout << endl;

        // Run Deductive fault simulation
        simulateATPG(myCircuit.gatelist, myCircuit.nodelist, &myCircuit.inputs, &myCircuit.outputs, myCircuit.gates, myCircuit.nodes);

        // Output deduced faults and do fault dropping
        for(int out: myCircuit.outputs) {
            cout << myCircuit.nodelist[out] << ": ";
            for(int fault: myCircuit.nodelist[out].faultset) {
                cout << fault/2 << "-" << fault%2 << " ";
                if(!allfaults[fault]) faultcount --;
                allfaults[fault] = true;
            }
            cout << endl;
        }

        cout << "Faults remaining: " << faultcount << endl;
    }
}

