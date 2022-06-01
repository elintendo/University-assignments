#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

void makeArray(std::string array, std::vector<std::string> &final)
{
    std::string z = "";
    for (int i = 0; i < array.size(); ++i)
    {
        if (array[i] != ' ')
        {
            z += array[i];
        }
        else
        {
            final.push_back(z);
            z = "";
            continue;
        }
    }
    final.push_back(z);
}

int main()
{
    std::ifstream tt("input.txt");
    std::string s;
    std::vector<std::string> v;
    std::vector<std::string> final;
    while (getline(tt, s))
        v.push_back(s);
    tt.close();
    std::string string = v[0];
    std::string array = v[2];
    makeArray(array, final);

    std::sort(final.begin(), final.end());
    std::vector<std::string> wow;

    do
    {
        for (auto x : final)
            wow.push_back(x);
    } while (std::next_permutation(final.begin(), final.end()));

    std::vector<std::string> finall;
    std::string t = "";
    int k = 0;
    for (int i = 0; i < wow.size(); i++)
    {
        t += wow[i];
        if (k == final.size() - 1)
        {
            finall.push_back(t);
            t = "";
            k = -1;
        }
        k++;
    }

    std::vector<int> output;
    std::string ss = "";
    for (int i = 0; i < finall.size(); i++)
    {
        if (string.find(finall[i]) != std::string::npos)
            output.push_back(string.find(finall[i]));
    }
    std::sort(output.begin(), output.end());

    for (auto x : output)
    {
        ss += std::to_string(x);
        ss += " ";
    }
    ss.pop_back();
    std::ofstream myfile("output.txt");
    myfile << ss;
    myfile.close();
}
