/* 2.2 Range Queries
 * Ivan Orekhov, 2 Group
 * https://codeforces.com/group/M5kRwzPJlU/contest/376242/submission/154792589
*/

#include <iostream>
#include <sstream>
#include <vector>

#define MIN(a, b) (a <= b ? a : b)
#define MAX(a, b) (a >= b ? a : b)

// Interface for RangeMap
template<typename K, typename V>
class RangeMap {
public:
    virtual int size() = 0;
    virtual bool isEmpty() = 0;
    virtual void add(K key, V value) = 0;
    virtual bool contains(K key) = 0;
    virtual V lookup(K key) = 0;
    virtual std::vector<V> lookupRange(K from, K to) = 0;
};

/*
 * Class Record is responsible for storing transactions.
 * Instance of class is a transaction that has key (date) and value (amount of money spent)
 * Also, each record has pointers to previous and next records.
 */
class Record {
public:
    int key;
    int value;
    Record *prev;
    Record *next;

    Record(int key, int value) {
        this->prev = nullptr;
        this->next = nullptr;
        this->key = key;
        this->value = value;
    }
};

// B-tree's order is going to be '10'.
const int t = 10;

/*
 * Class BTreeNode is responsible for storing records (instances of class Record)
 * Each node has a vector of keys (vector of records) and a vector of children (nodes).
 * A vector of records has 2 * t - 1 capacity and a vector of children has 2 * t capacity
 * according to Cormen's definition.
 */
template<typename K, typename V>
class BTreeNode {
public:
    std::vector<Record *> keys; // Vector of node's keys
    std::vector<BTreeNode *> C; // Vector of node's children
    int n;                    // Current number of node's keys
    int d;                    // Current number of node's children

    BTreeNode() {
        keys = std::vector<Record *>(2 * t - 1, nullptr);
        C = std::vector<BTreeNode *>(2 * t, nullptr);
        n = 0;
        d = 0;
    }

    bool isLeaf() {
        return this->d == 0;
    }
};

/*
 * Class BTree is a class used to implement all required methods of
 * a self-balancing tree data structure.
 */
template<typename K, typename V>
class BTree : public RangeMap<K, V> {
    std::vector<int> output;

    int m_size = 0;
    BTreeNode<K, V> *root;

public:
    explicit BTree<K, V>() {
        root = new BTreeNode<K, V>();
    }

    int size() override {
        return m_size;
    }

    bool isEmpty() override {
        if (m_size == 0) return true;
        else return false;
    }

    /*
     * The given method lookups values for a range of keys.
     * Initially, we search for the first key, Then, we traverse
     * through all keys by using "next" pointer.
     */
    std::vector<V> lookupRange(K from, K to) override {
        std::vector<V> output;
        Record *start = search(root, from);
        if (start == nullptr) add(from, 0);
        start = search(root, from);
        while (start != nullptr) {
            if (start->key - to <= 0) {
                output.push_back(start->value);
                start = start->next;
            } else break;
        }
        return output;
    }

    // If search method returns null pointers, then the given key is absent.
    bool contains(K key) override {
        if(search(root, key) == nullptr) return false;
        return true;
    }

    // The given method allows to look up a value by the key.
    V lookup(K key) override {
        auto x = search(root, key);
        if(contains(key)) return x->value;
        else return -1;
    }

    /*
     * This method allows to search for a given record in a BTree
     * with a given root and node's key.
     */
    Record *search(BTreeNode<K, V> *node, K k) {
        int i = 0;
        while (i < node->n && k > node->keys[i]->key)
            i++;

        if (i < node->n && k == node->keys[i]->key)
            return node->keys[i];

        if (node->isLeaf())
            return nullptr;

        return search(node->C[i], k);
    }

    /*
     * This method is one of the three methods (parallel with InsertNonfull and SplitByChild)
     * that works together to insert a new node into the BTree.
     * We insert a record in a BTree (key and value) in a single pass.
     * If-statement handles a case in which the root node is full: the root splits and a new node
     * that has two children becomes new root.
     */
    void add(K k, V v) override {
        Record *duplicate = search(root, k);
        if (duplicate != nullptr) {
            duplicate->value += v;
            return;
        }

        if (root->n >= 2 * t - 1) {
            BTreeNode<K, V> *temp = new BTreeNode<K, V>();
            temp->C[0] = root;
            temp->d = 1;
            root = temp;
            BTreeSplitChild(temp, 0);
            BTreeInsertNonfull(temp, k, v);
        } else
            BTreeInsertNonfull(root, k, v);
    }

