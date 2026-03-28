#pragma once

#include <functional>
#include <utility>

namespace cx {

/// @brief A smart pointer that can either borrow a raw pointer or take ownership of it
///
/// HybridPtr provides a flexible way to manage memory where the ownership state can be 
/// determined at runtime. It is useful for APIs that might either return a reference 
/// to an existing object or a newly allocated one.
template <typename T>
class HybridPtr {
    template <typename U>
    friend class HybridPtr;
protected:
    /// @brief The underlying raw pointer
    T* ptr;
    /// @brief Flag indicating if this HybridPtr owns the pointed-to object
    bool ownership;

public:
    /// @brief Creates an empty HybridPtr (borrowed nullptr)
    HybridPtr() noexcept : ptr(nullptr), ownership(false) {}

    /// @brief Creates a HybridPtr that borrows the given pointer
    /// @param borrowed The raw pointer to borrow
    /// @return A new HybridPtr instance in borrowed state
    /// @note The caller must ensure the pointed-to object outlives this HybridPtr
    static HybridPtr Borrow(T* borrowed) {
        HybridPtr result;
        result.ptr = borrowed;
        result.ownership = false;
        return result;
    }

    /// @brief Creates a HybridPtr that takes ownership of the given pointer
    /// @param owned The raw pointer to take ownership of
    /// @return A new HybridPtr instance in owned state
    static HybridPtr Take(T* owned) {
        HybridPtr result;
        result.ptr = owned;
        result.ownership = true;
        return result;
    }

    /// @brief Creates a HybridPtr that takes ownership of a newly constructed object
    /// @tparam Args Argument types for the T constructor
    /// @param args Arguments passed to the T constructor
    /// @return A new HybridPtr instance in owned state
    template <typename... Args>
    static HybridPtr Make(Args&&... args) {
        return HybridPtr::Take(new T(std::forward<Args>(args)...));
    }

    /// @brief Creates a HybridPtr that borrows from a reference
    /// @param ref The object to borrow
    HybridPtr(T& ref) noexcept
        : ptr(&ref), ownership(false) {}

    /// @brief Destructor that deletes the pointer if ownership is held
    ~HybridPtr() {
        if (hasOwnership()) {
            delete ptr;
        }
    }

    // move only
    HybridPtr(const HybridPtr&) = delete;
    HybridPtr& operator=(const HybridPtr&) = delete;

    /// @brief Creates a HybridPtr from nullptr
    HybridPtr(std::nullptr_t)
        : ptr(nullptr)
        , ownership(false) {}

    /// @brief Move constructor
    HybridPtr(HybridPtr&& other) noexcept
        : ptr(std::exchange(other.ptr, nullptr)),
          ownership(std::exchange(other.ownership, false)) {}

    /// @brief Move constructor for compatible types
    template<class U>
    HybridPtr(HybridPtr<U>&& other)
        : ptr(std::exchange(other.ptr, nullptr)),
          ownership(std::exchange(other.ownership, false)) {}

    /// @brief Move assignment operator
    HybridPtr& operator=(HybridPtr&& other) {
        reset();
        ptr = std::exchange(other.ptr, nullptr);
        ownership = std::exchange(other.ownership, false);
        return *this;
    }

    /// @brief Move assignment operator for compatible types
    template<class U>
    HybridPtr& operator=(HybridPtr<U>&& other) {
        reset();
        ptr = std::exchange(other.ptr, nullptr);
        ownership = std::exchange(other.ownership, false);
        return *this;
    }

    /// @brief Resets the HybridPtr from nullptr
    HybridPtr& operator=(std::nullptr_t) {
        reset();
        return *this;
    }

    /// @brief Gets the underlying raw pointer
    /// @return A pointer to the object, or nullptr
    T* get() { return ptr; }
    
    /// @brief Gets the underlying constant raw pointer
    /// @return A constant pointer to the object, or nullptr
    const T* get() const { return ptr; }

    /// @brief Gets a reference to the object
    /// @return A reference to the pointed-to object
    /// @warning Behavior is undefined if the pointer is nullptr
    T& ref() { return *ptr; };
    
    /// @brief Gets a constant reference to the object
    /// @return A constant reference to the pointed-to object
    /// @warning Behavior is undefined if the pointer is nullptr
    const T& ref() const { return *ptr; }

    /// @brief Member access operator
    T* operator->() { return get(); }
    
    /// @brief Constant member access operator
    const T* operator->() const { return get(); }

    /// @brief Dereference operator
    T& operator*() { return *ptr; };
    
    /// @brief Constant dereference operator
    const T& operator*() const { return *ptr; }

    /// @brief Releases ownership and returns the raw pointer
    /// @return The raw pointer
    /// @note The caller becomes responsible for the pointer's lifetime
    T* release() noexcept {
        ownership = false;
        return std::exchange(ptr, nullptr);
    }

    /// @brief Resets the HybridPtr to an empty borrowed state, deleting owned content
    void reset() noexcept {
        if (ownership) {
            delete ptr;
        }
        ptr = nullptr;
        ownership = false;
    }

    /// @brief Resets the HybridPtr with a new pointer and ownership state
    /// @param newPtr The new pointer to manage
    /// @param isOwner Whether this HybridPtr should take ownership of the new pointer
    void reset(T* newPtr, bool isOwner) noexcept {
        reset();
        ptr = newPtr;
        ownership = isOwner;
    }

    /// @brief Checks if this HybridPtr owns its content
    /// @return True if it has ownership, false if it is borrowing
    bool hasOwnership() const {
        return ownership;
    }

    /// @brief Checks if this HybridPtr borrows its content
    /// @return True if it is borrowing, false if it has ownership
    bool isBorrowed() const {
        return !hasOwnership();
    }

    /// @brief Compares two HybridPtr objects for equality
    friend bool operator==(const HybridPtr<T>& a, const HybridPtr<T>& b) {
        return a.get() == b.get();
    }

    /// @brief Compares two HybridPtr objects for inequality
    friend bool operator!=(const HybridPtr<T>& a, const HybridPtr<T>& b) {
        return a.get() != b.get();
    }

    /// @brief Compares a HybridPtr with nullptr for equality
    friend bool operator==(const HybridPtr<T>& a, std::nullptr_t) {
        return a.get() == nullptr;
    }

    /// @brief Compares a HybridPtr with nullptr for inequality
    friend bool operator!=(const HybridPtr<T>& a, std::nullptr_t) {
        return a.get() != nullptr;
    }
};

} // namespace cx

namespace std {
    /// @brief Hash specialization for @ref cx::HybridPtr
    /// @tparam T The type of the object pointed to
    template <class T>
    struct hash<cx::HybridPtr<T>> {
        /// @brief Calculates the hash of the @ref cx::HybridPtr
        /// @param x The HybridPtr to hash
        /// @return The calculated hash value
        size_t operator()(const cx::HybridPtr<T>& x) const {
            return hash<T*>()(x.get());
        }
    };
} // namespace std
