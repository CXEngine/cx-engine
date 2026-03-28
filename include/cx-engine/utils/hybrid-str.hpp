#pragma once

#include <cx-engine/defs/types.hpp>
#include <cx-engine/defs/errors.hpp>

#include <variant>

namespace cx {

/// @brief A hybrid string that can either borrow a StringView or take ownership of a String.
///
/// HybridStr provides a way to handle strings without unnecessary allocations by borrowing
/// a view when possible, while allowing for ownership transition when a persistent or
/// mutable string is required.
class HybridStr {
protected:
    /// @brief Internal storage using a variant-like Union of StringView and String.
    Union<StringView, String> data;

public:
    /// @brief Creates an empty HybridStr (borrowed empty StringView).
    HybridStr() noexcept : data(StringView{}) {}

    /// @brief Creates a HybridStr that borrows the given string view
    /// @param borrowed The string view to borrow
    /// @return A new HybridStr instance in borrowed state
    /// @note The caller must ensure the viewed string outlives this HybridStr
    static HybridStr Borrow(StringView borrowed) noexcept {
        HybridStr result;
        result.data = borrowed;
        return result;
    }

    /// @brief Creates a HybridStr that takes ownership of the given string
    /// @param owned The string to take ownership of
    /// @return A new HybridStr instance in owned state
    static HybridStr Take(String&& owned) noexcept {
        HybridStr result;
        result.data = std::move(owned);
        return result;
    }

    /// @brief Creates a HybridStr that takes ownership of a newly constructed string
    /// @tparam Args Argument types for the String constructor
    /// @param args Arguments passed to the String constructor
    /// @return A new HybridStr instance in owned state
    template <typename... Args>
    static HybridStr Make(Args&&... args) {
        return Take(String(std::forward<Args>(args)...));
    }

    /// @brief Borrows from a string view
    /// @param sv String view to borrow from
    HybridStr(StringView sv) noexcept : data(sv) {}
    
    /// @brief Borrows from a C-style string
    /// @param cs C-style string to borrow from
    HybridStr(const char* cs) noexcept : data(StringView(cs)) {}

    /// @brief Takes ownership of a moved string
    /// @param s String to take ownership of
    HybridStr(String&& s) noexcept : data(std::move(s)) {}

    /// @brief Default destructor.
    ~HybridStr() = default;

    // move only
    HybridStr(const HybridStr&) = delete;
    HybridStr& operator=(const HybridStr&) = delete;

    HybridStr(HybridStr&& other) noexcept = default;
    HybridStr& operator=(HybridStr&& other) noexcept = default;

    /// @brief Checks if this HybridStr owns its content (i.e., stores a String)
    /// @return True if it has ownership, false if it is borrowing
    bool hasOwnership() const noexcept {
        return std::holds_alternative<String>(data);
    }

    /// @brief Checks if this HybridStr borrows its content (i.e. stores a StringView)
    /// @return True if it is borrowing, false if it has ownership
    bool isBorrowed() const noexcept {
        return std::holds_alternative<StringView>(data);
    }

    /// @brief Gets a view of the string content
    /// @return A StringView pointing to the content, regardless of ownership state
    StringView get() const noexcept {
        if (const auto* s = std::get_if<String>(&data)) {
            return *s;
        }
        return std::get<StringView>(data);
    }

    /// @brief Attempts to get a pointer to the owned string
    /// @return A pointer to the owned string, or nullptr if the state is borrowed
    String* tryGetOwned() {
        if (!hasOwnership()) {
            return nullptr;
        }
        return &std::get<String>(data);
    }

    /// @brief Attempts to get a constant pointer to the owned string
    /// @return A constant pointer to the owned string, or nullptr if the state is borrowed
    const String* tryGetOwned() const {
        if (!hasOwnership()) {
            return nullptr;
        }
        return &std::get<String>(data);
    }

    /// @brief Implicit conversion to StringView for seamless use in string operations
    /// @return A StringView pointing to the content
    operator StringView() const noexcept {
        return get();
    }

    /// @brief Resets the HybridStr to an empty borrowed state
    void reset() noexcept {
        data = StringView{};
    }

    /// @brief Resets the HybridStr to a new borrowed state
    /// @param sv The new string view to borrow
    void reset(StringView sv) noexcept {
        data = sv;
    }

    /// @brief Resets the HybridStr to a new owned state
    /// @param s The new string to take ownership of
    void reset(String&& s) noexcept {
        data = std::move(s);
    }

    /// @brief Ensures the string is owned, copying if it was previously borrowed
    /// @note If the string was previously borrowed, it creates a new owned String
    ///       by copying the borrowed content.
    void makeOwned() {
        if (isBorrowed()) {
            data = String(std::get<StringView>(data));
        }
    }

    /// @brief Compares two HybridStr objects for equality
    friend bool operator==(const HybridStr& a, const HybridStr& b) noexcept {
        return a.get() == b.get();
    }

    /// @brief Compares two HybridStr objects for inequality
    friend bool operator!=(const HybridStr& a, const HybridStr& b) noexcept {
        return a.get() != b.get();
    }

    /// @brief Compares a HybridStr with a StringView for equality
    friend bool operator==(const HybridStr& a, StringView b) noexcept {
        return a.get() == b;
    }

    /// @brief Compares a StringView with a HybridStr for equality
    friend bool operator==(StringView a, const HybridStr& b) noexcept {
        return a == b.get();
    }

    /// @brief Compares two HybridStr objects for less-than relationship
    friend bool operator<(const HybridStr& a, const HybridStr& b) noexcept {
        return a.get() < b.get();
    }
};

} // namespace cx

namespace std {
    /// @brief Hash specialization for cx::HybridStr
    /// @note Allows HybridStr to be used as a key in unordered containers
    template <>
    struct hash<cx::HybridStr> {
        /// @brief Calculates the hash of the HybridStr.
        /// @param x The HybridStr to hash
        /// @return The calculated hash value
        size_t operator()(const cx::HybridStr& x) const {
            return hash<cx::StringView>()(x.get());
        }
    };
}
