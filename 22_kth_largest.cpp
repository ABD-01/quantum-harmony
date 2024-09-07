#include <iostream>
#include <vector>
#include <queue>

using namespace std;

class Solution {
public:
    int findKthLargest(vector<int>& nums, int k) {
        priority_queue<int, vector<int>, greater<int>> pq; // min-heap

        int i = 0;

        // Adding k elements in min-heap
        // the top priority element is kth largest element
        while (i<k) 
            pq.push(nums[i++]);

        while(i < nums.size()) {
            // if the top is smaller than next element
            // that means it is not kth largest element.
            // the idea is to create a min heap out of all the top k elements
            // and the top of this heap will be a kth largest element
            if (pq.top() < nums[i]) {
                pq.pop();
                pq.push(nums[i]);
            }
            i++;
        }
        return pq.top();
    }
};

int main() {
    Solution sol;
    
    // Test case 1
    vector<int> nums1 = {3, 2, 1, 5, 6, 4};
    int k1 = 2;
    int output1 = sol.findKthLargest(nums1, k1);
    int expected1 = 5;
    cout << "Test case 1 - Output: " << output1 << " Expected: " << expected1 << endl;
    
    // Test case 2
    vector<int> nums2 = {3, 2, 3, 1, 2, 4, 5, 5, 6};
    int k2 = 4;
    int output2 = sol.findKthLargest(nums2, k2);
    int expected2 = 4;
    cout << "Test case 2 - Output: " << output2 << " Expected: " << expected2 << endl;
    
    // Test case 3
    vector<int> nums3 = {1};
    int k3 = 1;
    int output3 = sol.findKthLargest(nums3, k3);
    int expected3 = 1;
    cout << "Test case 3 - Output: " << output3 << " Expected: " << expected3 << endl;
    
    // Test case 4
    vector<int> nums4 = {1, 2};
    int k4 = 1;
    int output4 = sol.findKthLargest(nums4, k4);
    int expected4 = 2;
    cout << "Test case 4 - Output: " << output4 << " Expected: " << expected4 << endl;
    
    // Test case 5
    vector<int> nums5 = {1, 2};
    int k5 = 2;
    int output5 = sol.findKthLargest(nums5, k5);
    int expected5 = 1;
    cout << "Test case 5 - Output: " << output5 << " Expected: " << expected5 << endl;

    return 0;
}
