from typing import List, Optional


class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next
class Solution:
    def mergeKLists(self, lists: List[Optional[ListNode]]) -> Optional[ListNode]:
        if len(lists) < 1:
            return None
        
        while len(lists) > 1:
            mergedList = []

            for i in range(0,len(lists),2):
                l1 = lists[i]
                l2 = lists[i+1] if i+1 < len(lists) else None
                mergedList.append( self.mergeTwoLists(l1, l2) )

            lists = mergedList
        
        return lists[0]


    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        head = curr = ListNode()
        while list1 and list2:
            if list1.val > list2.val:
                curr.next = list2
                list2 = list2.next
            else:
                curr.next = list1
                list1 = list1.next
            curr = curr.next
        
        if list1:
            curr.next = list1
        if list2:
            curr.next = list2
        
        return head.next