    /*
     * Firstly, we handle a case in which the node is a leaf by inserting record (k, v) into node.
     * If node is not a leaf, then we have to insert record (k, v) into the appropriate leaf node
     * in the subtree rooted at internal node.
     */
    void BTreeInsertNonfull(BTreeNode<K, V> *node, int k, int v) {
        auto *record = new Record(k, v);
        int i = node->n - 1;
        if (node->isLeaf()) {
            while ((i >= 0) && (k < node->keys[i]->key)) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            m_size++;
            node->keys[i + 1] = record;
            node->n++;

            /*
             * The following two if-statements are used to rearrange records' pointers.
             * The pointer "prev" points to the element with smaller key.
             * The pointer "next" points to the element with bigger key.
             */
            if (i >= 0) {
                node->keys[i + 1]->next = node->keys[i]->next;
                node->keys[i + 1]->prev = node->keys[i];
                node->keys[i]->next = record;
                if (record->next != nullptr)
                    record->next->prev = record;
            }

            if (i + 2 < node->n) {
                record->next = node->keys[i + 2];
                if (node->keys[i + 2]->prev != nullptr && node->keys[i + 2]->prev != record) {
                    record->prev = node->keys[i + 2]->prev;
                    node->keys[i + 2]->prev->next = record;
                }
                node->keys[i + 2]->prev = record;
            }
        } else {
            while ((i >= 0) && (k < node->keys[i]->key))
                i--;
            i++;
            if ((node->C[i])->n == 2 * t - 1) {
                BTreeSplitChild(node, i);
                if (record->key > node->keys[i]->key)
                    i++;
            }
            BTreeInsertNonfull(node->C[i], k, v);
        }
    }

    /*
     * The given method is used to split a child in two and adjust its parent
     * so that it has an additional child.
     */
    void BTreeSplitChild(BTreeNode<K, V> *node, int i) {
        BTreeNode<K, V> *y = node->C[i];
        BTreeNode<K, V> *z = new BTreeNode<K, V>();

        // Insert into parent
        auto it1 = node->C.begin();
        node->C.insert(it1 + i + 1, z);

        auto it2 = node->keys.begin();
        node->keys.insert(it2 + i, y->keys[t - 1]);

        node->n++;
        node->d++;

        int zKeysCount = MAX(0, y->n - t);

        for (int j = 0; j < zKeysCount; ++j) {
            z->keys[j] = y->keys[j + t];
        }
        z->n = zKeysCount;
        y->n = MIN(y->n, t - 1);

        if (!y->isLeaf()) {
            int zDegree = MAX(0, y->d - t);
            for (int j = 0; j < zDegree; ++j) {
                z->C[j] = y->C[j + t];
            }
            z->d = zDegree;
            y->d = MIN(y->d, t);
        }
    }

    /*
     * This method is used to count all records' values from one date to another.
     * The same logic was described in aforementioned lookupRange method.
     */
    void Report(int From, int To) {
        Record *start = search(root, From);
        if (start == nullptr) add(From, 0);
        start = search(root, From);
        int amount = 0;
        while (start != nullptr) {
            if (start->key - To <= 0) {
                amount += start->value;
                start = start->next;
            } else break;
        }
        output.push_back(amount);
    }

    // The given method reads from the standard output and initializes BTree's members and methods.
    void InitializeTree() {
        std::string line;
        std::getline(std::cin, line);
        std::stringstream str(line);
        int N;
        str >> N;

        for (int i = 0; i < N; i++) {
            std::getline(std::cin, line);
            if (line[0] == 'R') {
                int From = std::stoi(line.substr(12, 4) + line.substr(17, 2) + line.substr(20, 2));
                int To = std::stoi(line.substr(26, 4) + line.substr(31, 2) + line.substr(34, 2));
                Report(From, To);
            } else {
                int coeff;
                int value;
                std::string d_date = line.substr(0, 10);
                int date = std::stoi(d_date.substr(0, 4) + d_date.substr(5, 2) + d_date.substr(8, 2));
                if (line[11] == 'D')
                    coeff = 1;
                else
                    coeff = -1;

                for (int q = 12; q < line.size(); ++q) {
                    if (line[q] == ' ')
                        value = std::stoi(line.substr(q + 1, line.size() - q - 1)) * coeff;
                }
                add(date, value);
            }
        }
        for (auto x: output) std::cout << x << '\n';
    }
};

int main() {
    BTree<int, int> T;
    T.InitializeTree();
}
