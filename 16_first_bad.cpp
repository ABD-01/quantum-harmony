// The API isBadVersion is defined for you.
// bool isBadVersion(int version);

class Solution {
public:
    int firstBadVersion(int n) {
        
        return recursive(1,n);
        // unsigned int i = 1, mid;
        // unsigned int firstBad = n;
        // while (i <= n) {

        //     mid = (i+n) / 2;
        //     if (isBadVersion(mid)) {
        //         firstBad = mid;
        //         n = mid - 1;
        //     }
        //     else
        //         i = mid + 1;
        // }
        // return firstBad;
    }
    unsigned int recursive(unsigned int l, unsigned int r) {
        if (l>=r)
            return l;

        unsigned int mid = (l+r)/2;
        /*
        Yes, mid = (l+r)/2 is a correct way to find the middle index of the search range. However, 
        sometimes it can lead to an integer overflow, especially when l and r are very large. 
        An alternative way to find the middle index that avoids integer overflow is mid = l + (r-l)/2
        */

        if (isBadVersion(mid))
            return recursive(l, mid);
            /*
            Doesn't work for recursive(l, mid-1);
            If you use mid-1 in the recursive call when isBadVersion(mid) returns true, you are 
            effectively ignoring the possibility that mid itself is the first bad version. So, you should 
            continue the search on the left side including mid.
            */
        else
            return recursive(mid+1, r);

    }
};