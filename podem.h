#include <iostream>
#include "gates.h"
#include "simulation.h"

Value backtrace(Node* node, Value val, Node** pi); 

bool podemRec(Circuit& circuit); 
