#include <iostream>
#include <string>
#include <assert.h>
#include <chrono>
#include <unordered_map>
#include <vector>

using namespace std;

int test_unordered_map(size_t item, const std::string& value) {
    std::string key_prefix;
    key_prefix.resize(30, 'a');
    std::string key = "";
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    unordered_map<string, string> maps;
    for (size_t i = 0; i < item; ++i) {
        key = key_prefix + std::to_string(i);
        maps[key] = value + key;
        assert(maps.find(key) != maps.end());
    }
    std::chrono::steady_clock::time_point middle = std::chrono::steady_clock::now();
    for (size_t i = 0; i < item; ++i) {
        key = key_prefix + std::to_string(i);
        assert(maps.find(key) != maps.end());
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "datalen:" << value.size() << " times:" << item
            << " insert_time:" << std::chrono::duration_cast<std::chrono::microseconds>(middle - start).count() 
            << " find_time:" << std::chrono::duration_cast<std::chrono::microseconds>(end - middle).count() << std::endl;


    return 0;
}
int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Usage:exec datalen reqs" << std::endl;
        return -1;
    }
    int reqs = std::atoi(argv[2]);
    int datalen = std::atoi(argv[1]);
    if (reqs < 0 || datalen < 0) {
        std::cout << "error:reqs < 0 || datalen < 0" << std::endl;
        return -1;
    }
    std::cout << datalen << std::endl;
    std::string value;
    value.resize(datalen, 'v');
    test_unordered_map(reqs, value);
    return 0;
}


