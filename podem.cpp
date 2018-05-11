#include "podem.h"

using namespace std;

/* Output easiest node for backtrace from gate inputs */
Node* easiest(Gate* gate) {
    cerr << "easiest: " << *gate << endl;
    if(gate->type == 'p' || gate->type == 'n') return gate->inputs[0];

    Node* i0 = gate->inputs[0];
    Node* i1 = gate->inputs[1];

    if (i0->value != XVAL) return i1;
    if (i1->value != XVAL) return i0;

    return i0->level > i1->level ? i1 : i0;
}

/* Output hardest node for backtrace from gate inputs */
Node* hardest(Gate* gate) {
    cerr << "hardest: " << *gate << endl;
    if(gate->type == 'p' || gate->type == 'n') return gate->inputs[0];

    Node* i0 = gate->inputs[0];
    Node* i1 = gate->inputs[1];

    if (i0->value != XVAL) return i1;
    if (i1->value != XVAL) return i0;

    return i0->level > i1->level ? i0 : i1;
}

/* Run once on circuit to set longest distance from PI for every node */
void setlevels(Gate* gatelist, int gates) {
    for(int i=0; i<gates; i++) {
        int minl = gates + 1;
        for(Node *n : gatelist[i].inputs) {
            if(n->type == 'i') n->level = 0;
            minl = n->level < minl ? n->level : minl;
        }
        gatelist[i].output->level = minl + 1;
    }
}

Value backtrace(Node* node, Value val, Node** pi) {
    Gate* gate = node->in;
    int inv = 0;

    // If at input node, return as-is
    if(node->type == 'i') {*pi = node; return val;}

    // Based on implication and gate type, select appropriate input node to follow
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

    // Trace the shortest route to PI from selected node
    cerr << "backtrace: " << *node << endl;
    while (node->type != 'i') {
        // Take note of number of inversions
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

    // Go through all gates to see which gates fit onto D-Frontier
    // Add them to the list
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
    // Check if fault is yet to be sensitized and return that if not
    if(circuit.fault->value == XVAL) {
        *node = circuit.fault;
        *val = circuit.faultval;
        return true;
    }

    // If fault not sensitized, this iteration is wrong and need to backtrack. Return false
    if((circuit.fault->value == ONE || circuit.fault->value == ZERO) && circuit.fault->value != circuit.faultval) {
        return false;
    }

    updateDFrontier(circuit);

    // Nothing in the D-Frontier means D value not propagated
    if(circuit.dfront.empty()) return false;

    // Set first gate in D-Front as objective
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
    // Check if we have achieved propagation
    for(int n : circuit.outputs) {
        if(circuit.nodelist[n].value == DVAL || circuit.nodelist[n].value == DBAR)
            return true;
    }
    Node* obj;
    Node* pi;
    Value objval;

    // Try to get an objective, if we can't then propagation failed
    if(!getObjective(circuit, &obj, &objval)) return false;
    cerr << "Obj: " << *obj << ", val=" << objval << endl;

    // Backtrace to primary input
    Value pival = backtrace(obj, objval, &pi);
    pi->value = pival;
    checkFault(circuit, pi);

    cerr << "PI: " << *pi << endl;

    // Use assigned input value to simulate whole circuit
    trueValueSim(circuit);

    // Go into next recursion, and return if true
    if(podemRec(circuit)) return true;

    // if last recursion fails then try other value of PI
    pi->value = pival == ZERO ? ONE : ZERO;
    checkFault(circuit, pi);
    trueValueSim(circuit);

    if(podemRec(circuit)) return true;

    // If both failed, backtrack by returning false.
    pi->value = XVAL;
    return false;
}
