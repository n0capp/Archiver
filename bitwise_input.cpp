#include "bitwise_input.h"
#include "byte_handler.h"
BitwiseInput::BitwiseInput(std::istream &ifs) : ifs_(ifs) {
    eof_ = false;
    buf_ = 0;
    bit_cnt_ = 0;
}
bool BitwiseInput::ReadBit() {
    if (bit_cnt_ == 0) {
        buf_ = 0;
        bit_cnt_ = 8;
        ifs_.get(reinterpret_cast<char &>(buf_));
        if (ifs_.eof()) {
            buf_ = 0;
            bit_cnt_ = 8;
            eof_ = true;
            return false;
        }
    }
    --bit_cnt_;
    return 1 & (buf_ >> bit_cnt_);
}
Byte9 BitwiseInput::ReadByte9(size_t start) {
    Byte9 result;
    for (size_t i = start; i < 9; ++i) {
        if (bit_cnt_ == 0) {
            buf_ = 0;
            bit_cnt_ = 8;
            ifs_.get(reinterpret_cast<char &>(buf_));
            if (ifs_.eof()) {
                buf_ = 0;
                bit_cnt_ = 8;
                eof_ = true;
                break;
            }
        }
        result[i] = (1 & (buf_ >> (bit_cnt_ - 1)));
        --bit_cnt_;
    }
    return result;
}
const bool BitwiseInput::Eof() const {
    return eof_;
}

void BitwiseInput::Refresh() {
    ifs_.clear();
    ifs_.seekg(0);
    buf_ = 0;
    bit_cnt_ = 0;
    eof_ = false;
}