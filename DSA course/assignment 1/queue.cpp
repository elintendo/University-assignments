// Ivan Orekhov, group II
// 2.1.1 Circular Bounded Queue.
// All time complexities written below are worst-case time complexity

#include <iostream>

template<class T>
class ICircularBoundedQueue {
public:
    virtual void offer(T value) = 0; 
    // The method offer has time complexity of O(1). The method is used to add an element
    // to a rear of a queue. Also, it overwrites the oldest elements when the queue is full.

    virtual T poll() = 0; 
    // The method poll has time complexity of O(1). The method is used to remove the last element
    // from the queue.

    virtual T peek() = 0; 
    // The method peek has time complexity of O(1). The method is used to return the last element
    // from the queue

    virtual void flush() = 0; 
    // The method offer has time complexity of O(1). The method is used to remove all elements
    // from the queue.

    virtual bool isEmpty() = 0; 
    // The method offer has time complexity of O(1). The method is used to check whether the queue
    // is empty or not.

    virtual bool isFull() = 0; 
    // The method offer has time complexity of O(1). The method is used to check whether the queue
    // is full or not.

    virtual int size() = 0; // The method offer has time complexity of O(1). It returns the size of the queue

    virtual int capacity() = 0; // The method offer has time complexity of O(1). It returns the capacity of the queue.
};

template<class T>
class DoublyLinkedCircularBoundedQueue : public ICircularBoundedQueue<T> {
private:
    int msize;
    struct Node {
    public:
        Node *prev;
        Node *next;
        T data;
    };

    Node *getNewNode(T x) {
        Node *newNode = new Node();
        newNode->data = x;
        newNode->prev = nullptr;
        newNode->next = nullptr;
        return newNode;
    }
    // Here I implement the doubly linked list as the core of the bounded queue.
    // "Node" structure is used to store connected elements in the queue.

public:
    int mcapacity;
    Node *last = nullptr;
    Node *first = nullptr;

    explicit DoublyLinkedCircularBoundedQueue(int capacity) {
        mcapacity = capacity;
        msize = 0;
    }

    void offer(T value) override { 
        if (msize == mcapacity) {
            poll();
            offer(value);
        }

        Node *newNode = getNewNode(value);
        if (last == nullptr) {
            last = newNode;
            first = newNode;
            msize++;
            return;
        }
        last->prev = newNode;
        newNode->next = last;
        last = newNode;

        msize++;
    }

    T poll() override {
        Node *oldFirst = first;
        T x = oldFirst->data;
        if (oldFirst->prev == nullptr) first = last = nullptr;
        else {
            (oldFirst)->prev->next = nullptr;
            first = oldFirst->prev;
        }
        delete oldFirst;
        msize--;
        return x;
    }

    T peek() override {
        return first->data;
    }

    void flush() override {
        for (int i = 0; i < msize; i++) {
            poll();
        }
    }

    bool isEmpty() override {
        if (first == nullptr) return true;
        return false;
    }

    bool isFull() override {
        if (msize == mcapacity) return true;
        return false;
    }

    int size() override {
        return msize;
    }

    int capacity() override {
        return mcapacity;
    }
};
// The Main function shows that implementation works.
int main() 
{
    DoublyLinkedCircularBoundedQueue<int> q(5);
    q.offer(5);
    q.offer(3);
    q.poll();
    std::cout << q.peek();
}