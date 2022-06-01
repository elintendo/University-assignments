#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int sizeofMatrix(std::string s)
{
    int size = 0;
    for (auto x : s)
    {
        if (x == '{')
            size++;
    }
    return size;
}

std::vector<std::vector<int>> linetoVector(std::string &s) // transforms a line to 2D vector
{
    std::vector<std::vector<int>> v;
    std::vector<int> vh;
    s = s.substr(1, s.length() - 2);
    int n = sizeofMatrix(s);

    for (auto x : s)
    {
        if ((x == '{') || (x == ',') || (x == '}'))
            continue;
        else
            vh.push_back(x - '0');

        if (vh.size() == n)
        {
            v.push_back(vh);
            vh.clear();
        }
    }
    return v;
}

void subMatrix(std::vector<std::vector<int>> mat, std::vector<std::vector<int>> &temp, int p, int q, int size)
{
    int n = size;
    int i = 0, j = 0;
    for (int row = 1; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            if (row != p && col != q)
            {
                temp[i][j] = mat[row][col];
                j++;
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

int determinantOfMatrix(std::vector<std::vector<int>> v, int n)
{
    int det = 0;

    if (n == 1)
        return v[0][0];
    if (n == 2)
        return (v[0][0] * v[1][1]) - (v[0][1] * v[1][0]);
    std::vector<std::vector<int>> temp(n, std::vector<int>(n, 0));
    int sign = 1;
    for (int i = 0; i < n; i++)
    {
        subMatrix(v, temp, 0, i, n);
        det += sign * v[0][i] * determinantOfMatrix(temp, n - 1);
        sign = -sign;
    }

    return det;
}

int main()
{

    std::ifstream tt("input.txt");
    std::stringstream stream;
    stream << tt.rdbuf();
    std::string s = stream.str();
    std::vector<std::vector<int>> v = linetoVector(s);
    int det = determinantOfMatrix(v, sizeofMatrix(s));
    std::cout << det;

    std::ofstream myfile("output.txt");
    myfile << det;
    myfile.close();
}
