#include <unordered_set>
#include "setutil.h"

// A ^ ~B
void set_minus(unordered_set<int>& A, unordered_set<int>& B) {
    // remove from A all elements of B
    for(int elem: B)
        A.erase(elem);
}

// A v B
void set_union(unordered_set<int>& A, unordered_set<int>& B) {
    // insert into A all elements of B
    A.insert(B.begin(), B.end());
}

// A ^ B
void set_intersection(unordered_set<int>& A, unordered_set<int>& B) {
    for (int elem: A) {
        if (B.count(elem) == 0) {
            A.erase(elem);
        }
    }
}
