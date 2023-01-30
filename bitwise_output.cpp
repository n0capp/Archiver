#include "bitwise_output.h"

BitwiseOutput::BitwiseOutput(std::ostream &ofs) : ofs_(ofs) {
    buf_ = 0;
    bit_cnt_ = 0;
}

void BitwiseOutput::Refresh() {
    if (bit_cnt_ > 0) {
        ofs_.put(static_cast<char>(buf_));
    }
    buf_ = 0;
    bit_cnt_ = 0;
    ofs_.flush();
}
void BitwiseOutput::PrintNumber(size_t x, size_t cnt_bits) {
    for (size_t i = 0; i < cnt_bits; ++i) {
        size_t bit = (x >> (cnt_bits - i - 1)) & 1;
        if (bit) {
            buf_ |= (1 << (7 - bit_cnt_));
        }
        if (++bit_cnt_ == 8) {
            ofs_.put(static_cast<char>(buf_));
            buf_ = 0;
            bit_cnt_ = 0;
        }
    }
}
void BitwiseOutput::PrintBitwise(std::vector<bool> &list) {
    for (const auto &bit : list) {
        if (bit) {
            buf_ |= (1 << (7 - bit_cnt_));
        }
        if (++bit_cnt_ == 8) {
            ofs_.put(static_cast<char>(buf_));
            buf_ = 0;
            bit_cnt_ = 0;
        }
    }
}
void BitwiseOutput::PrintBitwise(Byte9 b, size_t start) {
    for (size_t i = start; i < 9; ++i) {
        bool bit = b[i];
        if (bit) {
            buf_ |= (1 << (7 - bit_cnt_));
        }
        if (++bit_cnt_ == 8) {
            ofs_.put(static_cast<char>(buf_));
            buf_ = 0;
            bit_cnt_ = 0;
        }
    }
}