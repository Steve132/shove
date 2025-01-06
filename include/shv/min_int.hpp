#pragma once

#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <type_traits>

namespace shv{
    template<size_t N>
    using uint_fast_can_hold_t = std::conditional_t<N <= std::numeric_limits<std::uint_fast8_t>::max(),std::uint_fast8_t,
        std::conditional_t<N <= std::numeric_limits<std::uint_fast16_t>::max(),std::uint_fast16_t,
        std::conditional_t<N <= std::numeric_limits<std::uint_fast32_t>::max(),std::uint_fast32_t,
        std::conditional_t<N <= std::numeric_limits<std::uint_fast64_t>::max(),std::uint_fast64_t,
        std::size_t
    >>>>;
/*
    template <typename SpanType>
        requires requires(const SpanType& sp){
            { []() constexpr { return std::size(t); }() } -> std::convertible_to<std::size_t>;
        }
    using uint_fast_can_index_t = uint_fast_can_hold_t<[]() constexpr { return std::size(SpanType{}); }()>;
    */
}