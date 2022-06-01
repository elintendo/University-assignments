#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

std::string takeLine(std::string s)
{
    std::string ss = "";
    bool can = false;

    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == '=')
        {
            can = true;
            continue;
        }
        if (can)
            ss += s[i];
    }
    ss.pop_back();
    return ss;
}

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

int Kadane(std::vector<int> &v)
{
    int maxCurrent, maxGlobal;
    maxCurrent = maxGlobal = v[0];

    for (int i = 1; i < v.size(); i++)
    {
        (v[i] > maxCurrent + v[i]) ? (maxCurrent = v[i]) : (maxCurrent = maxCurrent + v[i]);
        if (maxCurrent > maxGlobal)
            maxGlobal = maxCurrent;
    }
    return maxGlobal;
}

int main()
{
    std::ifstream tt("input.txt");
    std::stringstream line;
    line << tt.rdbuf();
    std::vector<int> v;
    std::string ss = line.str();
    ss = ss.substr(0, ss.size());

    splitbySpace(ss, v);

    std::ofstream myfile("output.txt");
    myfile << Kadane(v);
    myfile.close();
}
