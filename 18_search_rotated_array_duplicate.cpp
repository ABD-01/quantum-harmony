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
                return true;

            /*
            The purpose of the condition nums[mid] == nums[left] && nums[mid] == nums[right] is 
            to handle the case where we have duplicate values in the array. In such cases, we might
             not be able to determine which side of the array is sorted and which side is rotated.

            For example, consider the input array [2, 2, 2, 0, 2, 2] where target = 0. In this case,
            we cannot distinguish between the left and right subarrays since both of them contain only 2s.
            Therefore, we need to increment left and decrement right until we have 
            nums[mid] != nums[left] or nums[mid] != nums[right], which will allow us to determine
             which side of the array is sorted and which side is rotated.

            In summary, this condition is used to handle cases where the array has duplicates and we cannot 
            determine which side of the array is sorted and which side is rotated.
            */
            if (nums[mid] == nums[l] && nums[mid] == nums[r]) {
                ++l;
                --r;
            }

            else if (nums[mid] >= nums[l]) {
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
        return false;
    }
};

int main() {
    Solution s;
    vector<int> nums1 = {2, 5, 6, 0, 0, 1, 2};
    int target1 = 0;
    cout << "Test Case 1: " << s.search(nums1, target1) << endl;

    vector<int> nums2 = {2, 5, 6, 0, 0, 1, 2};
    int target2 = 3;
    cout << "Test Case 2: " << s.search(nums2, target2) << endl;

    vector<int> nums3 = {1};
    int target3 = 0;
    cout << "Test Case 3: " << s.search(nums3, target3) << endl;

    vector<int> nums4 = {1, 1, 1, 1, 1};
    int target4 = 2;
    cout << "Test Case 4: " << s.search(nums4, target4) << endl;

    vector<int> nums5 = {3, 1, 1};
    int target5 = 3;
    cout << "Test Case 5: " << s.search(nums5, target5) << endl;

    vector<int> nums6 = {5, 6, 7, 8, 9, 1, 2, 3, 4};
    int target6 = 1;
    cout << "Test Case 6: " << s.search(nums6, target6) << endl;


    vector<int> nums7 = {1,0,1,1,1};
    int target7 = 0;
    cout << "Test Case 7: " << s.search(nums7, target7) << endl;

    return 0;
}