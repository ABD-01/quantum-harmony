#include <iostream>
using namespace std;

class Node {
public:
    int data;
    Node* next;
    
    Node(int data, Node* next) {
        this->data = data;
        this->next = next;
    }
};

void modifyNode(Node& n) {
    n.data = 20;
    n.next = NULL;
}

// what is difference between
// Node* newNode = new Node(10, NULL);
// and
// Node newnode(10, NULL);
// 
// The first statement creates a pointer to a dynamically allocated Node object and initializes
//  its values to 10 and NULL. The newly created object is allocated on the heap using the new operator, 
//  and its address is stored in the pointer newNode.
// 
// The second statement creates a Node object named newnode with values 10 and NULL. This object is 
// created on the stack, and its lifespan is limited to the scope in which it is defined. Once the program 
// control exits this scope, the object is automatically destroyed.
// 
// The main difference between the two is that the first one creates the object on the heap, and thus 
// it persists beyond the scope in which it was created, while the second one creates the object on 
// the stack, and its lifespan is limited to the scope in which it was created.

int main() {
    // Creating a node using a constructor
    Node* newNode = new Node(10, NULL);
    cout << "Original node data: " << newNode->data << endl;
    
    // Modifying the node using a function with reference parameter
    modifyNode(*newNode);
    cout << "Modified node data: " << newNode->data << endl;
    
    // Creating a node using a constructor and passing it by reference to a function
    Node newnode(15, NULL);
    cout << "Original node data: " << newnode.data << endl;
    
    modifyNode(newnode);
    cout << "Modified node data: " << newnode.data << endl;
    
    return 0;
}
