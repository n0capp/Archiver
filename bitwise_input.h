#pragma once

#include "bitwise_stream.h"
#include "byte_handler.h"

class BitwiseInput : public BitwiseStream {
protected:
    std::istream& ifs_;

public:
    const bool Eof() const;
    explicit BitwiseInput(std::istream& ifs);
    std::vector<bool> ReadBitwise(size_t cnt);
    bool ReadBit();
    Byte9 ReadByte9(size_t start = 0);
    void Refresh() override;
};
