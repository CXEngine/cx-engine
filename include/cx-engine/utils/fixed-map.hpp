#include <cx-engine/defs/types.hpp>

#include <utility>
#include <stdexcept>

namespace cx {

class FixedMapKeyNotFoundError: std::out_of_range {
public:
    using std::out_of_range::out_of_range;
    FixedMapKeyNotFoundError() : FixedMapKeyNotFoundError("Key not found") {}
};

template <typename K, typename V, usize N>
class FixedMap {
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = Pair<K, V>;
    using storage_type = Array<value_type, N>;

    using const_iterator = typename storage_type::const_iterator;

    constexpr FixedMap(const storage_type& data)
        : data(data) {}

    template <typename... Pairs>
    constexpr FixedMap(Pairs&&... pairs)
        : data{std::forward<Pairs>(pairs)...} {}

    constexpr const V* find(const K& key) const {
        for (const auto& kv : data) {
            if (kv.first == key) {
                return &kv.second;
            }
        }
        return nullptr;
    }

    constexpr const V& at(const K& key) const {
        const V* value = find(key);
        if (!value) {
            throw FixedMapKeyNotFoundError();
        }
        return *value;
    }

    constexpr bool isEmpty() const {
        return N == 0;
    }

    constexpr usize getSize() const {
        return N;
    }

    constexpr const_iterator begin() const {
        return data.begin();
    }

    constexpr const_iterator end() const {
        return data.end();
    }

    constexpr const_iterator cbegin() const {
        return data.cbegin();
    }

    constexpr const_iterator cend() const {
        return data.cend();
    }

private:
    const storage_type data;
};

template <typename K, typename V, usize N>
FixedMap(const Array<Pair<K, V>, N>&) -> FixedMap<K, V, N>;

template <typename K, typename V, typename... Rest>
FixedMap(Pair<K, V>, Rest...)
    -> FixedMap<K, V, 1 + sizeof...(Rest)>;

template <typename K, typename V, typename... Pairs>
constexpr auto makeFixedMap(Pairs&&... pairs) {
    return FixedMap<K, V, sizeof...(Pairs)> (
        Array<Pair<K, V>, sizeof...(Pairs)> {
            std::forward<Pairs>(pairs)...
        }
    );
}

}
