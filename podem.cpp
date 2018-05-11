#include "podem.h"

using namespace std;

Node* easiest(Gate* gate) {
    cerr << "easiest: " << *gate << endl;
    if(gate->type == 'p' || gate->type == 'n') return gate->inputs[0];

    Node* i0 = gate->inputs[0];
    Node* i1 = gate->inputs[1];

    if (i0->value != XVAL) return i1;
    if (i1->value != XVAL) return i0;

    return i0->level > i1->level ? i1 : i0;
}

Node* hardest(Gate* gate) {
    cerr << "hardest: " << *gate << endl;
    if(gate->type == 'p' || gate->type == 'n') return gate->inputs[0];

    Node* i0 = gate->inputs[0];
    Node* i1 = gate->inputs[1];

    if (i0->value != XVAL) return i1;
    if (i1->value != XVAL) return i0;

    return i0->level > i1->level ? i0 : i1;
}

void setlevels(Gate* gatelist, int gates) {
    for(int i=0; i<gates; i++) {
        int minl = gates + 1;
        for(Node *n : gatelist[i].inputs) {
            if(n->type == 'i') n->level = 0;
            minl = n->level < minl ? n->level : minl;
        }
        gatelist[i].output->level = minl;
    }
}

Value backtrace(Node* node, Value val, Node** pi) {
    Gate* gate = node->in;
    int inv = 0;

    if(node->type == 'i') {*pi = node; return val;}

    if (gate->type == 'a') {
        if (val == ZERO) {
            node = easiest(gate);
        } else if (val == ONE) {
            node = hardest(gate);
        }
    }

    if (gate->type == 'o') {
        if (val == ONE) {
            node = easiest(gate);
        } else if (val == ZERO) {
            node = hardest(gate);
        }
    }

    if (gate->type == 'x') {
        node = easiest(gate);
    }

    if (gate->type == 'n') {
        node = gate->inputs[0];
        inv = 1;
    }

    if (gate->type == 'p') {
        node = gate->inputs[0];
    }

    cerr << "backtrace: " << *node << endl;
    while (node->type != 'i') {
        if(node->in->type == 'n') inv ++;

        node = easiest(node->in);
        cerr << "backtrace: " << *node << endl;
    }
    *pi = node;
    return inv%2 == 0 ? val : (val == ZERO ? ONE : ZERO);
}

void updateDFrontier(Circuit& circuit) {
    Gate* g;

    circuit.dfront.clear();
    circuit.dfront.reserve(circuit.gates/2);
    for(int i=0; i < circuit.gates; i++) {
        g = &(circuit.gatelist[i]);
        if(g->output->value != XVAL) continue;
        else {
            for(Node* in : g->inputs) {
                if(in->value == DVAL || in->value == DBAR) {
                    circuit.dfront.push_back(g);
                    break;
                }
            }
        }
    }
}

bool getObjective(Circuit& circuit, Node** node, Value* val) {
    if(circuit.fault->value == XVAL) {
        *node = circuit.fault;
        *val = circuit.faultval;
        return true;
    }

    if((circuit.fault->value == ONE || circuit.fault->value == ZERO) && circuit.fault->value != circuit.faultval) {
        return false;
    }

    updateDFrontier(circuit);

    if(circuit.dfront.empty()) return false;

    Gate* d = circuit.dfront[0];
    for(Node* n : d->inputs) {
        if(n->value == XVAL) {
            *node = n;
            break;
        }
    }
    *val = d->type == 'a' ? ONE : ((d->type == 'o' || d->type == 'x') ? ZERO : XVAL);
    return true;
}

bool podemRec(Circuit& circuit) {
    for(int n : circuit.outputs) {
        if(circuit.nodelist[n].value == DVAL || circuit.nodelist[n].value == DBAR)
            return true;
    }
    Node* obj;
    Node* pi;
    Value objval;

    if(!getObjective(circuit, &obj, &objval)) return false;
    cerr << "Obj: " << *obj << ", val=" << objval << endl;

    Value pival = backtrace(obj, objval, &pi);
    pi->value = pival;
    checkFault(circuit, pi);

    cerr << "PI: " << *pi << endl;

    trueValueSim(circuit);

    if(podemRec(circuit)) return true;

    pi->value = pival == ZERO ? ONE : ZERO;
    checkFault(circuit, pi);
    trueValueSim(circuit);

    if(podemRec(circuit)) return true;

    pi->value = XVAL;
    return false;
}
