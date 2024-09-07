#include <string>
#include <iostream>

using namespace std;

class Solution {
public:
    bool isPalindrome(string s) {
        int left = 0, right=s.length()-1;
        while (left < right) {
            if (!isalnum(s[left]))
                left++;

            else if (!isalnum(s[right]))
                right--;

            else if (tolower(s[left]) != tolower(s[right])) 
                return false;
            else {
                left++;
                right--;
            }
        };
        return true;
    }
};

int main() {
    string test1 = "A man, a plan, a canal: Panama";
    bool expected1 = true;
    string test2 = "0P";
    bool expected2 = false;
    
    Solution solution;

    cout << "Test case1: " << test1 << "\nExpected: " << expected1 << "\nOutput: " << solution.isPalindrome(test1) << endl;
    cout << "Test case2: " << test2 << "\nExpected: " << expected2 << "\nOutput: " << solution.isPalindrome(test2) << endl;

}