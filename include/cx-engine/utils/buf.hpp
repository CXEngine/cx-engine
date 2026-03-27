#pragma once

#include <cx-engine/defs/types.hpp>

#include <new>
#include <utility>
#include <stdexcept>
#include <iterator>

namespace cx {

class BufOverflowError: public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    BufOverflowError() : BufOverflowError("overflow") {}
};

template <typename T, usize N>
class Buf {
    alignas(T) byte storage[sizeof(T) * N];
    usize elemCount = 0;

    T* ptr(usize i) {
        return std::launder(reinterpret_cast<T*>(&storage[i * sizeof(T)]));
    }

    const T* ptr(usize i) const {
        return std::launder(reinterpret_cast<const T*>(&storage[i * sizeof(T)]));
    }

public:
    Buf() : elemCount(0) {}
    Buf(InitList<T> initList) : elemCount(0) {
        if (initList.size() > N) {
            throw BufOverflowError("init list too large for Buf");
        }
        for (const T& item : initList) {
            emplace(item);
        }
    }
    ~Buf() noexcept { clear(); }

    Buf(const Buf& other) : elemCount(other.elemCount) {
        for (usize i = 0; i < elemCount; ++i)
            new (ptr(i)) T(other[i]);
    }

    Buf& operator=(const Buf& other) {
        if (this == &other)
            return *this;

        clear();
        elemCount = other.elemCount;
        for (usize i = 0; i < elemCount; ++i)
            new (ptr(i)) T(other[i]);
        return *this;
    }

    Buf(Buf&& other) noexcept : elemCount(other.elemCount) {
        for (usize i = 0; i < elemCount; ++i)
            new (ptr(i)) T(std::move(other[i]));
        other.elemCount = 0;
    }

    Buf& operator=(Buf&& other) noexcept {
        if (this == &other)
            return *this;

        clear();
        elemCount = other.elemCount;
        for (usize i = 0; i < elemCount; ++i)
            new (ptr(i)) T(std::move(other[i]));
        other.elemCount = 0;
        return *this;
    }

    template <typename... Args>
    T& emplace(Args&&... args) {
        if (elemCount >= N)
            throw BufOverflowError();
        T* p = new (ptr(elemCount)) T(std::forward<Args>(args)...);
        ++elemCount;
        return *p;
    }

    T& push(const T& v) { return emplace(v); }
    T& push(T&& v) { return emplace(std::move(v)); }

    bool isFull() const noexcept {
        return elemCount == N;
    }

    bool isEmpty() const noexcept {
        return elemCount == 0;
    }

    void clear() noexcept {
        for (usize i = 0; i < elemCount; ++i)
            ptr(i)->~T();
        elemCount = 0;
    }

    void removeLastElement() noexcept {
        elemCount--;
        ptr(elemCount)->~T();
    }

    T pop() {
        if (isEmpty()) {
            throw std::out_of_range("pop() called on empty Buf");
        }
        --elemCount;
        T value = std::move(*ptr(elemCount));
        ptr(elemCount)->~T();
        return value;
    }


    T& operator[](usize i) { return *ptr(i); }
    const T& operator[](usize i) const { return *ptr(i); }

    usize getElementCount() const { return elemCount; }

    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

    private:
        T* currentPtr;

    public:
        Iterator(T* p) : currentPtr(p) {}

        reference operator*() const { return *currentPtr; }
        pointer operator->() const { return currentPtr; }

        Iterator& operator++() {
            ++currentPtr;
            return *this;
        }
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        Iterator& operator--() {
            --currentPtr;
            return *this;
        }
        Iterator operator--(int) {
            Iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const { return currentPtr == other.currentPtr; }
        bool operator!=(const Iterator& other) const { return currentPtr != other.currentPtr; }
    };

    class ConstIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

    private:
        const T* currentPtr;

    public:
        ConstIterator(const T* p) : currentPtr(p) {}
        ConstIterator(const Iterator& other) : currentPtr(other.operator->()) {}

        reference operator*() const { return *currentPtr; }
        pointer operator->() const { return currentPtr; }

        ConstIterator& operator++() {
            ++currentPtr;
            return *this;
        }
        ConstIterator operator++(int) {
            ConstIterator temp = *this;
            ++(*this);
            return temp;
        }

        ConstIterator& operator--() {
            --currentPtr;
            return *this;
        }
        ConstIterator operator--(int) {
            ConstIterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const ConstIterator& other) const { return currentPtr == other.currentPtr; }
        bool operator!=(const ConstIterator& other) const { return currentPtr != other.currentPtr; }
    };

    Iterator begin() { return Iterator(ptr(0)); }
    Iterator end() { return Iterator(ptr(elemCount)); }

    ConstIterator begin() const { return ConstIterator(ptr(0)); }
    ConstIterator end() const { return ConstIterator(ptr(elemCount)); }

    ConstIterator cbegin() const { return ConstIterator(ptr(0)); }
    ConstIterator cend() const { return ConstIterator(ptr(elemCount)); }

    std::reverse_iterator<Iterator> rbegin() { return std::make_reverse_iterator(end()); }
    std::reverse_iterator<Iterator> rend() { return std::make_reverse_iterator(begin()); }

    std::reverse_iterator<ConstIterator> rbegin() const { return std::make_reverse_iterator(end()); }
    std::reverse_iterator<ConstIterator> rend() const { return std::make_reverse_iterator(begin()); }

    std::reverse_iterator<ConstIterator> crbegin() const { return std::make_reverse_iterator(cend()); }
    std::reverse_iterator<ConstIterator> crend() const { return std::make_reverse_iterator(cbegin()); }
};

} // namespace cx
