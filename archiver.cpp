#include <unordered_map>
#include <map>
#include <cassert>
#include "archiver.h"
#include "parser.h"
#include "trie.h"

namespace constant {
const int EXIT_CODE = 111;
const size_t FILENAME_END = 256;
const size_t ONE_MORE_FILE = 257;
const size_t ARCHIVE_END = 258;
const Byte9 BYTE9_FILENAME_END = byte_handler::GetByte9codeByNumber(FILENAME_END);
const Byte9 BYTE9_ONE_MORE_FILE = byte_handler::GetByte9codeByNumber(ONE_MORE_FILE);
const Byte9 BYTE9_ARCHIVE_END = byte_handler::GetByte9codeByNumber(ARCHIVE_END);
}  // namespace constant

void Archiver::HowToUse() {
    std::cout << "Using archiver \n"
              << "Possible commands: \n"
              << "-c archive_name file1 [file2 ...] - compressing list of files and saving result to archive_name\n"
              << "-d archive_name - unarchive files from archive_name and move to current dir\n"
              << "author : Karim Minnibaev @n0capp \n";
}

void Archiver::CountCharacters(const std::string& s, std::unordered_map<Byte9, size_t>& cnt) {
    for (uint8_t c : s) {
        auto byte = byte_handler::GetByte9codeByNumber(static_cast<size_t>(c));
        ++cnt[byte];
    }
}
void Archiver::CountCharacters(BitwiseInput& in, std::unordered_map<Byte9, size_t>& cnt) {
    while (!in.Eof()) {
        Byte9 byte = in.ReadByte9(1);
        if (in.Eof()) {
            break;
        }
        ++cnt[byte];
    }
}
void Archiver::IncludeServiceSymbols(std::unordered_map<Byte9, size_t>& cnt) {
    ++cnt[constant::BYTE9_FILENAME_END];
    ++cnt[constant::BYTE9_ONE_MORE_FILE];
    ++cnt[constant::BYTE9_ARCHIVE_END];
}
bool Cmp(const std::pair<size_t, Byte9>& a, const std::pair<size_t, Byte9>& b) {
    if (a.first != b.first) {
        return a.first < b.first;
    }
    for (size_t i = 0; i < 9; ++i) {
        if (a.second[i] != b.second[i]) {
            return a.second[i] < b.second[i];
        }
    }
    return false;
}
std::vector<std::pair<size_t, Byte9>> Archiver::SortByLengths(std::unordered_map<Byte9, size_t>& bytes) {
    std::vector<std::pair<size_t, Byte9>> ord;
    for (const auto& [s, l] : bytes) {
        ord.push_back({l, s});
    }
    std::sort(ord.begin(), ord.end(), Cmp);
    return ord;
}
void Archiver::WriteEncoded(std::unordered_map<Byte9, std::vector<bool>>& codes,
                            std::vector<std::pair<size_t, Byte9>>& ordered, std::string& filename, BitwiseInput& in,
                            BitwiseOutput& out, bool last) {

    out.PrintNumber(codes.size());
    for (auto& element : ordered) {
        out.PrintBitwise(element.second);
    }
    for (size_t length = 1, i = 0; i < ordered.size(); ++length) {
        size_t result = 0;
        while (i < ordered.size() && ordered[i].first == length) {
            ++i;
            ++result;
        }
        out.PrintNumber(result);
    }

    auto fn_bytes = byte_handler::GetByte9codeByString(filename);
    for (auto& byte : fn_bytes) {
        out.PrintBitwise(codes[byte]);
    }
    out.PrintBitwise(codes[constant::BYTE9_FILENAME_END]);

    in.Refresh();
    while (!in.Eof()) {
        Byte9 byte = in.ReadByte9(1);
        if (in.Eof()) {
            break;
        }
        out.PrintBitwise(codes[byte]);
    }
    out.PrintBitwise(last ? codes[constant::BYTE9_ARCHIVE_END] : codes[constant::BYTE9_ONE_MORE_FILE]);
}
void Archiver::EncodeFile(const std::string& path, BitwiseOutput& out, bool last) {
    std::string filename = byte_handler::GetFilename(path);
    std::ifstream stream(path);
    // mb bad or fail
    if (!stream.is_open()) {
        throw FileReadingException();
    }

    BitwiseInput in(stream);
    std::unordered_map<Byte9, size_t> cnt;

    CountCharacters(filename, cnt);
    CountCharacters(in, cnt);

    IncludeServiceSymbols(cnt);

    SharedNodePtr trie = Node::Initialize(cnt);
    std::unordered_map<Byte9, size_t> coded = trie->GetLengths();

    auto ordered = SortByLengths(coded);

    auto codes = Huffman(ordered);

    WriteEncoded(codes, ordered, filename, in, out, last);
    stream.close();
    // write encoded is hard function
}

