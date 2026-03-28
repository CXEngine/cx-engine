#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>

#include <new>
#include <utility>
#include <iterator>

namespace cx {

/// @brief A fixed-capacity buffer that stores elements inline.
///
/// Buf provides a stack-allocated (or inline-allocated) container with a fixed
/// maximum size. It manages the lifecycle of its elements, ensuring they are
/// properly constructed and destroyed.
/// Used internally in CX Engine to avoid some dynamic memory allocations.
///
/// @tparam T Type of elements stored in the buffer
/// @tparam N Maximum number of elements the buffer can hold
template <typename T, usize N>
class Buf {
    alignas(T) byte storage[sizeof(T) * N]; //< @brief Internal storage for elements
    usize elemCount = 0;                    //< @brief Current number of elements in the buffer

    /// @brief Gets a pointer to the element at the specified index
    /// @param i Index of the element
    /// @return Pointer to the element
    T* ptr(usize i) {
        return std::launder(reinterpret_cast<T*>(&storage[i * sizeof(T)]));
    }

    /// @brief Gets a constant pointer to the element at the specified index
    /// @param i Index of the element
    /// @return Constant pointer to the element
    const T* ptr(usize i) const {
        return std::launder(reinterpret_cast<const T*>(&storage[i * sizeof(T)]));
    }

public:
    /// @brief Creates an empty buffer
    Buf() : elemCount(0) {}

    /// @brief Creates a buffer from an initializer list
    /// @param initList List of elements to initialize the buffer with
    /// @throws OverflowException if the initializer list size exceeds N
    Buf(InitList<T> initList) : elemCount(0) {
        if (initList.size() > N) {
            throw OverflowException("init list too large for Buf");
        }
        for (const T& item : initList) {
            emplace(item);
        }
    }

    /// @brief Destroys the buffer and all its elements.
    ~Buf() noexcept { clear(); }

    /// @brief Copy constructor
    /// @param other Buffer to copy from
    Buf(const Buf& other) : elemCount(other.elemCount) {
        for (usize i = 0; i < elemCount; ++i)
            new (ptr(i)) T(other[i]);
    }

    /// @brief Copy assignment operator
    /// @param other Buffer to copy from
    /// @return Reference to this buffer
    Buf& operator=(const Buf& other) {
        if (this == &other)
            return *this;

        clear();
        elemCount = other.elemCount;
        for (usize i = 0; i < elemCount; ++i)
            new (ptr(i)) T(other[i]);
        return *this;
    }

    /// @brief Move constructor
    /// @param other Buffer to move from
    Buf(Buf&& other) noexcept : elemCount(other.elemCount) {
        for (usize i = 0; i < elemCount; ++i)
            new (ptr(i)) T(std::move(other[i]));
        other.elemCount = 0;
    }

    /// @brief Move assignment operator
    /// @param other Buffer to move from
    /// @return Reference to this buffer
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

    /// @brief Constructs an element in-place at the end of the buffer
    /// @tparam Args Argument types for the element constructor
    /// @param args Arguments passed to the element constructor
    /// @return Reference to the newly created element
    /// @throws OverflowException if the buffer is full
    template <typename... Args>
    T& emplace(Args&&... args) {
        if (elemCount >= N)
            throw OverflowException("Buf overflow");
        T* p = new (ptr(elemCount)) T(std::forward<Args>(args)...);
        ++elemCount;
        return *p;
    }

    /// @brief Adds a copy of the element to the end of the buffer
    /// @param v Element to add
    /// @return Reference to the added element
    T& push(const T& v) { return emplace(v); }

    /// @brief Adds a moved element to the end of the buffer
    /// @param v Element to add
    /// @return Reference to the added element
    T& push(T&& v) { return emplace(std::move(v)); }

    /// @brief Checks if the buffer has reached its maximum capacity
    /// @return True if full, false otherwise
    bool isFull() const noexcept {
        return elemCount == N;
    }

    /// @brief Checks if the buffer contains no elements
    /// @return True if empty, false otherwise
    bool isEmpty() const noexcept {
        return elemCount == 0;
    }

    /// @brief Removes all elements from the buffer, calling their destructors.
    void clear() noexcept {
        for (usize i = 0; i < elemCount; ++i)
            ptr(i)->~T();
        elemCount = 0;
    }

    /// @brief Removes the last element from the buffer without returning it.
    void removeLastElement() noexcept {
        elemCount--;
        ptr(elemCount)->~T();
    }

    /// @brief Removes and returns the last element from the buffer
    /// @return The removed element
    /// @throws std::out_of_range if the buffer is empty
    T pop() {
        if (isEmpty()) {
            throw EmptyCollectionException("pop() called on empty Buf");
        }
        --elemCount;
        T value = std::move(*ptr(elemCount));
        ptr(elemCount)->~T();
        return value;
    }

    /// @brief Accesses the element at the specified index
    /// @param i Index of the element
    /// @return Reference to the element
    T& operator[](usize i) { return *ptr(i); }

    /// @brief Accesses the element at the specified index (const)
    /// @param i Index of the element
    /// @return Constant reference to the element
    const T& operator[](usize i) const { return *ptr(i); }

    /// @brief Gets the number of elements currently in the buffer
    /// @return Number of elements
    usize getElementCount() const { return elemCount; }

    /// @brief Bidirectional iterator for Buf.
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

    private:
        /// @brief Pointer to the current element.
        T* currentPtr;

    public:
        /// @brief Creates an iterator pointing to the given address
        /// @param p Address to point to
        Iterator(T* p) : currentPtr(p) {}

