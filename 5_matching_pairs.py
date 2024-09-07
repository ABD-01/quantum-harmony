import math
# Add any extra import statements you may need here


# Add any helper functions you may need here


def matching_pairs(s, t):
  # Write your code here
  n = len(s)
  output = 0
  swp1 = None
  swp2 = None
  for i in range(n):
    if s[i] != t[i]:
      if swp1 is None:
        swp1 = i
      elif swp2 is None:
        swp2 = i
    else:
      output += 1
  
  if swp1 is None:
    output -= 2
  if swp1 is not None and swp2 is None:
    if s[swp1] not in t:
      output -= 1
  if swp1 is not None and swp2 is not None:
    if s[swp1] == t[swp2]:
      output += 1
    if s[swp2] == t[swp1]:
      output += 1

  return output


## ChatGPT Solution
# def matchingPairs(s, t):
#     N = len(s)
#     match = 0
#     freq_s = [0] * 26
#     freq_t = [0] * 26
#     diff = 0
#     for i in range(N):
#         if s[i] == t[i]:
#             match += 1
#         else:
#             freq_s[ord(s[i]) - ord('a')] += 1
#             freq_t[ord(t[i]) - ord('a')] += 1
#             if freq_s[ord(t[i]) - ord('a')] > 0:
#                 diff = 2
#             if freq_t[ord(s[i]) - ord('a')] > 0:
#                 diff = 2
#     if diff == 0:
#         if match == N:
#             return match - 2
#         else:
#             return match + 2
#     elif diff == 2:
#         return match + 1
#     else:
#         return match






# These are the tests we use to determine if the solution is correct.
# You can add your own at the bottom.

def printInteger(n):
  print('[', n, ']', sep='', end='')

test_case_number = 1

def check(expected, output):
  global test_case_number
  result = False
  if expected == output:
    result = True
  rightTick = '\u2713'
  wrongTick = '\u2717'
  if result:
    print(rightTick, 'Test #', test_case_number, sep='')
  else:
    print(wrongTick, 'Test #', test_case_number, ': Expected ', sep='', end='')
    printInteger(expected)
    print(' Your output: ', end='')
    printInteger(output)
    print()
  test_case_number += 1

if __name__ == "__main__":
  s_1, t_1 = "abcde", "adcbe"
  expected_1 = 5
  output_1 = matching_pairs(s_1, t_1)
  check(expected_1, output_1)

  s_2, t_2 = "abcd", "abcd"
  expected_2 = 2
  output_2 = matching_pairs(s_2, t_2)
  check(expected_2, output_2)

  s_3, t_3 = "abcd", "adcb"
  expected_3 = 4
  output_3 = matching_pairs(s_3, t_3)
  check(expected_3, output_3)
  # Add your own test cases here
  