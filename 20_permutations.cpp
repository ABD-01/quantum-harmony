#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> ans;
        if (nums.empty())
            return ans;
        
        permuteall(ans, nums, 0);
        return ans;
    }
private:
    void permuteall(vector<vector<int>>& ans, vector<int>& nums, int start) {
        // passing nums by reference to prevent copying it multiple times
        if (start >= nums.size()){
            ans.push_back(nums);
            return;
        }
        

        for(int i = start; i < nums.size(); i++) {
            swap(nums[start], nums[i]);
            permuteall(ans, nums, start + 1);
            swap(nums[start], nums[i]); //backtrack
        }
    }
};


int main() {
    Solution sol;

    // Test Case 1
    vector<int> nums1 {1, 2, 3};
    vector<vector<int>> ans1 = sol.permute(nums1);
    cout << "Test Case 1:\n";
    for (const auto& perm : ans1) {
        for (const auto& num : perm) {
            cout << num << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    // Test Case 2
    vector<int> nums2 {0, 1};
    vector<vector<int>> ans2 = sol.permute(nums2);
    cout << "Test Case 2:\n";
    for (const auto& perm : ans2) {
        for (const auto& num : perm) {
            cout << num << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    // Test Case 3
    vector<int> nums3 {1};
    vector<vector<int>> ans3 = sol.permute(nums3);
    cout << "Test Case 3:\n";
    for (const auto& perm : ans3) {
        for (const auto& num : perm) {
            cout << num << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    // Test Case 4
    vector<int> nums4 {4, 5, 6};
    vector<vector<int>> ans4 = sol.permute(nums4);
    cout << "Test Case 4:\n";
    for (const auto& perm : ans4) {
        for (const auto& num : perm) {
            cout << num << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    // Test Case 5
    vector<int> nums5 {1, 2};
    vector<vector<int>> ans5 = sol.permute(nums5);
    cout << "Test Case 5:\n";
    for (const auto& perm : ans5) {
        for (const auto& num : perm) {
            cout << num << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    return 0;
}