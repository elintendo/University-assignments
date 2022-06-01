/* 2.1 Simple Fraud Detection
 * Ivan Orekhov, 2 Group
 * https://codeforces.com/group/M5kRwzPJlU/contest/376242/submission/154623339
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// We need to implement the Date class to count difference between two days.
class Date
{
public:
    int d, m, y;
    Date() {} // Default constructor
    Date(int d, int m, int y) // Custom constructor
    {
        this->d = d;
        this->m = m;
        this->y = y;
    }
};

// The implementation of a year
int monthlyDays[12] = {31, 28, 31, 30, 31, 30,
                           31, 31, 30, 31, 30, 31};

// We have to count leap years (366 days) as well.
int countLeapYears(Date d)
{
    int years = d.y;

    if (d.m <= 2)
        years--;
    // The formula to count leap years under the given date.
    return years / 4 - years / 100 + years / 400;
}

// Now we can count difference between two dates.
int getDifference(Date d1, Date d2)
{
    int sum1 = d1.y * 365 + d1.d;
    for (int i = 0; i < d1.m - 1; i++)
        sum1 += monthlyDays[i];
    // We counted the amount of days since 0 BC of the given date.
    // The amount of leap years is equal to the amount of days we have to add.
    // So we just add leap years of the given date to the total amount.
    sum1 += countLeapYears(d1);

    // The same procedure for the second date.
    int sum2 = d2.y * 365 + d2.d;
    for (int i = 0; i < d2.m - 1; i++)
        sum2 += monthlyDays[i];
    sum2 += countLeapYears(d2);

    // Finally, count the difference between two dates.
    return sum2 - sum1;
}

// Class Transaction is responsible for storing transactions from input.
// Each instance of class has date, amount and "all" (merged day, month and year, e.g. 01/02/2003 -> 20030201)
class Transaction
{
public:
    std::string d_date;
    double amount;
    Date date;
    int all;

    // Here we initialize a date. Each instance of Transaction has a member date and a member "all".
    void initializeDate()
    {
        date = Date(std::stoi(d_date.substr(8, 2)),
                    std::stoi(d_date.substr(5, 2)),
                    std::stoi(d_date.substr(0, 4)));
        all = std::stoi(
                (d_date.substr(0, 4)) + (d_date.substr(5, 2)) + d_date.substr(8, 2));
    }

};

// The main class of this program is the Account class. It contains transactions of the user (vector tokens).
// Also, it contains lastDate and lastAmount members, which show last spending information.
// The main class contains methods for adding, processing transaction.
class Account
{
public:
    int N, D;
    int trigger;

    Date lastDate;
    double lastAmount;

    int minDate = INT32_MAX;
    int maxDate = 0;

    bool first;
    double m_median;

    std::vector<Transaction> tokens;
    std::queue<double> q;

    void initializeAccount();
    void addTransaction(Transaction t);
    void processTransaction(Transaction t);
    void addAmountToQueue(double amount);
    double median();
};

// This method reads from the standard input, adds transactions to the vector "tokens" and updates
// minDate and maxDate variables (they will be used for CountingSort further)
void Account::initializeAccount()
{
    std::string line;
    std::getline(std::cin, line);
    trigger = 0;
    m_median = 0.0;
    first = true;

    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] == ' ')
        {
            N = std::atoi(line.substr(0, i).c_str());
            D = std::atoi(line.substr(i + 1, line.size() - i - 1).c_str());
        }
    }

    for (int i = 0; i < N; i++)
    {
        std::getline(std::cin, line);
        for (int i = 0; i < line.size(); ++i)
        {
            if (line[i] == ' ')
            {
                // Transactions are push backed to the vector "tokens". All info about a transaction
                // is changed before adding to the vector.
                Transaction *tr = new Transaction();
                tr->d_date = line.substr(0, i);
                tr->amount = std::atof(line.substr(i + 2).c_str());
                tr->initializeDate();
                tokens.push_back(*tr);

                minDate = min(minDate, tr->all);
                maxDate = max(maxDate, tr->all);
                delete tr;
            }
        }
    }
}

// This stable sorting algorithm is used to sort queue's transactions (data corresponding to the trailing days).
// This is a normal implementation of Merge sort, that is comparison-based.
void merge(std::vector<double> &arr, int p, int q, int r)
{
    int n1 = q - p + 1;
    int n2 = r - q;

    double L[n1], M[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[p + i];
    for (int j = 0; j < n2; j++)
        M[j] = arr[q + 1 + j];

    int i, j, k;
    i = 0;
    j = 0;
    k = p;
    while (i < n1 && j < n2)
    {
        if (L[i] <= M[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = M[j];
            j++;
        }
        k++;
    }
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = M[j];
        j++;
        k++;
    }
}

void mergeSort(std::vector<double> &arr, int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// This linear based sorting algorithm is used to sort the input data by date.
void countingSort(std::vector<Transaction> &arr, int max_date, int min_date)
{
    int MAX = max_date - min_date + 1;
    std::vector<std::vector<Transaction>> buckets(MAX, std::vector<Transaction>());
    for (int i = 0; i < arr.size(); i++)
    {
        Transaction t = arr[i];
        buckets[arr[i].all - min_date].push_back(t);
    }

    std::vector<Transaction> sorted;
    for (int i = 0; i < MAX; i++)
    {
        std::vector<Transaction> bucket = buckets[i];
        for (auto &t : bucket)
            sorted.push_back(t);

    }
    arr = sorted;
}

// This method is used to count the median of the given queue.
// Firstly, we copy all elements from queue to vector. Then we use mergeSort to sort the vector's
// transactions by the amount spent. Finally, we count the median according the task's definition.
double Account::median()
{
    auto cp = q;
    int x = cp.size();
    std::vector<double> v;
    for (int i = 0; i < x; ++i)
    {
        v.push_back(cp.front());
        cp.pop();
    }
    mergeSort(v, 0, v.size() - 1);

    if (v.size() % 2 == 0)
        return ((v[x / 2] + v[x / 2 - 1]) / 2.0);
    else
        return v[x / 2];
}

// This method is used to add transaction to the Account instance.
void Account::addTransaction(Transaction t)
{
    // If this is the first transaction added, we simply update lastDate variable.
    if (first)
    {
        first = false;
        lastDate = t.date;
    }
    // Count the difference between the lastDate and the  date of the current transaction.
    long difference = getDifference(lastDate, t.date);

    // If the dates are the same, then this is the same date.
    bool sameDay = difference == 0;

    // If the dates are the same, we simply add the transaction's amount to the lastAmount variable.
    if (sameDay)
        lastAmount += t.amount;
    // If the dates are different, we add lastAmount to the queue. If the gap between two days is more than 1
    // we have to add zeros to queue as well. Then we update lastAmount and lastDate variables and count the median.
    else
    {
        addAmountToQueue(lastAmount);
        for (int i = 0; i < difference - 1; ++i)
            addAmountToQueue(0.0);

        lastAmount = t.amount;
        lastDate = t.date;
        m_median = median();
    }
}

// This method is called only from the main function. It adds the transaction using aforementioned
// function and checks whether the lastAmount is two times more than the median of the queue.
void Account::processTransaction(Transaction t)
{
    addTransaction(t);
    if ((q.size() == D) && (lastAmount >= 2.0 * m_median))
        trigger++;

}

// This method is used to add the transaction's amount to the queue. If the queue is full, we simply
// remove the first added element and add the current element. (FIFO ADT)
void Account::addAmountToQueue(double amount)
{
    if (q.size() == D)
        q.pop();
    q.push(amount);
}

// We create an instance of Account, initialize it from standard library, use all aforementioned methods
// and get the result in "trigger" variable (the amount of triggers).
int main()
{
    Account acc;
    acc.initializeAccount();
    countingSort(acc.tokens, acc.maxDate, acc.minDate);
    for (int i = 0; i < acc.N; i++)
        acc.processTransaction(acc.tokens[i]);
    std::cout << acc.trigger << std::endl;
}
