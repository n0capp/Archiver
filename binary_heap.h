#pragma once

#include <functional>

template <typename T, typename E = std::less<T>, typename K = std::vector<T>>
class BinaryHeap {
protected:
    K data_;
    inline const size_t getLeftChild(size_t i) const /*NOLINT*/ {
        return i * 2 + 1;
    }
    inline const size_t getRightChild(size_t i) const /*NOLINT*/ {
        return i * 2 + 2;
    }
    inline const bool hasLeftChild(size_t i) const /*NOLINT*/ {
        return getLeftChild(i) < data_.size();
    }
    inline const bool hasRightChild(size_t i) const /*NOLINT*/ {
        return getRightChild(i) < data_.size();
    }
    inline const bool isRoot(size_t i) const /*NOLINT*/ {
        return i == 0;
    }

    inline const size_t getParent(size_t i) const /*NOLINT*/ {
        if (isRoot(i)) {
            return NullPosition;
        }
        return (i - 1) / 2;
    }

public:
    static constexpr size_t NullPosition = 65534;
    size_t size() const /*NOLINT*/ {
        return data_.size();
    }
    const T top() const /*NOLINT*/ {
        return data_[0];
    }
    void heapifyDown(size_t i) /*NOLINT*/ {
        while (hasLeftChild(i)) {
            size_t l = getLeftChild(i);
            size_t r = getRightChild(i);
            size_t go = l;
            // Ewin(l, r) = l => take l
            if (hasRightChild(i) && !E()(data_[r], data_[l])) {
                go = r;
            }
            if (E()(data_[go], data_[i])) {
                break;
            }
            std::swap(i, go);
            std::swap(data_[i], data_[go]);
        }
    }
    void heapifyUp(size_t i) /*NOLINT*/ {
        while (!isRoot(i) && !E()(data_[i], data_[getParent(i)])) {
            std::swap(data_[i], data_[getParent(i)]);
            i = getParent(i);
        }
    }
    void push(const T &t) /*NOLINT*/ {
        data_.push_back(t);
        heapifyUp(data_.size() - 1);
    }
    void pop() /*NOLINT*/ {
        std::swap(data_.front(), data_.back());
        data_.pop_back();
        heapifyDown(0);
    }
};