void Archiver::Encode(Archiver::Options o) {
    int64_t s_time = clock();
    std::ofstream stream(o[0]);
    // mb bad or fail
    if (!stream.is_open()) {
        throw FileReadingException();
    }
    BitwiseOutput out(stream);
    for (size_t i = 1; i < o.size(); ++i) {
        EncodeFile(o[i], out, i + 1 == o.size());
    }
    out.Refresh();
    int64_t f_time = clock();
    double secs = static_cast<double>(f_time - s_time) / CLOCKS_PER_SEC;
    std::cout << "Successfully encoded in " << secs << " seconds!" << std::endl;
}
void Archiver::DecodeOrdering(size_t characters_cnt, BitwiseInput& in, std::vector<std::pair<size_t, Byte9>>& ordered) {
    size_t index = 0;
    size_t length = 0;
    size_t cnt = 0;
    std::vector<Byte9> bytes(characters_cnt);
    for (size_t i = 0; i < characters_cnt; ++i) {
        bytes[i] = in.ReadByte9();
    }

    while (index < characters_cnt) {
        while (cnt == 0) {
            Byte9 read = in.ReadByte9();
            cnt = byte_handler::GetNumberByByte9code(read);
            ++length;
        }
        ordered.emplace_back(length, bytes[index++]);
        --cnt;
    }
}

std::string Archiver::GetFilename(BitwiseInput& in, SharedNodePtr trie) {
    std::string filename;
    while (true) {
        SharedNodePtr node = trie;
        while (!node->IsTerminal()) {
            bool bit = in.ReadBit();
            if (!bit) {
                node = node->l_;
            } else {
                node = node->r_;
            }
        }
        if (node->data_ != constant::BYTE9_FILENAME_END) {
            filename.push_back(static_cast<char>(byte_handler::GetNumberByByte9code(node->data_)));
        } else {
            break;
        }
    }
    return filename;
}
void Archiver::WriteDecoded(BitwiseInput& in, BitwiseOutput& out, bool& arc_end, SharedNodePtr trie) {

    while (true) {
        SharedNodePtr node = trie;
        while (!node->IsTerminal()) {
            bool bit = in.ReadBit();
            if (!bit) {
                node = node->l_;
            } else {
                node = node->r_;
            }
        }
        auto data = node->data_;
        if (data != constant::BYTE9_ONE_MORE_FILE && data != constant::BYTE9_ARCHIVE_END) {
            out.PrintBitwise(data, 1);
        } else {
            out.Refresh();
            if (data == constant::BYTE9_ARCHIVE_END) {
                arc_end = true;
            }
            break;
        }
    }
}
void Archiver::DecodeFile(BitwiseInput& in, bool& arc_end) {
    Byte9 read = in.ReadByte9();
    size_t characters_cnt = byte_handler::GetNumberByByte9code(read);

    std::vector<std::pair<size_t, Byte9>> ordered;
    DecodeOrdering(characters_cnt, in, ordered);

    auto forward = Huffman(ordered);
    std::unordered_map<std::vector<bool>, Byte9> back;
    SharedNodePtr trie = std::make_shared<Node>(Node());
    for (auto& [s, c] : forward) {
        auto s_cpy = s;
        Insert(trie, c, s_cpy);
    }
    auto filename = GetFilename(in, trie);
    std::ofstream stream(filename);
    if (!stream.is_open()) {
        throw FileReadingException();
    }
    BitwiseOutput out(stream);
    WriteDecoded(in, out, arc_end, trie);
}

void Archiver::Decode(Archiver::Options o) {
    int64_t s_time = clock();
    std::ifstream stream(o[0]);
    // mb bad or fail
    if (!stream.is_open()) {
        throw FileReadingException();
    }
    BitwiseInput in(stream);
    bool arc_end = false;
    while (!arc_end) {
        DecodeFile(in, arc_end);
    }
    int64_t f_time = clock();
    double secs = static_cast<double>(f_time - s_time) / CLOCKS_PER_SEC;
    std::cout << "Successfully decoded in " << secs << " seconds!" << std::endl;
}
std::unordered_map<Byte9, std::vector<bool>> Archiver::Huffman(const std::vector<std::pair<size_t, Byte9>>& bytes) {
    // reordering to build canonical huffman code
    size_t length = bytes[0].first;
    std::vector<bool> code(length, false);
    std::unordered_map<Byte9, std::vector<bool>> result;
    for (size_t i = 0; i < bytes.size(); ++i) {
        result[bytes[i].second] = code;
        // we have string 00000011111, so we need to find last zero, change it to 1 and change put zeroes after
        size_t index = code.size();
        while (index > 0 && code[index - 1]) {
            code[index - 1] = false;
            --index;
        }
            if (index != 0) {
            --index;
        }
        code[index] = true;
        if (i != bytes.size() - 1 && code.size() < bytes[i + 1].first) {
            code.resize(bytes[i + 1].first);
        }
    }
    return result;
}
int SafeMain(int argc, char** argv) {
    Parser parser(argc, argv);
    ParsedData parsed = parser.Parse();
    if (!parser.ParseSuccess()) {
        return constant::EXIT_CODE;
    }
    Archiver archiver;
    if (parsed.first == ArchiverUnits::help) {
        archiver.HowToUse();
    } else if (parsed.first == ArchiverUnits::archive) {
        archiver.Encode(parsed.second);
    } else {
        archiver.Decode(parsed.second);
    }
    return 0;
}
int main(int argc, char** argv) {
    try {
        return SafeMain(argc, argv);
    } catch (std::exception& e) {
        std::cout << e.what();
        return constant::EXIT_CODE;
    }
}
