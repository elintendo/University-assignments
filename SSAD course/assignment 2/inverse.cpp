#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cmath>
#define N 3

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

std::vector<std::vector<double>> linetoVector(std::string s) // transforms a line to 2D vector
{
    std::vector<std::vector<double>> v;
    std::vector<double> vh;
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

void subMatrix(std::vector<std::vector<double>> mat, std::vector<std::vector<double>> &temp, int p, int q, int size)
// p = row, q = column
{
    int n = size;
    int i = 0, j = 0;
    for (int row = 0; row < n; row++)
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

int determinantOfMatrix(std::vector<std::vector<double>> v, int n)
{
    int det = 0;

    if (n == 1)
        return v[0][0];
    if (n == 2)
        return (v[0][0] * v[1][1]) - (v[0][1] * v[1][0]);
    std::vector<std::vector<double>> temp(n, std::vector<double>(n, 0));
    int sign = 1;
    for (int i = 0; i < n; i++)
    {
        subMatrix(v, temp, 0, i, n);
        det += sign * v[0][i] * determinantOfMatrix(temp, n - 1);
        sign = -sign;
    }

    return det;
}

void adjacentMatrix(std::vector<std::vector<double>> mat, std::vector<std::vector<double>> &temp)
{
    int adj[N][N];
    // adjacent, then signs
    int i = 0, j = 0;
    int sign = 1;
    for (int row = 0; row < N; row++)
    {
        for (int col = 0; col < N; col++)
        {
            subMatrix(mat, temp, row, col, N);
            adj[row][col] = sign * determinantOfMatrix(temp, N - 1);
            sign = -sign;
        }
    }

    // then rotate
    for (int row = 0; row < N; row++)
    {
        for (int col = 0; col < N; col++)
        {
            temp[row][col] = adj[col][row];
        }
    }
    double coeff = ((double)1 / (determinantOfMatrix(mat, N)));

    for (int row = 0; row < N; row++)
    {
        for (int col = 0; col < N; col++)
        {
            temp[row][col] = (double)temp[row][col] * coeff;
        }
    }
}

int main()
{
    std::ifstream tt("input.txt");
    std::stringstream stream;
    stream << tt.rdbuf();
    std::string s = stream.str();

    freopen("output.txt", "w", stdout);

    std::vector<std::vector<double>> mat = linetoVector(s);
    std::vector<std::vector<double>> temp(N, std::vector<double>(N, 0));

    adjacentMatrix(mat, temp);
    std::string output = "";
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (temp[i][j] == -0.00)
            {
                temp[i][j] = 0.00;
            }
            printf("%.2f ", temp[i][j]);
            if (j == N - 1)
                std::cout << '\n';
        }
    }

    std::cout << output;
    fclose(stdout);
}
