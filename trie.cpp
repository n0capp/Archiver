#include <queue>
#include <cassert>
#include <iostream>
#include "trie.h"
#include "binary_heap.h"

Node::Node() : cnt_(0), byte_(0) {
}

Node::Node(size_t byte, size_t cnt) {
    byte_ = byte;
    cnt_ = cnt;
}

const bool Node::IsTerminal() const {
    return !l_ && !r_;
}

Node::Node(SharedNodePtr l, SharedNodePtr r) {
    l_ = l;
    r_ = r;
    cnt_ = l_->cnt_ + r_->cnt_;
    byte_ = std::min(l_->byte_, r_->byte_);
}

bool Node::operator>(Node &node) const {
    return std::tie(cnt_, byte_) > std::tie(node.cnt_, node.byte_);
}

SharedNodePtr Node::Initialize(const std::unordered_map<Byte9, size_t> &cnt) {
    BinaryHeap<SharedNodePtr, NodeComparator, std::vector<SharedNodePtr>> p;
    for (auto &[b, c] : cnt) {
        auto b_copy = b;
        p.push(std::make_shared<Node>(Node(byte_handler::GetNumberByByte9code(b_copy), c)));
    }
    while (p.size() >= 2) {
        SharedNodePtr left = p.top();
        p.pop();
        SharedNodePtr right = p.top();
        p.pop();
        p.push(std::make_shared<Node>(left, right));
    }
    return p.top();
}

std::unordered_map<Byte9, size_t> Node::GetLengths() {
    std::queue<std::pair<SharedNodePtr, size_t>> bfs;
    std::unordered_map<Byte9, size_t> result;
    bfs.push({std::make_shared<Node>(*this), 0});
    while (!bfs.empty()) {
        auto [node, h] = bfs.front();
        bfs.pop();
        if (!node) {
            continue;
        }
        if (node->IsTerminal()) {
            result[byte_handler::GetByte9codeByNumber(node->byte_)] = h;
        } else {
            bfs.push({node->l_, h + 1});
            bfs.push({node->r_, h + 1});
        }
    }
    return result;
}

void Insert(SharedNodePtr trie, std::vector<bool> &x, Byte9 &c, size_t begin) {
    if (begin == x.size()) {
        trie->data_ = c;
        return;
    }
    if (!x[begin]) {
        if (!trie->l_) {
            trie->l_ = std::make_shared<Node>(Node());
        }
        Insert(trie->l_, x, c, begin + 1);
    } else {
        if (!trie->r_) {
            trie->r_ = std::make_shared<Node>(Node());
        }
        Insert(trie->r_, x, c, begin + 1);
    }
}
bool NodeComparator::operator()(SharedNodePtr left, SharedNodePtr right) {
    return *left > *right;
}
