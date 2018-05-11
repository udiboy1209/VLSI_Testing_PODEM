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
    DBAR = 'd'
};

ostream& operator<<(ostream& out, const Value val);

class Gate;
class Node;

struct Circuit {
    Node* nodelist;
    int nodes;
    Gate* gatelist;
    int gates;
    vector<int> inputs;
    vector<int> outputs;
    vector<Gate*> dfront;

    Node* fault;
    Value faultval;
};

class Node {
public:
    char type;
    Value value;
    int index;
    int level;
    unordered_set<int> faultset;
    Gate* in;

    Node(char type, int index): type(type), 
                                index(index),
                                level(0),
                                value(XVAL),
                                in(NULL) {}

    friend ostream & operator << (ostream& out, const Node& node) {
        out << "Node(" << node.type << "-" << node.index << " = " << node.value << ")";
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
