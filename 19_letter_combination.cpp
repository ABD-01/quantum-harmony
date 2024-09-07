#include <iostream>
// #include <string>
#include <vector>

using namespace std;

class Solution {
public:
    vector<string> letterCombinations(string digits) {
        vector<string> ans;
        if (digits.length() < 1) {
            return ans;
        }
        combine("", ans, digits, 0);
        return ans;
    }

    vector<string> digitToLetter = {"", "", "abc", "def",
                                    "ghi", "jkl", "mno",
                                    "pqrs", "tuv", "wxyz"};

    void combine(string prefix, vector<string>& ans, string digits, int start) {
        if (start >= digits.length()) {
            ans.push_back(prefix);
            return;
        }

        int digit = digits[start] - '0';
        // cout << digit << endl;
        string letters = digitToLetter[digit];
        // cout << letters << endl;
        int n = letters.length();
        for(int i = 0; i < n; i++) {
            combine(prefix + letters[i], ans, digits, start + 1);
        }
    }
};

int main(){
    Solution s;

    // Example from the prompt
    vector<string> ans1 = s.letterCombinations("23");
    // Expected output: {"ad","ae","af","bd","be","bf","cd","ce","cf"}
    for (const auto& s : ans1) {
        cout << s << " ";
    }
    cout << endl;

    // Single digit input
    vector<string> ans2 = s.letterCombinations("7");
    // Expected output: {"p","q","r","s"}
    for (const auto& s : ans2) {
        cout << s << " ";
    }
    cout << endl;

    // Input with repeating digits
    vector<string> ans3 = s.letterCombinations("22");
    // Expected output: {"aa","ab","ac","ba","bb","bc","ca","cb","cc"}
    for (const auto& s : ans3) {
        cout << s << " ";
    }
    cout << endl;

    // Empty input
    vector<string> ans4 = s.letterCombinations("");
    // Expected output: {}
    for (const auto& s : ans4) {
        cout << s << " ";
    }
    cout << endl;

    return 0;
}