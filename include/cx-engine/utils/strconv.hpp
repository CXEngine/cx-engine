#pragma once

#include <cx-engine/defs/errors.hpp>
#include <cx-engine/defs/types.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

#include <charconv>
#include <sstream>
#include <type_traits>

namespace cx {

class StrconvError: public Exception {
public:
    using Exception::Exception;
};

template <typename Enum>
Enum enumFromString(StringView s);

template <typename Enum>
String enumToString(Enum value);

inline namespace strconv {

namespace detail {

template <typename T> struct IsSfVectorType: FalseType {};
template <typename U> struct IsSfVectorType<sf::Vector2<U>>: TrueType {};
template <typename T> constexpr bool IsSfVector = IsSfVectorType<T>::value;

template <typename T> struct IsSfRectType: FalseType {};
template <typename U> struct IsSfRectType<sf::Rect<U>>: TrueType {};
template <typename T> constexpr bool IsSfRect = IsSfRectType<T>::value;

template <typename T> struct IsOptionalType: FalseType {};
template <typename T> struct IsOptionalType<Optional<T>>: TrueType {};
template <typename T> constexpr bool IsOptional = IsOptionalType<T>::value;

} // namespace detail

template <typename OutputIt>
usize splitStringView(StringView s, char delimiter, OutputIt out) {
    usize count = 0;
    usize start = 0;
    usize end = s.find(delimiter);
    while (end != StringView::npos) {
        *out++ = s.substr(start, end - start);
        count++;
        start = end + 1;
        end = s.find(delimiter, start);
    }
    if (start <= s.size()) {
        *out++ = s.substr(start);
        count++;
    }
    return count;
}

template <typename T>
inline T parseComponent(StringView sv) {
    T value;
    auto [p, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    if (ec != std::errc()) {
        throw StrconvError("parse error: failed to convert component '" + String(sv) + "'");
    }
    return value;
}

template <typename T>
T fromString(StringView s) {
    if constexpr (std::is_same_v<T, bool>) {
        if (s == "true" || s == "1") return true;
        if (s == "false" || s == "0") return false;
        throw StrconvError("Failed to convert string to bool: '" + String(s) + "'");
    } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        T value;
        auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
        if (ec != std::errc()) throw StrconvError("parse error");
        return value;
    } else if constexpr (std::is_enum_v<T>) {
        return cx::enumFromString<T>(s);
    } else if constexpr (detail::IsSfVector<T>) {
        Array<StringView, 2> tokens;
        usize numToks = splitStringView(s, ',', tokens.begin());
        if (numToks != 2) throw StrconvError("Failed to parse sf::Vector2<T>: expected 'x,y'");
        using ValueType = typename T::value_type;
        return T{parseComponent<ValueType>(tokens[0]), parseComponent<ValueType>(tokens[1])};
    } else if constexpr (detail::IsSfRect<T>) {
        Array<StringView, 4> tokens;
        usize numToks = splitStringView(s, ',', tokens.begin());
        if (numToks != 4) throw StrconvError("Failed to parse sf::Rect<T>: expected 'left,top,width,height'");
        using ValueType = typename T::value_type;
        return T{parseComponent<ValueType>(tokens[0]), parseComponent<ValueType>(tokens[1]), parseComponent<ValueType>(tokens[2]), parseComponent<ValueType>(tokens[3])};
    } else if constexpr (std::is_same_v<T, sf::Color>) {
        Array<StringView, 4> tokens;
        usize numToks = splitStringView(s, ',', tokens.begin());
        if (numToks < 3 || numToks > 4) throw StrconvError("Failed to parse sf::Color: expected 'r,g,b' or 'r,g,b,a'");
        u8 r = parseComponent<u8>(tokens[0]);
        u8 g = parseComponent<u8>(tokens[1]);
        u8 b = parseComponent<u8>(tokens[2]);
        u8 a = (numToks == 4) ? parseComponent<u8>(tokens[3]) : 255;
        return sf::Color{r, g, b, a};
    } else if constexpr (detail::IsOptional<T>) {
        if (s.empty()) {
            return T{};
        }
        using ValueType = typename T::value_type;
        return fromString<ValueType>(s);
    }
    else {
        std::istringstream iss{ String(s) };
        T value;
        iss >> value;
        if (iss.fail() || iss.bad()) {
            throw StrconvError("Failed to convert string to type: '" + String(s) + "'");
        }
        return value;
    }
}

template <typename T>
String toString(const T& v) {
    if constexpr (std::is_enum_v<T>) {
        return cx::enumToString(v);
    } else if constexpr (detail::IsSfVector<T>) {
        return toString(v.x) + "," + toString(v.y);
    } else if constexpr (detail::IsSfRect<T>) {
        return toString(v.left) + "," + toString(v.top) + "," + toString(v.width) + "," + toString(v.height);
    } else if constexpr (std::is_same_v<T, sf::Color>) {
        return toString(v.r) + "," + toString(v.g) + "," + toString(v.b) + "," + toString(v.a);
    } else if constexpr (detail::IsOptional<T>) {
        if (!v.has_value()) {
            return "nullopt";
        }
        return toString(v.value());
    }
    else {
        return std::to_string(v);
    }
}

} // namespace strconv
} // namespace cx
