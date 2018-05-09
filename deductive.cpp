#include <iostream>
#include <string>
#include "gates.h"
#include "simulation.h"
#include <memory>

using namespace std;

extern "C" void parse(const char*, Node*, Gate*, int *, int*, vector<int>*, vector<int>*);

int main (int argc, char* argv[]) {
    cout << "Hello";
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <graph-file>" << endl;
        return 1;
    }

    string filename(argv[1]);
    int nodes, gates;

    Node* nodelist = (Node*) malloc(sizeof(Node) * 100);
    Gate* gatelist = (Gate*) malloc(sizeof(Gate) * 30);
    vector<int> inputs;
        inputs.reserve(30);
    vector<int> outputs;
        outputs.reserve(30);

    parse(filename.c_str(), nodelist, gatelist, &nodes, &gates, &inputs, &outputs);

    char inputstr[30];

    bool allfaults[200];
    int faultcount = nodes*2;

    for(int i=0; i<nodes*2; i++) allfaults[i] = false;

    while (faultcount > 0) {
        cin >> inputstr;
        cout << "Testing with input: " << inputstr << endl;

        simulate(gatelist, nodelist, &inputs, &outputs, inputstr, gates, nodes);

        // Output faults
        for(int out: outputs) {
            cout << nodelist[out] << ": ";
            for(int fault: nodelist[out].faultset) {
                cout << fault/2 << "-" << fault%2 << " ";
                if(!allfaults[fault]) faultcount --;
                allfaults[fault] = true;
            }
            cout << endl;
        }
    }

    cout << "All faults covered" << endl;
}
