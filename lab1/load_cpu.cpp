#include <iostream>
#include <chrono>
#include <cmath>

bool checkPrime(int value) {
    if (value < 2) return false;
    int limit = (int)sqrt(value);
    for (int divisor = 2; divisor <= limit; divisor++) {
        if (value % divisor == 0) {
            return false;
        }
    }
    return true;
}

int main()
{
    const long long count = 100000000; // 10 mlrd
    long long cnt = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < count; ++i)
    {
        checkPrime(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
    return 0;
}