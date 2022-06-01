/* 2.3 Car rental company
 * 2.3.1 Priority Queue
 * Ivan Orekhov, 2 Group
 * https://codeforces.com/group/M5kRwzPJlU/contest/376242/submission/154875808
*/

#include <iostream>
#include <vector>
#include <sstream>

/* Class Node is responsible for storing a name and a penalty number of a branch
 * Also, every instance of Node has index at which it stands in the vector "heap".
 * This vector is representation of a binary heap.
 */

class Node {
public:
    std::string name;
    int value;
    int index; // Index at which node stands in the vector

    Node(std::string name, int value) {
        this->name = name;
        this->value = value;
        this->index = -1;
    }
};

// The required interface for a binary heap.
// The typename V is absent because there is no usage of it in the code.
template<typename K>
class IPriorityQueue {
public:
    virtual int getSize() const = 0;
    virtual std::vector<Node *> getHeap() const = 0;
    virtual void insert(Node* item) = 0;
    virtual Node* findMin() = 0;
    virtual Node* extractMin() = 0;
    virtual void decreaseKey(Node* item, K newKey) = 0;
    virtual void Delete(Node* item) = 0;
    virtual void Union(const IPriorityQueue<K> &anotherQueue) = 0;
    // The methods "Delete" and "Union" were capitalized b) = 0ecause "union" and "delete" are reserved by C++ standard library.
};

/*
 * A priority queue is implemented as a binary heap.
 * The binary heap contains the vector "heap" that consists of pointers to Node.
 * Also, it contains "size" variable, that shows the number of elements in the binary heap.
 */

template<typename K>
class BinaryHeap : IPriorityQueue<K>{
private:
    int size;
    std::vector<Node *> heap;

public:

    int getSize() const override{
        return size;
    }

    std::vector<Node *> getHeap() const override {
        return heap;
    }

    // Constructor initializes the vector "heap" with lots of nullptr(s) to prevent time errors.
    explicit BinaryHeap(int maxSize) : heap(maxSize + 1, nullptr) {
        size = 0;
    }

    // returns the index of the parent node
    int parent(int i) {
        return (i - 1) / 2;
    }

    // returns the index of the left child
    int leftChild(int i) {
        return 2*i + 1;
    }

    // returns the index of the right child
    int rightChild(int i) {
        return 2*i + 2;
    }

    /* The current method decreases a key of a given node (it changes the node's value to a given one).
     * Then, the method swaps the node and its parent if necessary to adjust min-heap property.
    */
    void decreaseKey(Node* item, K newKey) override {
        item->value = newKey;
        int index = item->index;
        while(index >= 1 && heap[parent(index)] > heap[index]) {
            auto tmp = heap[index];
            heap[index] = heap[parent(index)] ;
            heap[parent(index)] = tmp;
            item->index = parent(index);
        }
    }

    // The current method removes a node from the binary heap
    void Delete(Node* item) override {
        heap[item->index] = heap[heap.size()]; // Replace the deleted node with the farthest right node
        heap.erase(heap.end()); // Delete the last node
        minHeapify(item->index); // Fix the heap
    }

    // The given method unites two binary heaps into one by inserting all nodes from one to another.
    void Union(const IPriorityQueue<K> &anotherQueue) override {
        auto m_heap = anotherQueue.getHeap();
        for (int i = 0; i < anotherQueue.getSize(); ++i)
            insert(m_heap[i]);
    }

    /*
     * This method inserts a node in the binary heap.
     * Firstly, we insert a new node at the end of the heap.
     * Then, we start to compare it with its parent and repeat until heap property is satisfied.
     * Also, we update indexes if two nodes were swapped.
     */
    void insert(Node* node) override{
        heap[size] = node;
        size++;

        node->index = size; // !

        int i = size - 1;
        while (i != 0 && heap[parent(i)]->value > heap[i]->value) {
            int parentI = parent(i);
            auto tmp = heap[i];
            heap[i] = heap[parentI];
            heap[parentI] = tmp;
            i = parentI;

            heap[i]->index = parentI; // !
            heap[parentI]->index = i; // !
        }
    }

    /* MinHeapify is a recursive algorithm that reorganizes the heap
     * to follow shape and heap properties.
     * If the smallest children of a given node is smaller than its parent, then swap them.
     * We proceed until heap property is satisfied.
     */
    void minHeapify(int i){
        int left = leftChild(i);
        int right;
        if(heap.size() >= 3)
            right = rightChild(i);
        else right = 0;

        int smallest = i;

        if (left <= size && heap[left]->value < heap[smallest]->value) {
            smallest = left;
        }

        if (right <= size && heap[right]->value < heap[smallest]->value) {
            smallest = right;
        }

        if (i != smallest) {
            auto temp = heap[i];
            heap[i] = heap[smallest];
            heap[smallest] = temp;
            minHeapify(smallest);

            heap[i]->index = smallest; // !
            heap[smallest]->index = i; // !
        }

    }

    // This method returns the smallest element of a binary heap.
    Node* findMin() override{
        return heap[0];
    }

    /* This method returns the smallest element of a binary heap,
     * removes it, and heapifies a tree.
    */
    Node* extractMin() override{
        Node* minItem = heap[0];
        heap[0] = heap[size - 1];
        heap[0]->index = 0; // !
        heap.pop_back();
        size--;
        minHeapify(0);
        return minItem;
    }

    /*
     * This method initializes the created heap, reads all branches' information from standard input.
     */
    void initializeHeap() {
        std::string line;
        std::getline(std::cin, line);
        std::stringstream str(line);
        int N;
        str >> N;

        for(int i = 0; i < N; ++i) {
            std::getline(std::cin, line);
            if(line[0] == 'P') std::cout << extractMin()->name << '\n';
            else {
                std::string name;
                long long int value;
                for(int i = 4; i < line.size(); ++i) {
                    if(line[i] == ' ') {
                        name = line.substr(4, i - 4);
                        value = stol(line.substr(i + 1, line.size() - i - 1));
                    }
                }
                Node* node = new Node(name, value);
                insert(node);
            }
        }
    }
};

int main() {
    BinaryHeap<int> h(5000000);
    h.initializeHeap();
}
