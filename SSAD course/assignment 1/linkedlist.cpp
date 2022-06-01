#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

void splitbySpace(std::string s, std::vector<int> &v)
{
    std::string integer = "";
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] != ' ')
        {
            integer += s[i];
        }
        else
        {
            v.push_back(std::atoi(integer.c_str()));
            integer = "";
        }
    }
    v.push_back(std::atoi(integer.c_str()));
}

struct Node
{
    Node *prev = nullptr;
    Node *next = nullptr;
    int data;
};

class LinkedList
{
public:
    int size = 0;
    Node *last = nullptr;
    Node *first = nullptr;

    void createNewNode(int data)
    {
        Node *node = new Node();
        node->data = data;
        if (last == nullptr)
        {
            last = node;
            first = node;
            node->next = nullptr;
        }
        else
        {
            last->prev = node;
            node->next = last;
            last = node;
        }
        size++;
    }

    void swapNodes()
    {
        Node *curNode = first;
        for (int i = 0; i < size - 1; i += 2)
        {
            Node *prev = curNode->prev;
            Node *next = curNode->next;
            Node *beforePrev = curNode->prev->prev;

            if (next != nullptr)
                next->prev = prev;
            if (beforePrev != nullptr)
                beforePrev->next = curNode;

            curNode->prev = beforePrev;
            curNode->next = prev;
            prev->next = next;
            prev->prev = curNode;

            if (i == 0)
                first = prev;
            curNode = beforePrev;
        }

        if (curNode != nullptr)
            last = curNode;
    }

    std::string print()
    {
        std::string output = "";
        Node *temp = first;
        for (int i = 0; i < size; i++)
        {
            output += std::to_string(temp->data);
            output += " ";
            temp = temp->prev;
        }
        output.pop_back();
        return output;
    }
};

int main()
{
    std::ifstream tt("input.txt");
    std::stringstream line;
    line << tt.rdbuf();
    std::vector<int> v;
    std::string ss = line.str();
    ss = ss.substr(0, ss.size());
    splitbySpace(ss, v);

    LinkedList l;
    for (int i = 0; i < v.size(); i++)
    {
        l.createNewNode(v[i]);
    }
    l.swapNodes();

    std::ofstream myfile("output.txt");
    myfile << l.print();
    myfile.close();
}
