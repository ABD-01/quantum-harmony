#include <iostream>
#include <vector>
#include <algorithm> // for sort

using namespace std;

class Solution {
public:
    vector<vector<int>> combinationSum3(int k, int n) {
        vector<vector<int>> ans;
        if (k > n) {
            return ans;
        }
        vector<int> temp;
        combine(temp, ans, k, n, 1);
        return ans;
    }

    void combine(vector<int>& buffer, vector<vector<int>>& ans, int k, int n, int start) {
        if (start > 9)
            return;

        if (start > n)
            return;
        
        if (k==1) {
            if (n>= start && n < 10) {
                buffer.push_back(n);
                ans.push_back(buffer);
                buffer.pop_back();
                return;
            }
        }

        for(int i = start; i < 10; i++) {
            int rem = n - i;
            if (rem < 0)
                return;
            buffer.push_back(i);
            combine(buffer, ans, k-1, rem, i+1);
            buffer.pop_back(); //backtrack
        }
    }
};

void print2DVector(vector<vector<int>> &vec) {
    cout << "[\n";
    for (int i = 0; i < vec.size(); i++) {
        cout << "  [ ";
        for (int j = 0; j < vec[i].size(); j++) {
            cout << vec[i][j] << " ";
        }
        cout << "]\n";
    }
    cout << "]\n";
}


int main() {
    Solution s;
    
    // Test case 1
    int k1 = 3, n1 = 7;
    vector<vector<int>> expected1 = {{1, 2, 4}};
    vector<vector<int>> output1 = s.combinationSum3(k1, n1);
    sort(output1.begin(), output1.end());
    sort(expected1.begin(), expected1.end());
    if (output1 == expected1) {
        cout << "Test case 1 passed\n";
    } else {
        cout << "Test case 1 failed\n";
        cout << "Expected: ";
        print2DVector(expected1);
        cout << "Output: ";
        print2DVector(output1);
    }
    
    // Test case 2
    int k2 = 3, n2 = 9;
    vector<vector<int>> expected2 = {{1,2,6},{1,3,5},{2,3,4}};
    vector<vector<int>> output2 = s.combinationSum3(k2, n2);
    sort(output2.begin(), output2.end());
    sort(expected2.begin(), expected2.end());
    if (output2 == expected2) {
        cout << "Test case 2 passed\n";
    } else {
        cout << "Test case 2 failed\n";
        cout << "Expected: ";
        print2DVector(expected2);
        cout << "Output: ";
        print2DVector(output2);
    }
    
    // Test case 3
    int k3 = 4, n3 = 1;
    vector<vector<int>> expected3 = {};
    vector<vector<int>> output3 = s.combinationSum3(k3, n3);
    sort(output3.begin(), output3.end());
    sort(expected3.begin(), expected3.end());
    if (output3 == expected3) {
        cout << "Test case 3 passed\n";
    } else {
        cout << "Test case 3 failed\n";
        cout << "Expected: ";
        print2DVector(expected3);
        cout << "Output: ";
        print2DVector(output3);
    }
    
    // Test case 4
    int k4 = 2, n4 = 18;
    vector<vector<int>> expected4 = {};
    vector<vector<int>> output4 = s.combinationSum3(k4, n4);
    sort(output4.begin(), output4.end());
    sort(expected4.begin(), expected4.end());
    if (output4 == expected4) {
        cout << "Test case 4 passed\n";
    } else {
        cout << "Test case 4 failed\n";
        cout << "Expected: ";
        print2DVector(expected4);
        cout << "Output: ";
        print2DVector(output4);
    }
    

    // Test case 5
    int k5 = 5, n5 = 50;
    vector<vector<int>> expected5 = {};

    vector<vector<int>> output5 = s.combinationSum3(k5, n5);
    sort(output5.begin(), output5.end());
    sort(expected5.begin(), expected5.end());
    if (output5 == expected5) {
        cout << "Test case 5 passed\n";
    } else {
        cout << "Test case 5 failed\n";
        cout << "Expected: ";
        print2DVector(expected5);
        cout << "Output: ";
        print2DVector(output5);
    }

}