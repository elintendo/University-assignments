// Ivan Orekhov, II group
// https://codeforces.com/group/M5kRwzPJlU/contest/368875/submission/146880612

#include <iostream>

template<class T>
class ICircularBoundedQueue {
public:
    virtual void offer(T value) = 0;

    virtual T poll() = 0;

    virtual T peek() = 0;

    virtual void flush() = 0;

    virtual bool isEmpty() = 0;

    virtual bool isFull() = 0;

    virtual int size() = 0;

    virtual int capacity() = 0;
};

/* My bounded queue is based on doubly linked list.
 *
 */

template<class T>
class DoublyLinkedCircularBoundedQueue : public ICircularBoundedQueue<T> {
private:
    int msize;
    int mcapacity;
    struct Node {
    public:
        Node *prev = nullptr;
        T data;
    };

public:
    Node *last = nullptr;
    Node *first = nullptr;

    explicit DoublyLinkedCircularBoundedQueue(int capacity) {
        mcapacity = capacity;
        msize = 0;
    }

    void offer(T value) override {
        if ((isFull())) {
            poll();
        }

        Node *newNode = new Node();
        newNode->data = value;
        newNode->prev = nullptr;

        if (last == nullptr) {
            last = newNode;
            first = newNode;
            msize++;
            return;
        }
        last->prev = newNode;
        last = newNode;
        msize++;
    }


    T poll() override {
        Node *oldFirst = first;
        T x = first->data;
        if (oldFirst->prev == nullptr) {
            first = last = nullptr;
            return x;
        } else {
            //((oldFirst)->prev)->next = nullptr;
            first = oldFirst->prev;
        }
        delete oldFirst;
        if (msize > 0) msize--;
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

template<class T>
class IBoundedStack {
    virtual void push(T value) = 0;

    virtual T pop() = 0;

    virtual T top() = 0;

    virtual void flush() = 0;

    virtual void isEmpty() = 0;

    virtual void isFull() = 0;

    virtual void size() = 0;

    virtual void capacity() = 0;
};

template<class T>
class QueuedBoundedStack : IBoundedStack<T> {
    int msize, mcapacity;
    DoublyLinkedCircularBoundedQueue<T> queue1, queue2;

    explicit QueuedBoundedStack(T capacity) {
        msize = 0;
        mcapacity = capacity;
        queue1 = DoublyLinkedCircularBoundedQueue<T>(mcapacity);
        queue2 = DoublyLinkedCircularBoundedQueue<T>(mcapacity);
    }

    void push(T value) override {
        msize++;
        queue2.offer(value);

        // Push all the remaining
        // elements in q1 to q2.
        while (!queue1.empty()) {
            queue2.push(queue1.front());
            queue2.pop();
        }

        // swap the names of two queues
        DoublyLinkedCircularBoundedQueue<int> q = queue1;
        queue1 = queue2;
        queue2 = q;
    }

    T pop() override {
        // if no elements are there in q1
        if (queue1.empty())
            return;
        queue1.peek();
    }

    T top() override {
        if (queue1.empty())
            return;
        return queue1.peek();
    }

    void flush() override {
        queue1.flush();
    }

    bool isEmpty() override {
        if (queue1.first == nullptr) return true;
        return false;
    }

    bool isFull() override {
        if (msize == mcapacity) return true;
        return true;
    }

    int size() override { return msize; }

    int capacity() override { return mcapacity; }

};

int main() {
    std::string line;
    int n, k;
    std::cin >> n >> k;
    getline(std::cin, line);
    DoublyLinkedCircularBoundedQueue<std::string> q = DoublyLinkedCircularBoundedQueue<std::string>(k);

    for (int i = 0; i < n; i++) {
        getline(std::cin, line);
        q.offer(line);
    }

    int t = q.size();

    for(int i = 0; i < t; i++) {
        std::cout << q.poll() << std::endl;
    }
}