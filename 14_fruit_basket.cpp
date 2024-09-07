#include <vector>
#include <unordered_map>

class Solution {
public:
    int totalFruit(vector<int>& fruits) {
        int n = fruits.size();
        unordered_map<int, int> umap;
        
        int max_fruits = 0;
        int unique = 0;
        int l=0,r=0, curr;

        while(r < n) {
            curr = fruits[r++];
            umap[curr]++;
            if (umap[curr] == 1)
                unique++;
            while (unique > 2) {
                curr = fruits[l++];
                umap[curr]--;
                if (umap[curr] == 0)
                    unique--;
            }
            if (r-l > max_fruits)
                max_fruits = r-l;
        }

        return max_fruits;

    }
};