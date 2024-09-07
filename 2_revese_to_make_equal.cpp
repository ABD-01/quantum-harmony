#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

bool reverse_to_make_equal(vector<int>& A, vector<int>& B) {
    // Check if the arrays have the same elements
    if (unordered_set<int>(A.begin(), A.end()) != unordered_set<int>(B.begin(), B.end())) {
        return false;
    }
    
    // Create maps to store the frequency of each element in A and B
    unordered_map<int, int> freqA;
    unordered_map<int, int> freqB;
    for (int i = 0; i < A.size(); i++) {
        freqA[A[i]]++;
        freqB[B[i]]++;
    }
    
    // Check if the frequency of each element in A and B is the same
    for (auto& [key, value] : freqA) {
        if (freqB[key] != value) {
            return false;
        }
    }
    
    return true;
}

int main() {
    // Example usage
    vector<int> A = {1, 2, 3, 4};
    vector<int> B = {4, 3, 2, 1};
    cout << reverse_to_make_equal(A, B) << endl; // Output: 1 (true)
    
    return 0;
}


// In Python
// def reverse_to_make_equal(A, B):
//     # Check if the arrays have the same elements
//     if set(A) != set(B):
//         return False
    
//     # Create dictionaries to store the frequency of each element in A and B
//     freqA = {}
//     freqB = {}
//     for i in range(len(A)):
//         if A[i] not in freqA:
//             freqA[A[i]] = 0
//         freqA[A[i]] += 1
//         if B[i] not in freqB:
//             freqB[B[i]] = 0
//         freqB[B[i]] += 1
    
//     # Check if the frequency of each element in A and B is the same
//     for key in freqA:
//         if freqA[key] != freqB[key]:
//             return False
    
//     return True
