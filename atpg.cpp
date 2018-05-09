#include <iostream>
#include <string>
#include "gates.h"
#include "simulation.h"
#include <memory>

using namespace std;

extern "C" void parse(const char*, Node*, Gate*, int *, int*, vector<int>*, vector<int>*);

int main (int argc, char* argv[]) {
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
}
