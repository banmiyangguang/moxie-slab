#include <iostream>
#include <string.h>
#include <chrono>

using namespace std;

int main () {
    char buf[101024];
    memset(buf, 'V', 101024);
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    for (int i = 0; i < 60000; ++i) {
        char buf1[101024];
        //memset(buf1, 'V', 101024);
        memcpy(buf1, buf, 101024);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "copy time:" << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    return 0;
}
