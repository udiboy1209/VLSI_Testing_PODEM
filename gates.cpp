#include "gates.h"
#include "setutil.h"

void Gate::setOutput(Node* node) {
    output=node;
    node->in = this;
}

void Gate::addInput(Node* node) {
    inputs.push_back(node);
    node->out = this;
}

void AndGate::compute() {
    if (inputs.size() < 2) {
        cerr << "Error: AndGate " << type << "-" << index
            << " with too few inputs." << endl;
        throw(this);
    }

    Value val = ONE;
    for(Node* node : inputs) {
        if(node->value == ZERO) {
            val = ZERO;
        }
    }

    output->value = val;
}

void AndGate::computeSet() {
    if(inputs.size() > 2) {
        cerr << "Too many inputs for set computation: " << *this << endl;
        throw(3);
    }
    if (output->value == ONE)
        output->faultset.insert(output->index*2);
    else
        output->faultset.insert(output->index*2+1);

    if (inputs[0]->value == ONE && inputs[1]->value == ONE) {
        // i1 v i2
        set_union(output->faultset, inputs[0]->faultset);
        set_union(output->faultset, inputs[1]->faultset);
    } else if (inputs[0]->value == ZERO && inputs[1]->value == ONE) {
        // i1 - i2
        set_union(output->faultset, inputs[0]->faultset);
        set_minus(output->faultset, inputs[1]->faultset);
    } else if (inputs[0]->value == ONE && inputs[1]->value == ZERO) {
        // i2 - i1
        set_union(output->faultset, inputs[1]->faultset);
        set_minus(output->faultset, inputs[0]->faultset);
    } else if (inputs[0]->value == ZERO && inputs[1]->value == ZERO) {
        // i1 ^ i2
        set_intersection(inputs[0]->faultset, inputs[1]->faultset);
        set_union(output->faultset, inputs[0]->faultset);
    }
}

void OrGate::compute() {
    if (inputs.size() < 2) {
        cerr << "Error: OrGate " << type << "-" << index
            << " with too few inputs." << endl;
        throw(this);
    }

    Value val = ZERO;
    for(Node* node : inputs) {
        if(node->value == ONE) {
            val = ONE;
        }
    }

    output->value = val;
}

void OrGate::computeSet() {
    if(inputs.size() > 2) {
        cerr << "Too many inputs for set computation: " << *this << endl;
        throw(3);
    }
    if (output->value == ONE)
        output->faultset.insert(output->index*2);
    else
        output->faultset.insert(output->index*2+1);

    if (inputs[0]->value == ZERO && inputs[1]->value == ZERO) {
        // i1 v i2
        set_union(output->faultset, inputs[0]->faultset);
        set_union(output->faultset, inputs[1]->faultset);
    } else if (inputs[0]->value == ONE && inputs[1]->value == ZERO) {
        // i1 - i2
        set_union(output->faultset, inputs[0]->faultset);
        set_minus(output->faultset, inputs[1]->faultset);
    } else if (inputs[0]->value == ZERO && inputs[1]->value == ONE) {
        // i2 - i1
        set_union(output->faultset, inputs[1]->faultset);
        set_minus(output->faultset, inputs[0]->faultset);
    } else if (inputs[0]->value == ONE && inputs[1]->value == ONE) {
        // i1 ^ i2
        set_intersection(inputs[0]->faultset, inputs[1]->faultset);
        set_union(output->faultset, inputs[0]->faultset);
    }
}

void NotGate::compute() {
    if (inputs.size() != 1) {
        cerr << "Error: NotGate " << type << "-" << index
            << " with too many inputs." << endl;
        throw(this);
    }

    Node* input = inputs.at(0);
    if(input->value == ZERO) {
        output->value = ONE;
    } else {
        output->value = ZERO;
    }
}

void NotGate::computeSet() {
    if (output->value == ONE)
        output->faultset.insert(output->index*2);
    else
        output->faultset.insert(output->index*2+1);

    set_union(output->faultset, inputs[0]->faultset);
}

void PassGate::compute() {
    if (inputs.size() != 1) {
        cerr << "Error: PassGate " << type << "-" << index
             << " with too many inputs." << endl;
        throw(this);
    }

    Node* input = inputs[0];
    output->value = input->value;
}

void PassGate::computeSet() {
    if (output->value == ONE)
        output->faultset.insert(output->index*2);
    else
        output->faultset.insert(output->index*2+1);

    set_union(output->faultset, inputs[0]->faultset);
}

void XorGate::compute() {
    if (inputs.size() < 2) {
        cerr << "Error: XorGate " << type << "-" << index
            << " with too few inputs." << endl;
        throw(this);
    }

    Value val = ZERO;
    for(Node* input : inputs) {
        if((input->value == ONE && val == ZERO) ||
                (input->value == ZERO && val == ONE)) {
            val = ONE;
        } else {
            val = ZERO;
        }
    }
    output->value = val;
}

void XorGate::computeSet() {
    if(inputs.size() > 2) {
        cerr << "Too many inputs for set computation: " << *this << endl;
        throw(3);
    }
    if (output->value == ONE)
        output->faultset.insert(output->index*2);
    else
        output->faultset.insert(output->index*2+1);

    if (inputs[0]->value == ZERO && inputs[1]->value == ZERO) {
        // i1 v i2
        set_union(output->faultset, inputs[0]->faultset);
        set_union(output->faultset, inputs[1]->faultset);
    } else if (inputs[0]->value == ONE && inputs[1]->value == ZERO) {
        // i1 v i2 - i1 ^ i2
        set_union(output->faultset, inputs[0]->faultset);
        set_union(output->faultset, inputs[1]->faultset);
        set_intersection(inputs[0]->faultset, inputs[1]->faultset);
        set_minus(output->faultset, inputs[0]->faultset);
    } else if (inputs[0]->value == ZERO && inputs[1]->value == ONE) {
        // i1 v i2 - i1 ^ i2
        set_union(output->faultset, inputs[0]->faultset);
        set_union(output->faultset, inputs[1]->faultset);
        set_intersection(inputs[0]->faultset, inputs[1]->faultset);
        set_minus(output->faultset, inputs[0]->faultset);
    } else if (inputs[0]->value == ONE && inputs[1]->value == ONE) {
        // i1 v i2
        set_union(output->faultset, inputs[0]->faultset);
        set_union(output->faultset, inputs[1]->faultset);
    }
}

void createGateFromName(Gate* gateptr, char type, int index) {
    if(type == 'a') {
        new (gateptr) AndGate(type, index);
    } else if (type == 'o') {
        new (gateptr) OrGate(type, index);
    } else if (type == 'n') {
        new (gateptr) NotGate(type, index);
    } else if (type == 'p') {
        new (gateptr) PassGate(type, index);
    } else if (type == 'x') {
        new (gateptr) XorGate(type, index);
    }

}
