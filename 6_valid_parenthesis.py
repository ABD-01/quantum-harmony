class Solution:
    def isValid(self, s: str) -> bool:
        stack = []
        Map = {'(':')', '{':'}', '[':']'}
        for b in s:
            if b in Map:
                stack.append(b)
            elif not stack:
                return False
            elif Map[stack[-1]] != b:
                return False
            else:
                stack.pop()
        
        if len(stack) < 1:
            return True
        return False
    
solution = Solution()

print(solution.isValid("()[]{}"))