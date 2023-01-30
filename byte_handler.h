#pragma once

#include <string>
#include <vector>
#include <bitset>
using Byte9 = std::bitset<9>;
using Byte = std::bitset<8>;
namespace byte_handler {
inline std::string GetFilename(const std::string& path) {
    size_t div_symbol = path.rfind('/');
    size_t index = 0;
    if (div_symbol != std::string::npos) {
        index = div_symbol + 1;
    }
    return path.substr(index);
}

inline size_t GetNumberByByte9code(Byte9& byte, size_t start = 0) {
    size_t result = 0;
    for (size_t i = start; i < 9; ++i) {
        result = result * 2 + static_cast<size_t>(byte[i]);
    }
    return result;
}

inline Byte9 GetByte9codeByNumber(size_t number) {
    Byte9 result;
    for (size_t i = 0; i < 9; ++i) {
        if ((number >> i) & 1) {
            result[8 - i] = true;
        }
    }
    return result;
}
inline std::vector<Byte9> GetByte9codeByString(std::string& s) {
    std::vector<Byte9> result;
    for (uint8_t c : s) {
        result.push_back(GetByte9codeByNumber(static_cast<size_t>(c)));
    }
    return result;
}
}  // namespace byte_handler