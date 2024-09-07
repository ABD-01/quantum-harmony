#include <iostream>

//  Definition for singly-linked list.
struct ListNode {
int val;
struct ListNode *next;
ListNode() : val(0), next(nullptr) {}
ListNode(int x) : val(x), next(nullptr) {}
ListNode(int x, ListNode *next) : val(x), next(next) {}
};
 

class Solution {
public:
    bool isPalindrome(ListNode* head) {
        ListNode *midNode = (*this).middleNode(head);
        ListNode *reversedhalf = (*this).reverseLinkedList(midNode);

        ListNode *l1 = head, *l2 = reversedhalf;

        while(l1 && l2) {
            if((*l1).val != (*l2).val)
                return false;
            l1 = l1->next;
            l2 = l2->next;
        }
        return true;
    }

private:
    ListNode* reverseLinkedList(ListNode* head) {
        ListNode *prev = NULL, *curr = head;
        ListNode *temp;
        while(curr) {
            temp = curr->next;
            curr->next = prev;
            prev = curr;
            curr = temp;
        }
        return prev;
    }

    ListNode* middleNode(ListNode* head) {
        ListNode *slow = head, *fast=head;
        while(fast->next && fast->next->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        if (fast->next)
            return slow->next;
        else
            return slow;
    }
};