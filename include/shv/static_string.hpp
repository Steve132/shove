#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <string_view>
#include <compare>
#include <limits>
#include <iosfwd>  // for std::basic_ostream and std::basic_istream

#include <iosfwd>  // for std::basic_ostream and std::basic_istream
#include <ostream>
#include <istream>
#include <string_view>


#include <array>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <string_view>
#include <compare>
#include <algorithm> // for std::min

namespace shv
{
template<typename T>
concept CharType = 
    std::is_same_v<T, char> ||
    std::is_same_v<T, wchar_t> ||
#ifdef __cpp_lib_char8_t
    std::is_same_v<T, char8_t> ||
#endif
    std::is_same_v<T, char16_t> ||
    std::is_same_v<T, char32_t>;

template<
    std::size_t N, 
    CharType CharT = char, 
    typename Traits = std::char_traits<CharT>
>
class basic_static_string : public std::array<CharT,N> {
public:
    static_assert(N > 1, "Capacity including null terminator must be greater than 1.");

    using base_t = std::array<CharT,N>;

    using traits_type = Traits;
    using value_type = CharT;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = value_type*;
    using const_iterator = const value_type*;

    static constexpr size_type npos = static_cast<size_type>(N);

    constexpr basic_static_string() noexcept {
        buffer_[0] = value_type();
    }

    constexpr basic_static_string(const basic_static_string& other) noexcept = default;
    constexpr basic_static_string(basic_static_string&& other) noexcept = default;
    constexpr basic_static_string& operator=(const basic_static_string& other) noexcept = default;
    constexpr basic_static_string& operator=(basic_static_string&& other) noexcept = default;

    constexpr basic_static_string(const CharT* s) {
        assign_from(s, Traits::length(s));
    }

    constexpr basic_static_string(const CharT* s, size_type count) {
        assign_from(s, count);
    }

    using base_t::operator=;
    using base_t::operator[];
    using base_t::data;

    constexpr size_type max_chars() const noexcept { return N - 1; }

    constexpr void assign_from(const CharT* s, size_type len) {
        if (len >= N)
            throw std::out_of_range("basic_static_string: string too long");
        Traits::copy(buffer_, s, len);
        buffer_[len] = value_type(); // null terminator
    }

    constexpr void assign(std::basic_string_view<CharT, Traits> sv) {
        if (sv.size() >= N)
            throw std::out_of_range("basic_static_string: string_view too long");
        assign_from(sv.data(), sv.size());
    }

    // Capacity
    constexpr size_type size() const noexcept {
        return Traits::length(data());
    }

    constexpr static size_type capacity() noexcept { return N - 1; }
    constexpr bool empty() const noexcept { return buffer_[0] == 0; }

    // Modifiers
    constexpr void clear() noexcept {
        buffer_[0] = value_type();
    }

    // Conversion to std::basic_string_view
    constexpr operator std::basic_string_view<CharT, Traits>() const noexcept {
        return { data(), size() };
    }

private:
    CharT buffer_[N];

    // Convert various types to std::basic_string_view<CharT, Traits>
    static constexpr std::basic_string_view<CharT, Traits> to_view(const basic_static_string& s) noexcept {
        return { s.data(), s.size() };
    }

    static constexpr std::basic_string_view<CharT, Traits> to_view(std::basic_string_view<CharT, Traits> sv) noexcept {
        return sv;
    }

    static constexpr std::basic_string_view<CharT, Traits> to_view(const CharT* s) {
        return { s, Traits::length(s) };
    }

    template<std::size_t M>
    static constexpr std::basic_string_view<CharT, Traits> to_view(const CharT(&arr)[M]) {
        // char arrays also end with '\0' in string literals. 
        // If not, Traits::length will stop at the first null.
        return { arr, Traits::length(arr) };
    }

    template<class String>
    requires requires (const String& x) { { x.data() } -> std::convertible_to<const CharT*>; { x.size() } -> std::convertible_to<std::size_t>; }
    static constexpr std::basic_string_view<CharT, Traits> to_view(const String& str) {
        return { str.data(), str.size() };
    }

