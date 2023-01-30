#pragma once

#include "bitwise_output.h"
#include "bitwise_input.h"
#include "byte_handler.h"
#include <vector>
#include <string>
#include "trie.h"

enum class ArchiverUnits { archive, unarchive, help };
class Archiver {
protected:
    static void EncodeFile(const std::string &path, BitwiseOutput &out, bool last);
    static void DecodeFile(BitwiseInput &in, bool &arc_end);
    static std::unordered_map<Byte9, std::vector<bool>> Huffman(const std::vector<std::pair<size_t, Byte9>> &bytes);
    static void CountCharacters(const std::string &s, std::unordered_map<Byte9, size_t> &cnt);
    static void CountCharacters(BitwiseInput &in, std::unordered_map<Byte9, size_t> &cnt);
    static void IncludeServiceSymbols(std::unordered_map<Byte9, size_t> &cnt);
    static std::vector<std::pair<size_t, Byte9>> SortByLengths(std::unordered_map<Byte9, size_t> &bytes);
    static void WriteEncoded(std::unordered_map<Byte9, std::vector<bool>> &codes,
                             std::vector<std::pair<size_t, Byte9>> &ordered, std::string &filename, BitwiseInput &in,
                             BitwiseOutput &out, bool last);
    static void DecodeOrdering(size_t characters_cnt, BitwiseInput &in, std::vector<std::pair<size_t, Byte9>> &ordered);
    static std::string GetFilename(BitwiseInput &in, SharedNodePtr trie);
    static void WriteDecoded(BitwiseInput &in, BitwiseOutput &out, bool &arc_end, SharedNodePtr trie);

public:
    using Options = std::vector<std::string>;
    static void HowToUse();
    static void Encode(Options o);
    static void Decode(Options o);
};