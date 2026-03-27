#pragma once

#include <functional>
#include <utility>

template <typename T>
class HybridPtr {
    template <typename U>
    friend class HybridPtr;
protected:
    T* ptr;
    bool ownership;

public:
    HybridPtr() noexcept : ptr(nullptr), ownership(false) {}

    static HybridPtr Borrow(T* borrowed) {
        HybridPtr result;
        result.ptr = borrowed;
        result.ownership = false;
        return result;
    }

    static HybridPtr Take(T* owned) {
        HybridPtr result;
        result.ptr = owned;
        result.ownership = true;
        return result;
    }

    template <typename... Args>
    static HybridPtr Make(Args&&... args) {
        return HybridPtr::Take(new T(std::forward<Args>(args)...));
    }

    HybridPtr(T& ref) noexcept
        : ptr(&ref), ownership(false) {}

    ~HybridPtr() {
        if (hasOwnership()) {
            delete ptr;
        }
    }

    HybridPtr(const HybridPtr&) = delete;
    HybridPtr& operator=(const HybridPtr&) = delete;

    HybridPtr(std::nullptr_t)
        : ptr(nullptr)
        , ownership(false) {}

    HybridPtr(HybridPtr&& other) noexcept
        : ptr(std::exchange(other.ptr, nullptr)),
          ownership(std::exchange(other.ownership, false)) {}

    template<class U>
    HybridPtr(HybridPtr<U>&& other)
        : ptr(std::exchange(other.ptr, nullptr)),
          ownership(std::exchange(other.ownership, false)) {}

    HybridPtr& operator=(HybridPtr&& other) {
        ptr = std::exchange(other.ptr, nullptr);
        ownership = std::exchange(other.ownership, false);
        return *this;
    }

    template<class U>
    HybridPtr& operator=(HybridPtr<U>&& other) {
        ptr = std::exchange(other.ptr, nullptr);
        ownership = std::exchange(other.ownership, false);
        return *this;
    }

    HybridPtr& operator=(std::nullptr_t) {
        reset();
        return *this;
    }

    T* get() { return ptr; }
    const T* get() const { return ptr; }

    T& ref() { return *ptr; };
    const T& ref() const { return *ptr; }

    T* operator->() { return get(); }
    const T* operator->() const { return get(); }

    T& operator*() { return *ptr; };
    const T& operator*() const { return *ptr; }

    T* release() noexcept {
        ownership = false;
        return std::exchange(ptr, nullptr);
    }

    void reset() noexcept {
        if (ownership) {
            delete ptr;
        }
        ptr = nullptr;
        ownership = false;
    }

    void reset(T* newPtr, bool isOwner) noexcept {
        reset();
        ptr = newPtr;
        ownership = isOwner;
    }

    bool hasOwnership() const {
        return ownership;
    }

    bool isBorrowed() const {
        return !hasOwnership();
    }

    friend bool operator==(const HybridPtr<T>& a, const HybridPtr<T>& b) {
        return a.get() == b.get();
    }

    friend bool operator!=(const HybridPtr<T>& a, const HybridPtr<T>& b) {
        return a.get() != b.get();
    }

    friend bool operator==(const HybridPtr<T>& a, std::nullptr_t) {
        return a.get() == nullptr;
    }

    friend bool operator!=(const HybridPtr<T>& a, std::nullptr_t) {
        return a.get() != nullptr;
    }
};


namespace std {
    template <class T>
    struct hash<HybridPtr<T>> {
        size_t operator()(const HybridPtr<T>& x) const {
            return hash<T*>()(x.get());
        }
    };
}
