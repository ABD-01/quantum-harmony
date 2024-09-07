#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


class Solution {
public:
    int maxScore(vector<int>& nums) {
        // Move all positive integers to the front
        rearrange(nums);
        
        // Compute the prefix sum array
        vector<int> prefix(nums.size(), 0);
        prefix[0] = nums[0];
        for (int i = 1; i < nums.size(); i++) {
            prefix[i] = prefix[i-1] + nums[i];
        }

        cout << "Prefix array:";
        for (int num : prefix) cout << num << " "; cout << "\n";
        
        // Count the number of positive integers in the prefix sum array
        int count = 0;
        for (int i = 0; i < nums.size(); i++) {
            if (prefix[i] > 0) {
                count++;
            }
        }
        
        return count;
    }

    void rearrange(vector<int>& nums) {
        int n = nums.size();
        int l=0, r = n-1;
        while (l<r){
            if (nums[l] < 0 && nums[r] > 0)
                swap(nums[l], nums[r]);
            if (nums[l] == 0) // Edge case to add, if not loop turns into infinte loop
                l++;
            if (nums[l] > 0)
                l++;
            if (nums[r] < 0)
                r--;
        }
    }
};



int main() {
    vector<int> nums1 = {1,2,0,4,5};
    vector<int> nums2 = {1,-2,3,4,5};
    vector<int> nums3 = {2, -1, 0, 1, -2};
    vector<int> nums4 = {-1,2,3,-4,5};
    
    Solution sol;
    cout << sol.maxScore(nums1) << endl; // expected output: 5
    cout << sol.maxScore(nums2) << endl; // expected output: 4
    cout << sol.maxScore(nums3) << endl; // expected output: 0
    cout << sol.maxScore(nums4) << endl; // expected output: 3
    

    // sol.rearrange(nums1); // expected output: 5
    // sol.rearrange(nums2); // expected output: 4
    // sol.rearrange(nums3); // expected output: 0
    // sol.rearrange(nums4); // expected output: 3

    for (int num : nums1) cout << num << " "; cout << "\n";
    for (int num : nums2) cout << num << " "; cout << "\n";
    for (int num : nums3) cout << num << " "; cout << "\n";
    for (int num : nums4) cout << num << " "; cout << "\n";

    return 0;
}

