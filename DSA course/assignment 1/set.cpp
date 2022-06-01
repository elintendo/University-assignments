// Ivan Orekhov, group II
// 2.1.3 Set with double hashing.
// All time complexities written below are worst-case time complexity

template <class T>
class ISet
{
public:
    virtual void add(T item) = 0; // Method is used to add item in the set
    // Time complexity O(n)
    virtual void remove(T item) = 0; // Method is used to remove an item from a set
    // Time complexity O(n)
    virtual bool contains(T item) = 0; // Method is used to find out whether an item is in the set
    // Time complexity O(n)
    virtual int size() = 0; // Method returns size of the set
    // Time complexity O(1)
    virtual bool isEmpty() = 0; // Method is used to check whether a set is empty
    // Time complexity O(1)
};

template <class T>
class DoubleHashSet : public ISet<T>
{
private:
    int msize, keys;
    int index, offset;
    T *hashTable;
    bool *uninitializedFields;
    bool *removedFields;
    std::hash<T> hashFunc;
    int hashcode1, hashcode2, doublehash;

    int doubleHash(int i)
    {
        return abs(hashcode1 + i * hashcode2) % msize;
    }

public:
    void initialize(T item)
    {
        int key = hashFunc(item);
        hashcode1 = key % msize;
        key = ((key >> 16) ^ key) * 0x45d9f3b;
        key = ((key >> 16) ^ key) * 0x45d9f3b;
        key = (key >> 16) ^ key;
        hashcode2 = key;
    }

    explicit DoubleHashSet(int size)
    {
        hashFunc = std::hash<T>();
        hashTable = new T[size];
        uninitializedFields = new bool[size];
        removedFields = new bool[size];
        msize = size;
        keys = 0;

        for (int i = 0; i < msize; i++)
        {
            uninitializedFields[i] = true;
            removedFields[i] = false;
        }
    }

    ~DoubleHashSet()
    {
        delete[] uninitializedFields, removedFields, hashTable;
    }

    void add(T item) override
    {
        initialize(item);

        for (int i = 0; i < msize; ++i)
        {
            doublehash = doubleHash(i);

            if (uninitializedFields[doublehash])
            {
                hashTable[doublehash] = item;
                keys++;
                uninitializedFields[doublehash] = false;
                removedFields[doublehash] = false;
                return;
            }
            else if (hashTable[doublehash] == item)
                return;
        }
    }

    void remove(T item) override
    {
        initialize(item);

        for (int i = 0; i < msize; i++)
        {
            doublehash = doubleHash(i);

            if (removedFields[doublehash])
            {
                continue;
            }
            else if (uninitializedFields[doublehash])
            {
                throw ItemNotFoundException();
            }
            else if (hashTable[doublehash] == item)
            {
                keys--;
                uninitializedFields[doublehash] = true;
                removedFields[doublehash] = true;
                return;
            }
        }
    }

    bool contains(T item) override
    {
        initialize(item);

        for (int i = 0; i < msize; i++)
        {
            doublehash = doubleHash(i);
            if (removedFields[doublehash])
                continue;
            else if (uninitializedFields[doublehash])
            {
                return false;
            }
            else if (hashTable[doublehash] == item)
            {
                return true;
            }
        }
        return false; 
    }

    int size() override
    {
        return keys;
    }

    bool isEmpty() override
    {
        return keys == 0;
    }

};