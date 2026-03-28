#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <set>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <initializer_list>
#include <source_location>
#include <unordered_set>
#include <variant>
#include <vector>

namespace cx {

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8  = int8_t;

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

using usize = size_t;
using isize = ptrdiff_t;

using uint = unsigned int;
using ulong = unsigned long;

using uintptr = uintptr_t;

using byte = std::uint8_t;

struct NoneType {};
constexpr NoneType None;

template <typename T>
using RefWrapper = std::reference_wrapper<T>;

template <typename T>
using Vec = std::vector<T>;

template <typename T>
using Set = std::set<T>;

template <typename T>
using HashSet = std::unordered_set<T>;

template <typename T, usize E = std::dynamic_extent>
using Slice = std::span<T, E>;

template <typename T, usize N>
using Array = std::array<T, N>;

template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template <typename L, typename R>
using Pair = std::pair<L, R>;

template <typename Signature>
using Func = std::function<Signature>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename... Variants>
using Union = std::variant<Variants...>;

template <typename... Variants>
using NullableUnion = Union<NoneType, Variants...>;

using String = std::string;
using StringView = std::string_view;

template <typename T>
using MaybeUninit = std::optional<T>;

template <typename T>
using Optional = std::optional<T>;

template <typename T>
using InitList = std::initializer_list<T>;

using SourceLoc = std::source_location;

using Typeid = std::type_info;
using TypeIndex = std::type_index;

using UiScaleUnit = float;
using GameScaleUnit = float;

using TrueType = std::true_type;
using FalseType = std::false_type;

}