        /// @brief Dereferences the iterator
        /// @return Reference to the element
        reference operator*() const { return *currentPtr; }

        /// @brief Accesses the element through the iterator
        /// @return Pointer to the element
        pointer operator->() const { return currentPtr; }

        /// @brief Pre-increment operator
        /// @return Reference to this iterator
        Iterator& operator++() {
            ++currentPtr;
            return *this;
        }

        /// @brief Post-increment operator
        /// @return Iterator pointing to the previous position
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        /// @brief Pre-decrement operator
        /// @return Reference to this iterator
        Iterator& operator--() {
            --currentPtr;
            return *this;
        }

        /// @brief Post-decrement operator
        /// @return Iterator pointing to the previous position
        Iterator operator--(int) {
            Iterator temp = *this;
            --(*this);
            return temp;
        }

        /// @brief Compares two iterators for equality
        /// @param other Iterator to compare with
        /// @return True if equal, false otherwise
        bool operator==(const Iterator& other) const { return currentPtr == other.currentPtr; }

        /// @brief Compares two iterators for inequality
        /// @param other Iterator to compare with
        /// @return True if not equal, false otherwise
        bool operator!=(const Iterator& other) const { return currentPtr != other.currentPtr; }
    };

    /// @brief Bidirectional constant iterator for Buf.
    class ConstIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

    private:
        /// @brief Pointer to the current element.
        const T* currentPtr;

    public:
        /// @brief Creates a constant iterator pointing to the given address
        /// @param p Address to point to
        ConstIterator(const T* p) : currentPtr(p) {}

        /// @brief Conversion constructor from Iterator to ConstIterator
        /// @param other Iterator to convert from
        ConstIterator(const Iterator& other) : currentPtr(other.operator->()) {}

        /// @brief Dereferences the iterator
        /// @return Constant reference to the element
        reference operator*() const { return *currentPtr; }

        /// @brief Accesses the element through the iterator
        /// @return Constant pointer to the element
        pointer operator->() const { return currentPtr; }

        /// @brief Pre-increment operator
        /// @return Reference to this iterator
        ConstIterator& operator++() {
            ++currentPtr;
            return *this;
        }

        /// @brief Post-increment operator
        /// @return Constant iterator pointing to the previous position
        ConstIterator operator++(int) {
            ConstIterator temp = *this;
            ++(*this);
            return temp;
        }

        /// @brief Pre-decrement operator
        /// @return Reference to this iterator
        ConstIterator& operator--() {
            --currentPtr;
            return *this;
        }

        /// @brief Post-decrement operator
        /// @return Constant iterator pointing to the previous position
        ConstIterator operator--(int) {
            ConstIterator temp = *this;
            --(*this);
            return temp;
        }

        /// @brief Compares two constant iterators for equality
        /// @param other Iterator to compare with
        /// @return True if equal, false otherwise
        bool operator==(const ConstIterator& other) const { return currentPtr == other.currentPtr; }

        /// @brief Compares two constant iterators for inequality
        /// @param other Iterator to compare with
        /// @return True if not equal, false otherwise
        bool operator!=(const ConstIterator& other) const { return currentPtr != other.currentPtr; }
    };

    /// @brief Returns an iterator to the beginning of the buffer
    /// @return Iterator to the first element
    Iterator begin() { return Iterator(ptr(0)); }

    /// @brief Returns an iterator to the end of the buffer
    /// @return Iterator to the element following the last element
    Iterator end() { return Iterator(ptr(elemCount)); }

    /// @brief Returns a constant iterator to the beginning of the buffer
    /// @return Constant iterator to the first element
    ConstIterator begin() const { return ConstIterator(ptr(0)); }

    /// @brief Returns a constant iterator to the end of the buffer
    /// @return Constant iterator to the element following the last element
    ConstIterator end() const { return ConstIterator(ptr(elemCount)); }

    /// @brief Returns a constant iterator to the beginning of the buffer
    /// @return Constant iterator to the first element
    ConstIterator cbegin() const { return ConstIterator(ptr(0)); }

    /// @brief Returns a constant iterator to the end of the buffer
    /// @return Constant iterator to the element following the last element
    ConstIterator cend() const { return ConstIterator(ptr(elemCount)); }

    /// @brief Returns a reverse iterator to the beginning of the buffer
    /// @return Reverse iterator to the last element
    std::reverse_iterator<Iterator> rbegin() { return std::make_reverse_iterator(end()); }

    /// @brief Returns a reverse iterator to the end of the buffer
    /// @return Reverse iterator to the element preceding the first element
    std::reverse_iterator<Iterator> rend() { return std::make_reverse_iterator(begin()); }

    /// @brief Returns a constant reverse iterator to the beginning of the buffer
    /// @return Constant reverse iterator to the last element
    std::reverse_iterator<ConstIterator> rbegin() const { return std::make_reverse_iterator(end()); }

    /// @brief Returns a constant reverse iterator to the end of the buffer
    /// @return Constant reverse iterator to the element preceding the first element
    std::reverse_iterator<ConstIterator> rend() const { return std::make_reverse_iterator(begin()); }

    /// @brief Returns a constant reverse iterator to the beginning of the buffer
    /// @return Constant reverse iterator to the last element
    std::reverse_iterator<ConstIterator> crbegin() const { return std::make_reverse_iterator(cend()); }

    /// @brief Returns a constant reverse iterator to the end of the buffer
    /// @return Constant reverse iterator to the element preceding the first element
    std::reverse_iterator<ConstIterator> crend() const { return std::make_reverse_iterator(cbegin()); }
};

} // namespace cx
