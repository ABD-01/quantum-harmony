#include <iostream>
#include <vector>

using namespace std;

class Heap {
private:
    vector<int> heapArray;

    void heapifyUp(int i) {
        if (i == 0) {
            return;
        }
        int parent = (i - 1) / 2;
        if (heapArray[parent] > heapArray[i]) {
            swap(heapArray[parent], heapArray[i]);
            heapifyUp(parent);
        }

        // iterative way
        // int parent = (i - 1) / 2;
        // while (i > 0 && heapArray[parent] > heapArray[i]) {
        //     swap(heapArray[parent], heapArray[i]);
        //     i = parent;
        // }

    }

    void heapifyDown(int i) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        if (left < heapArray.size() && heapArray[left] < heapArray[smallest]) {
            smallest = left;
        }
        if (right < heapArray.size() && heapArray[right] < heapArray[smallest]) {
            smallest = right;
        }
        if (smallest != i) {
            swap(heapArray[i], heapArray[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    Heap() {}

    void insert(int val) {
        heapArray.push_back(val);
        heapifyUp(heapArray.size() - 1);
    }

    int pop() {
        if (heapArray.empty()) {
            throw runtime_error("Heap is empty.");
        }
        int result = heapArray[0];
        heapArray[0] = heapArray.back();
        heapArray.pop_back();
        heapifyDown(0);
        return result;
    }

    int size() {
        return heapArray.size();
    }

    bool empty() {
        return heapArray.empty();
    }

    int top() {
        if (heapArray.empty()) {
            throw runtime_error("Heap is empty.");
        }
        return heapArray[0];
    }
};

int main() {
    Heap minHeap;

    minHeap.insert(3);
    minHeap.insert(1);
    minHeap.insert(4);
    minHeap.insert(1);
    minHeap.insert(5);
    minHeap.insert(9);

    while (!minHeap.empty()) {
        cout << minHeap.pop() << " ";
    }
    cout << endl;

    return 0;
}