    static constexpr std::strong_ordering compare_views(std::basic_string_view<CharT, Traits> lhs, 
                                                        std::basic_string_view<CharT, Traits> rhs) noexcept {
        const size_type count = std::min(lhs.size(), rhs.size());
        int cmp = Traits::compare(lhs.data(), rhs.data(), count);
        if (cmp < 0) return std::strong_ordering::less;
        if (cmp > 0) return std::strong_ordering::greater;
        // If prefix matches and lengths differ
        if (lhs.size() < rhs.size()) return std::strong_ordering::less;
        if (lhs.size() > rhs.size()) return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }
    
public:
    // basic_static_string <=> basic_static_string
    friend constexpr std::strong_ordering operator<=>(const basic_static_string& lhs, const basic_static_string& rhs) noexcept {
        return compare_views(to_view(lhs), to_view(rhs));
    }

    // basic_static_string <=> std::basic_string_view
    friend constexpr std::strong_ordering operator<=>(const basic_static_string& lhs, std::basic_string_view<CharT, Traits> rhs) noexcept {
        return compare_views(to_view(lhs), rhs);
    }

    friend constexpr std::strong_ordering operator<=>(std::basic_string_view<CharT, Traits> lhs, const basic_static_string& rhs) noexcept {
        return compare_views(lhs, to_view(rhs));
    }

    // basic_static_string <=> const CharT*
    friend constexpr std::strong_ordering operator<=>(const basic_static_string& lhs, const CharT* rhs) {
        return compare_views(to_view(lhs), to_view(rhs));
    }

    friend constexpr std::strong_ordering operator<=>(const CharT* lhs, const basic_static_string& rhs) {
        return compare_views(to_view(lhs), to_view(rhs));
    }

    // basic_static_string <=> char arrays
    template<std::size_t M>
    friend constexpr std::strong_ordering operator<=>(const basic_static_string& lhs, const CharT(&rhs)[M]) {
        return compare_views(to_view(lhs), to_view(rhs));
    }

    template<std::size_t M>
    friend constexpr std::strong_ordering operator<=>(const CharT(&lhs)[M], const basic_static_string& rhs) {
        return compare_views(to_view(lhs), to_view(rhs));
    }

    // basic_static_string <=> std::basic_string<CharT, Traits, Alloc>
    template<class Alloc>
    friend constexpr std::strong_ordering operator<=>(const basic_static_string& lhs, const std::basic_string<CharT, Traits, Alloc>& rhs) {
        return compare_views(to_view(lhs), to_view(rhs));
    }

    template<class Alloc>
    friend constexpr std::strong_ordering operator<=>(const std::basic_string<CharT, Traits, Alloc>& lhs, const basic_static_string& rhs) {
        return compare_views(to_view(lhs), to_view(rhs));
    }
    friend constexpr bool operator==(const basic_static_string& lhs, const basic_static_string& rhs) noexcept {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

friend constexpr bool operator==(const basic_static_string& lhs, std::basic_string_view<CharT, Traits> rhs) noexcept {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

friend constexpr bool operator==(std::basic_string_view<CharT, Traits> lhs, const basic_static_string& rhs) noexcept {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

friend constexpr bool operator==(const basic_static_string& lhs, const CharT* rhs) {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

friend constexpr bool operator==(const CharT* lhs, const basic_static_string& rhs) {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

template<std::size_t M>
friend constexpr bool operator==(const basic_static_string& lhs, const CharT(&rhs)[M]) {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

template<std::size_t M>
friend constexpr bool operator==(const CharT(&lhs)[M], const basic_static_string& rhs) {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

template<class Alloc>
friend constexpr bool operator==(const basic_static_string& lhs, const std::basic_string<CharT, Traits, Alloc>& rhs) {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

template<class Alloc>
friend constexpr bool operator==(const std::basic_string<CharT, Traits, Alloc>& lhs, const basic_static_string& rhs) {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

};



template<std::size_t N>
using static_string = basic_static_string<N, char>;


}

namespace shv{

template<std::size_t N, typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits>& 
operator<<(std::basic_ostream<CharT, Traits>& os, const basic_static_string<N, CharT, Traits>& str) {
    // Output all characters of the string
    return os << str.data();
}

template<std::size_t N, typename CharT, typename Traits>
inline std::basic_istream<CharT, Traits>& 
operator>>(std::basic_istream<CharT, Traits>& is, basic_static_string<N, CharT, Traits>& str) {
    is >> std::skipws;

    std::streamsize oldWidth = is.width();
    is.width(N);

    CharT value[N];
    is >> value;
    is.width(oldWidth);
    std::copy(value, value + N, str.data());
    return is;
}

}