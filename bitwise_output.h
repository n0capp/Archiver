#pragma once

#include "bitwise_stream.h"
#include "byte_handler.h"

class BitwiseOutput : public BitwiseStream {
protected:
    std::ostream& ofs_;

public:
    explicit BitwiseOutput(std::ostream& ofs);
    void PrintBitwise(std::vector<bool>& list);
    void PrintBitwise(Byte9 b, size_t start = 0);
    void PrintNumber(size_t x, size_t cnt_bits = 9);
    void Refresh() override;
};