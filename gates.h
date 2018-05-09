#ifndef _GATES_H
#define _GATES_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>

using namespace std;

enum Value: unsigned char {
    ONE = '1',
    ZERO = '0',
    XVAL = 'X',
    DVAL = 'D',
    DBVAL = 'd'
};

class Gate;

class Node {
public:
    char type;
    Value value;
    int index;
    unordered_set<int> faultset;
    Gate* in;
    Gate* out;

    Node(char type, int index): type(type), 
                                index(index), 
                                value(ZERO),
                                in(NULL), out(NULL) {}

    friend ostream & operator << (ostream& out, const Node& node) {
        out << "Node(" << node.type << "-" << node.index << " = " << node.value - 48 << ")";
    }
};

class Gate {
public:
    Node* output;
    vector<Node*> inputs;
    char type;
    int index;
    
    Gate(char type, int index): type(type), index(index) {
        inputs.reserve(4);
    }

    virtual void compute() = 0;    
    virtual void computeSet() = 0;    
    void setOutput(Node* node);
    void addInput(Node* node);

    friend ostream & operator << (ostream& out, const Gate& gate) {
        out << "Gate(" << gate.type << "-" << gate.index << " => " << *(gate.output) << " <= ";
        for(Node* node : gate.inputs) out << *node << " ";
        out << ")";
    }
};

class AndGate: public Gate {
public:
    AndGate(char type, int index): Gate(type, index) {}
    void compute();
    void computeSet();    
};

class OrGate: public Gate {
public:
    OrGate(char type, int index): Gate(type, index) {}
    void compute();
    void computeSet();    
};

class XorGate: public Gate {
public:
    XorGate(char type, int index): Gate(type, index) {}
    void compute();
    void computeSet();    
};

class NotGate: public Gate {
public:
    NotGate(char type, int index): Gate(type, index) {}
    void compute();
    void computeSet();    
};

// For fanout
class PassGate: public Gate {
public:
    PassGate(char type, int index): Gate(type, index) {}
    void compute();
    void computeSet();    
};

void createGateFromName(Gate* gateptr, char type, int index);

#endif
