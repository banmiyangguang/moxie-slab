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
    std::cout << "insert_time:" << std::chrono::duration_cast<std::chrono::microseconds>(middle - start).count() 
                << " find_time:" << std::chrono::duration_cast<std::chrono::microseconds>(end - middle).count() << std::endl;


    return 0;
}

int main(int argc, char **argv) {
    std::string value = "v";
    std::vector<size_t> items = {100000, 300000,  500000, 700000, 900000, 1100000};
    std::vector<size_t> valuelen = {128, 256, 512, 768, 1024, 1536, 1792};
    for (size_t i = 0; i < valuelen.size(); ++i) {
        std::cout << valuelen[i] << std::endl;
        value.resize(valuelen[i], 'v');
        for (size_t j = 0; j < items.size(); ++j) {
            std::cout << "items_size=" << items[j] << " "; 
            test_unordered_map(items[j], value);
        }
        std::cout << std::endl;
    }
    return 0;
}
