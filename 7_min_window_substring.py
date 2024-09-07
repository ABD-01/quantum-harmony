class Solution:
    def minWindow(self, s: str, t: str) -> str:
        if len(s) < len(t):
            return ""

        # if len(s) == len(t):
        #     if s==t:
        #         return s
        #     return ""

        n = len(s)

        left = 0
        right = n-1
        MinLen = n
        i = 0
        j = 0
        found = False
        
        Map = {}
        for char in t:
            Map[char] = Map.get(char, 0) + 1
        count = len(Map)
            
        while j < n:
            curr_char = s[j]
            if curr_char in Map:
                Map[curr_char] -= 1
                if Map[curr_char] == 0:
                    count -= 1
                
            j += 1
            if count > 0:
                continue
            
            while count == 0: # removing prefxing unwanted
                curr_char = s[i]
                if curr_char in Map:
                    Map[curr_char] += 1
                    if Map[curr_char]>0:
                        count += 1
                
                i += 1
                
            if j-i+1  <= MinLen:
                left = i-1
                right = j
                MinLen = j-i+1
                found = True
            
        if found:
            return s[left:right]
        return ""
    
solution = Solution()
print(solution.minWindow(s="abc", t="cba"))