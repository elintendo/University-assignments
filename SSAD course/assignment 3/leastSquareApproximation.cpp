#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

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

double determinantOfMatrix(std::vector<std::vector<double>> v, int n)
{
    double det = 0.0;

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
        // std::cout << "! " << det << "! ";
        sign = -sign;
    }

    return det;
}

std::vector<std::vector<double>> transpose(std::vector<std::vector<double>> adj, int row, int col)
{
    std::vector<std::vector<double>> temp(col + 1, std::vector<double>(row));
    for (int i = 0; i < col + 1; i++)
    {
        for (int j = 0; j < row; j++)
        {
            temp[i][j] = adj[j][i];
        }
    }
    return temp;
}

// void inverseMatrix(std::vector<std::vector<double>> mat, std::vector<std::vector<double>> &temp, int s)
// {
//     std::vector<std::vector<double>> adj(s, std::vector<double>(s));
//     int i = 0, j = 0;
//     int sign = 1;
//     for (int row = 0; row < s; row++)
//     {
//         for (int col = 0; col < s; col++)
//         {
//             subMatrix(mat, temp, row, col, s);
//             adj[row][col] = sign * determinantOfMatrix(temp, s - 1);
//             sign = -sign;
//         }
//     }
//     temp = transpose(adj, s, s);

//     double coeff = ((double)1 / (determinantOfMatrix(mat, s)));
//     for (int row = 0; row < s; row++)
//     {
//         for (int col = 0; col < s; col++)
//         {
//             temp[row][col] = (double)temp[row][col] * coeff;
//         }
//     }
// }

void InverseOfMatrix(std::vector<std::vector<double>> &a, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            a[i].push_back(0.0);
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
            {
                a[i][j + n] = 1;
            }
            else
            {
                a[i][j + n] = 0;
            }
        }
    }

    float r;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i != j)
            {
                r = a[j][i] / a[i][i];
                for (int k = 1; k < 2 * n; k++)
                {
                    a[j][k] = a[j][k] - r * a[i][k];
                }
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = n; j < 2 * n; j++)
        {
            a[i][j] = a[i][j] / a[i][i];
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = n; j < 2 * n; j++)
        {
            a[i][j % n] = a[i][j];
        }
    }
}

std::vector<std::vector<double>> mult(std::vector<std::vector<double>> &A, std::vector<std::vector<double>> &B,
                                      int a, int b, int c)
{
    // A axb, B bxc => AxB
    std::vector<std::vector<double>> res(a, std::vector<double>(c)); // is it full of zeros?

    for (int i = 0; i < a; ++i)
        for (int j = 0; j < c; ++j)
            for (int k = 0; k < b; ++k)
                res[i][j] += A[i][k] * B[k][j];
    return res;
}

std::vector<double> v_mult(std::vector<std::vector<double>> &A, std::vector<double> &B,
                           int a, int b)
{
    // A axb, B bxc => AxB
    std::vector<double> res(a); // is it full of zeros?

    for (int i = 0; i < a; ++i)
        for (int k = 0; k < b; ++k)
            res[i] += A[i][k] * B[k];
    return res;
}

void readfirstLine(int &n, int &m)
{
    std::string s = "";
    std::ifstream file("input.txt");
    std::string str;
    std::vector<std::string> v;

    std::getline(file, str);
    file.close();
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[i] != ' ')
            s += str[i];
        else
        {
            v.push_back(s);
            s = "";
        }
    }
    v.push_back(s);

    n = std::atoi(v[0].c_str());
    m = std::atoi(v[1].c_str());
}

void initializeMatrices(std::vector<std::vector<double>> &A, std::vector<double> &b)
{
    std::ifstream file("input.txt");
    std::string str;
    std::getline(file, str);

    std::vector<std::string> g;
    std::string s = "";
    int rows = 0;
    int columns = 1;

    while (std::getline(file, str))
    {
        for (int i = 0; i < str.size(); i++)
        {
            if (str[i] != ' ')
                s += str[i];
            else
            {
                g.push_back(s);
                s = "";
            }
        }
        g.push_back(s);
        s = "";

        for (int i = 0; i < g.size() - 1; ++i)
        {
            A[rows][columns] = atof(g[i].c_str());
            columns++;
        }
        b[rows] = atof(g[g.size() - 1].c_str());

        rows++;
        columns = 1;
        g.clear();
    }

    file.close();
}

int main()
{
    int n, m;
    readfirstLine(n, m);
    std::vector<std::vector<double>> A(m, std::vector<double>(n + 1));
    std::vector<double> b(m);

    for (int i = 0; i < m; i++)
        A[i][0] = 1.00;
    initializeMatrices(A, b);

    freopen("output.txt", "w", stdout);
    std::cout << std::fixed << std::showpoint;
    std::cout << std::setprecision(2);

    std::cout << "A:" << std::endl;
    for (int i = 0; i < A.size(); i++)
    {
        for (int j = 0; j < A[i].size(); j++)
        {

            std::cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "b: " << std::endl;
    for (int i = 0; i < b.size(); i++)
    {
        std::cout << b[i] << " ";
        std::cout << std::endl;
    }

    std::vector<std::vector<double>> A_T = transpose(A, m, n);
    std::vector<std::vector<double>> A_T_A = mult(A_T, A, n + 1, m, n + 1);

    std::cout << std::endl;
    std::cout << "A_T*A:" << std::endl;
    for (int i = 0; i < A_T_A.size(); i++)
    {
        for (int j = 0; j < A_T_A[i].size(); j++)
        {
            std::cout << A_T_A[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::vector<std::vector<double>> inverse = A_T_A;
    InverseOfMatrix(inverse, n + 1);
    std::cout << std::endl;
    std::cout << "(A_T*A)_-1:" << std::endl;
    for (int i = 0; i < n + 1; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            std::cout << inverse[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "(A_T*A)_-1*A_T:" << std::endl;
    std::vector<std::vector<double>> final = mult(inverse, A_T, n + 1, n + 1, m);
    for (int i = 0; i < final.size(); i++)
    {
        for (int j = 0; j < final[i].size(); j++)
        {
            std::cout << final[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "x:" << std::endl;
    std::vector<double> x = v_mult(final, b, n + 1, m);
    for (int i = 0; i < x.size(); i++)
        std::cout << x[i] << std::endl;

    fclose(stdout);
}
