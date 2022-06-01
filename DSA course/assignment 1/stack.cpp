// Ivan Orekhov, II group
// 2.1.2. Bounded Stack

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

// The bounded stack is built with two Circular Bounded Queues. 
// All time complexities written below are worst-case time complexity
template<class T>
class IBoundedStack {
    virtual void push(T value) = 0;
    // Time complexity O(n). Push method is used to put an element on the stack. 
    // Also, it removes the oldest element if the stack is full.
    virtual T pop() = 0;
    // Time complexity O(1). Pop methods removes the top element from the queue.
    virtual T top() = 0;

    virtual void flush() = 0; // removes all elements from the queue

    virtual bool isEmpty() = 0; // shows whether the stack is empty 

    virtual bool isFull() = 0; // shows whether the stack is full

    virtual int size() = 0; // returns size of the stack

    virtual int capacity() = 0; // shows the maximum capacity
};

template<class T>
class QueuedBoundedStack : IBoundedStack<T> {
    int msize, mcapacity;
    DoublyLinkedCircularBoundedQueue<T> queue1, queue2;

    explicit QueuedBoundedStack(int capacity) {
        msize = 0;
        mcapacity = capacity;
        queue1 = DoublyLinkedCircularBoundedQueue<T>(mcapacity);
        queue2 = DoublyLinkedCircularBoundedQueue<T>(mcapacity);
    }

    void push(T value) override {
        msize++;
        queue2.offer(value);

        // Push remaining elements from queue1 to queue2.
        while (!queue1.empty()) {
            queue2.push(queue1.front());
            queue2.pop();
        }

        // Here we swap the names of two queues.
        DoublyLinkedCircularBoundedQueue<int> q = queue1;
        queue1 = queue2;
        queue2 = q;
    }

    T pop() override {
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