#pragma once

#include <cstdint>
#include <cstddef>
#include <fstream>
#include <vector>

class BitwiseStream {
protected:
    bool eof_;
    uint8_t buf_;
    uint8_t bit_cnt_;

public:
    virtual void Refresh() = 0;
};
