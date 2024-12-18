#include <iostream>
#include <chrono>
#include <cmath>
#include <cstdlib>

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

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: ./load_cpu <count>" << std::endl;
        return 1;
    }

    long long count = std::atoll(argv[1]);

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        checkPrime(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << count << " " << elapsed.count() << std::endl;
    return 0;
}
