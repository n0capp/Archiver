#pragma once

#include "byte_handler.h"
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <memory>

struct Node;
using UniqueNodePtr = std::unique_ptr<Node>;
using SharedNodePtr = std::shared_ptr<Node>;
using WeakNodePtr = std::weak_ptr<Node>;

void Insert(SharedNodePtr trie, std::vector<bool> &x, Byte9 &c, size_t begin = 0);

struct Node {
protected:
    size_t cnt_;
    size_t byte_;

public:
    Byte9 data_;
    SharedNodePtr l_;
    SharedNodePtr r_;
    const bool IsTerminal() const;
    Node();
    Node(size_t byte, size_t cnt);
    Node(SharedNodePtr l, SharedNodePtr r);
    static SharedNodePtr Initialize(const std::unordered_map<Byte9, size_t> &cnt);
    std::unordered_map<Byte9, size_t> GetLengths();
    bool operator>(Node &node) const;
    friend void Insert(SharedNodePtr trie, std::vector<bool> &x, Byte9 &c, size_t begin);
};

struct NodeComparator {
    bool operator()(SharedNodePtr left, SharedNodePtr right);
};
