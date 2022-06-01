// Ivan Orekhov, II group
// https://codeforces.com/group/M5kRwzPJlU/contest/368875/submission/147076428

#include <iostream>
#include <cmath>
#include <exception>
#include <sstream>

class ItemNotFoundException : std::exception {};

template <class T>
class ISet
{
public:
    virtual void add(T item) = 0;

    virtual void remove(T item) = 0;

    virtual bool contains(T item) = 0;

    virtual int size() = 0;

    virtual bool isEmpty() = 0;
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

    bool isPrime(int x)
    {
        for (int i = 2; i <= ceil(sqrt(x)); i++)
        {
            if (x % i == 0)
                return false;
        }
        return true;
    }

    int findLessPrime(int x)
    {
        for (int i = x - 1; i > 1; i--)
        {
            if (isPrime(i))
                return i;
        }
        return -1;
    }

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
            return; // throw smth
    }
    }

    void remove(T item) override
    {
        initialize(item);

        for (int i = 0; i < msize; i++)
        {
            doublehash = doubleHash(i);

            if (removedFields[doublehash]) {
                continue;
            }
            else if (uninitializedFields[doublehash]) {
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

    std::string list()
    {
        std::string s = "";
        for (int i = 0; i < msize; i++)
        {
            if (!uninitializedFields[i])
            {
                s += hashTable[i];
                s += ' ';
            }
        }
        return s;
    }
};

struct Pair
{
    std::string first, second;

    Pair()
    {
        first = "";
        second = "";
    }

    Pair(std::string line)
    {
        bool firstFound = false;
        std::string word = "";
        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] == ' ')
            {
                first = word;
                firstFound = true;
                word = "";
            }
            else
                word += line[i];
        }
        if (firstFound)
            second = word;
        else
            first = word;
    }

};


std::string cutSlash(std::string s)
{
    if (s[s.length() - 1] == '/')
        s.pop_back();
    return s;
}


std::string addSlash(std::string s)
{
    if (s[s.length() - 1] != '/')
        s += '/';
    return s;
}


int main()
{
    std::string first, second;
    std::string line;
    int n;
    std::cin >> n;
    getline(std::cin, line);
    DoubleHashSet<std::string> set(1000003);
    Pair pair;
    for (int i = 0; i < n; i++)
    {
        getline(std::cin, line);
        pair = Pair(line);

        try {
            if ((pair.first == "NEW")) {
                if(set.contains(cutSlash(pair.second))||(set.contains(addSlash(pair.second)))) throw std::exception();
                set.add(pair.second);
            }
            else if (pair.first == "REMOVE") {
                set.remove(pair.second);
            }
            else if (pair.first == "LIST")
                std::cout << set.list() << std::endl;
        } catch (...) {
            std::cout << "ERROR: cannot execute " << line << std::endl;
        }
    }
}
