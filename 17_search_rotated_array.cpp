#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    int search(vector<int>& nums, int target) {
        int l=0, r=nums.size()-1, mid;

        while (l <= r) {
            mid = (l+r) / 2;
            if (nums[mid] == target)
                return mid;

            if (nums[mid] >= nums[l]) {
                if (target >= nums[l] && target < nums[mid])
                    r = mid - 1;
                else
                    l = mid + 1;
            }

            else if (nums[mid] <= nums[r]) {
                if (target > nums[mid] && target <= nums[r])
                    l = mid + 1;
                else
                    r = mid - 1;
            }
        }
        return -1;
    }
};

int main() {
    Solution s;

    // Test Case 1
    vector<int> nums1 = {4, 5, 6, 7, 0, 1, 2};
    int target1 = 0;
    int result1 = s.search(nums1, target1);
    cout << "Test Case 1: " << result1 << endl;

    // Test Case 2
    vector<int> nums2 = {4, 5, 6, 7, 0, 1, 2};
    int target2 = 3;
    int result2 = s.search(nums2, target2);
    cout << "Test Case 2: " << result2 << endl;

    // Test Case 3
    vector<int> nums3 = {1};
    int target3 = 0;
    int result3 = s.search(nums3, target3);
    cout << "Test Case 3: " << result3 << endl;

    // Test Case 4
    vector<int> nums4 = {3, 1};
    int target4 = 1;
    int result4 = s.search(nums4, target4);
    cout << "Test Case 4: " << result4 << endl;

    // Test Case 5
    vector<int> nums5 = {3, 5, 1};
    int target5 = 5;
    int result5 = s.search(nums5, target5);
    cout << "Test Case 5: " << result5 << endl;

    return 0;
}
