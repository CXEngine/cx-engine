#include <cx-engine/defs/types.hpp>

#include <utility>
#include <stdexcept>

namespace cx {

/// @brief Exception thrown when a key is not found in a FixedMap.
class FixedMapKeyNotFoundError: std::out_of_range {
public:
    using std::out_of_range::out_of_range;

    /// @brief Creates a new FixedMapKeyNotFoundError with a default message.
    FixedMapKeyNotFoundError() : FixedMapKeyNotFoundError("Key not found") {}
};

/// @brief A fixed-size, compile-time map implementation using an Array.
///
/// FixedMap provides a read-only mapping of keys to values with a size determined
/// at compile time. It uses linear search for lookups, making it suitable for
/// small sets of data where a full hash map or tree would be overkill.
///
/// @tparam K Type of keys
/// @tparam V Type of values
/// @tparam N Number of elements in the map
template <typename K, typename V, usize N>
class FixedMap {
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = Pair<K, V>;
    using storage_type = Array<value_type, N>;

    using const_iterator = typename storage_type::const_iterator;

    /// @brief Constructs a FixedMap from an existing Array of pairs
    /// @param data The array containing key-value pairs
    constexpr FixedMap(const storage_type& data)
        : data(data) {}

    /// @brief Constructs a FixedMap from a variadic list of pairs
    /// @tparam Pairs Types of the pairs
    /// @param pairs Key-value pairs to initialize the map with
    template <typename... Pairs>
    constexpr FixedMap(Pairs&&... pairs)
        : data{std::forward<Pairs>(pairs)...} {}

    /// @brief Searches for a value associated with the given key
    /// @param key The key to look for
    /// @return Pointer to the value if found, or nullptr if not found
    constexpr const V* find(const K& key) const {
        for (const auto& kv : data) {
            if (kv.first == key) {
                return &kv.second;
            }
        }
        return nullptr;
    }

    /// @brief Accesses the value associated with the given key
    /// @param key The key to look for
    /// @return Constant reference to the value
    /// @throws FixedMapKeyNotFoundError if the key is not found
    constexpr const V& at(const K& key) const {
        const V* value = find(key);
        if (!value) {
            throw FixedMapKeyNotFoundError();
        }
        return *value;
    }

    /// @brief Checks if the map contains no elements
    /// @return True if empty (N == 0), false otherwise
    constexpr bool isEmpty() const {
        return N == 0;
    }

    /// @brief Gets the total number of elements in the map
    /// @return Number of elements
    constexpr usize getSize() const {
        return N;
    }

    /// @brief Returns a constant iterator to the beginning of the map
    /// @return Constant iterator to the first element
    constexpr const_iterator begin() const {
        return data.begin();
    }

    /// @brief Returns a constant iterator to the end of the map
    /// @return Constant iterator to the element following the last element
    constexpr const_iterator end() const {
        return data.end();
    }

    /// @brief Returns a constant iterator to the beginning of the map
    /// @return Constant iterator to the first element
    constexpr const_iterator cbegin() const {
        return data.cbegin();
    }

    /// @brief Returns a constant iterator to the end of the map
    /// @return Constant iterator to the element following the last element
    constexpr const_iterator cend() const {
        return data.cend();
    }

private:
    const storage_type data; //< Internal storage for key-value pairs
};

/// @brief Deduction guide for FixedMap from an Array.
template <typename K, typename V, usize N>
FixedMap(const Array<Pair<K, V>, N>&) -> FixedMap<K, V, N>;

/// @brief Deduction guide for FixedMap from a list of pairs.
template <typename K, typename V, typename... Rest>
FixedMap(Pair<K, V>, Rest...)
    -> FixedMap<K, V, 1 + sizeof...(Rest)>;

/// @brief Utility function to create a FixedMap with explicit key and value types
/// @tparam K Type of keys
/// @tparam V Type of values
/// @tparam Pairs Types of the pairs
/// @param pairs Key-value pairs to initialize the map with
/// @return A new FixedMap instance
template <typename K, typename V, typename... Pairs>
constexpr auto makeFixedMap(Pairs&&... pairs) {
    return FixedMap<K, V, sizeof...(Pairs)> (
        Array<Pair<K, V>, sizeof...(Pairs)> {
            std::forward<Pairs>(pairs)...
        }
    );
}

} // namespace cx
