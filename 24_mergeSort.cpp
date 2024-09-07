#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    vector<int> sortArray(vector<int>& nums) {
        vector<int> buffer(nums.size());
        mergeSort(nums, 0, nums.size()-1, buffer);
        return nums;
        
    }

    void merge2arrays(vector<int>& nums, int left, int mid, int right, vector<int>& buffer) {
        int n1 = mid -left + 1;
        int n2 = right - mid;
        
        for (int i = 0; i < n1; i++) 
            buffer[left + i] = nums[left + i];
        for (int i = 0; i < n2; i++) 
            buffer[mid+1 + i] = nums[mid+1 + i];

        int i=mid, j=right, k=right;

        while (i>=left && j>=mid+1) {
            if (buffer[i] > buffer[j])
                nums[k--] = buffer[i--];
            else
                nums[k--] = buffer[j--];
        }
        while (i>=left) {
                nums[k--] = buffer[i--];
        }
        while (j>=mid+1) {
                nums[k--] = buffer[j--];
        }
    }

    void mergeSort(vector<int>& nums, int l, int r, vector<int>& buffer) {
        if (l >= r)
            return;

        int mid  = l + (r - l) / 2;
        mergeSort(nums, l, mid, buffer);
        mergeSort(nums, mid+1, r, buffer);
        merge2arrays(nums, l, mid, r, buffer);
    }
};


int main() {
    Solution s;

    // Test case 1: empty vector
    vector<int> nums1;
    vector<int> expected1;
    vector<int> result1 = s.sortArray(nums1);
    cout << "Test case 1: ";
    if (result1 == expected1) {
        cout << "Passed" << endl;
    } else {
        cout << "Failed" << endl;
    }

    // Test case 2: already sorted vector
    vector<int> nums2 = {1, 2, 3, 4, 5};
    vector<int> expected2 = {1, 2, 3, 4, 5};
    vector<int> result2 = s.sortArray(nums2);
    cout << "Test case 2: ";
    if (result2 == expected2) {
        cout << "Passed" << endl;
    } else {
        cout << "Failed" << endl;
    }

    // Test case 3: reverse sorted vector
    vector<int> nums3 = {5, 4, 3, 2, 1};
    vector<int> expected3 = {1, 2, 3, 4, 5};
    vector<int> result3 = s.sortArray(nums3);
    cout << "Test case 3: ";
    if (result3 == expected3) {
        cout << "Passed" << endl;
    } else {
        cout << "Failed" << endl;
    }

    // Test case 4: vector with duplicate elements
    vector<int> nums4 = {5, 1, 4, 3, 1, 2};
    vector<int> expected4 = {1, 1, 2, 3, 4, 5};
    vector<int> result4 = s.sortArray(nums4);
    cout << "Test case 4: ";
    if (result4 == expected4) {
        cout << "Passed" << endl;
    } else {
        cout << "Failed" << endl;
    }

    // Test case 5: vector with negative elements
    vector<int> nums5 = {-3, 1, -5, 0, 2};
    vector<int> expected5 = {-5, -3, 0, 1, 2};
    vector<int> result5 = s.sortArray(nums5);
    cout << "Test case 5: ";
    if (result5 == expected5) {
        cout << "Passed" << endl;
    } else {
        cout << "Failed" << endl;
    }

    // Test case 6: vector with large number of elements
    vector<int> nums6 = {3, 6, 9, 2, 4, 8, 1, 7, 5, 0, 10, 11, 12, 15, 14, 13, 19, 18, 17, 16};
    vector<int> expected6 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    vector<int> result6 = s.sortArray(nums6);
    cout << "Test case 6: ";
    if (result6 == expected6) {
        cout << "Passed\n";
    } else {
        cout << "Failed\n";
    } 

    // Edge case: empty vector
    vector<int> nums7;
    vector<int> expected7;
    vector<int> result7 = s.sortArray(nums7);
    cout << "Test case 7: ";
    if (result7 == expected7) {
        cout << "Passed\n";
    } else {
        cout << "Failed\n";
    }

    // Edge case: vector with one element
    vector<int> nums8 = {5};
    vector<int> expected8 = {5};
    vector<int> result8 = s.sortArray(nums8);
    cout << "Test case 8: ";
    if (result8 == expected8) {
        cout << "Passed\n";
    } else {
        cout << "Failed\n";
    }

    // Edge case: vector with repeated elements
    vector<int> nums9 = {3, 2, 1, 3, 2, 1, 3, 2, 1};
    vector<int> expected9 = {1, 1, 1, 2, 2, 2, 3, 3, 3};
    vector<int> result9 = s.sortArray(nums9);
    cout << "Test case 9: ";
    if (result9 == expected9) {
        cout << "Passed\n";
    } else {
        cout << "Failed\n";
    }

    return 0;
}