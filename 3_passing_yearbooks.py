import math
# Add any extra import statements you may need here


# Add any helper functions you may need here

def pass_book(old_loc, arr):
  new_loc = {}
  for stud_i, book_i in old_loc.items():
    if arr[stud_i-1] != book_i:
      new_loc[arr[stud_i-1]] = book_i
  return new_loc

def findSignatureCounts(arr):
  # Write your code here
  book_loc = dict(enumerate(range(1, len(arr)+1), start=1))
  num_sign = [1]*len(arr)
  passing = True
  
  while passing:
    book_loc = pass_book(book_loc, arr)
        
    if len(book_loc) < 1:
      passing = False
  
    for i in book_loc.keys():
      num_sign[i-1] += 1
  
  return num_sign


## Aolution of the internet, same time complexity as above O(n^2) and space complexity same as above one O(n    )
# def findSignatureCounts(arr):
  
#   ans = [1]*len(arr)
  
#   for student in range(1,len(arr)+1):
#     curr_stud = student
#     while arr[curr_stud-1] != student:
#       ans[student-1] += 1
#       curr_stud = arr[curr_stud-1]
  
#   return ans


## Link: https://leetcode.com/discuss/interview-question/614096/Facebook-or-Recruiting-Portal-or-Passing-Yearbooks/1306887 
# def findSignatureCounts(arr: list[int]) -> list[int]:
#     '''
#     O(n)
#     '''
#     signatures = [0] * len(arr)
#     # root indexes,
#     # every index has a root index
#     # who started the passing cycle of which it's part of
#     roots = [0] * len(arr)
#     # visited indexes from previous cycles
#     # to skip their counting
#     visiteds = [False] * len(arr)

#     # O(n)
#     for i in range(len(arr)):
#         # if student at i was already visited
#         if visiteds[i]:
#             continue

#         visiteds[i] = True

#         j = -1
#         student = arr[i]
#         # increment signatures for current student
#         # before she receives her own yearbook
#         while j != i:
#             # sign current yearbook
#             signatures[i] += 1
#             # student will send current yearbook to student at next index j
#             j = student - 1
#             # next index is being visited
#             visiteds[j] = True
#             # root of next index is current index
#             roots[j] = i
#             # next student
#             student = arr[j]
    
#     # set signatures of yearbooks that weren't explored
#     # because they had a root
#     # O(n)
#     for i in range(len(signatures)):
#         if roots[i] > -1:
#             signatures[i] = signatures[roots[i]]
    
#     return signatures








# These are the tests we use to determine if the solution is correct.
# You can add your own at the bottom.

def printInteger(n):
  print('[', n, ']', sep='', end='')

def printIntegerList(array):
  size = len(array)
  print('[', end='')
  for i in range(size):
    if i != 0:
      print(', ', end='')
    print(array[i], end='')
  print(']', end='')

test_case_number = 1

def check(expected, output):
  global test_case_number
  expected_size = len(expected)
  output_size = len(output)
  result = True
  if expected_size != output_size:
    result = False
  for i in range(min(expected_size, output_size)):
    result &= (output[i] == expected[i])
  rightTick = '\u2713'
  wrongTick = '\u2717'
  if result:
    print(rightTick, 'Test #', test_case_number, sep='')
  else:
    print(wrongTick, 'Test #', test_case_number, ': Expected ', sep='', end='')
    printIntegerList(expected)
    print(' Your output: ', end='')
    printIntegerList(output)
    print()
  test_case_number += 1

if __name__ == "__main__":
  arr_1 = [2, 1]
  expected_1 = [2, 2]
  output_1 = findSignatureCounts(arr_1)
  check(expected_1, output_1)

  arr_2 = [1, 2]
  expected_2 = [1, 1]
  output_2 = findSignatureCounts(arr_2)
  check(expected_2, output_2)

  arr_3 = [4, 2, 1, 3, 5]
  expected_3 = [3, 1, 3, 3, 1]
  output_3 = findSignatureCounts(arr_3)
  check(expected_3, output_3)
  # Add your own test cases here
  
  arr_4 = [4, 2, 1, 3, 5]
  expected_4 = [3, 1, 3, 3, 1]
  output_4 = findSignatureCounts(arr_4)
  check(expected_4, output_4)