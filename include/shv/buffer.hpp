#pragma once


#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <span>

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <string_view>

namespace shv
{


// A buffer (static_vector/inplace_vector) with fixed capacity.
// T: Element type
// N: Fixed capacity
// SizeType: Type used for size tracking (defaults to std::size_t)
template<typename T, std::size_t N, typename SizeType = std::size_t>
class buffer {
public:
    using value_type = T;
    using size_type = SizeType;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;

    // Default construct an empty buffer
    constexpr buffer() noexcept = default;

    // Iterators
    constexpr iterator begin() noexcept { return storage_.data(); }
    constexpr const_iterator begin() const noexcept { return storage_.data(); }
    constexpr const_iterator cbegin() const noexcept { return storage_.data(); }

    constexpr iterator end() noexcept { return storage_.data() + size_; }
    constexpr const_iterator end() const noexcept { return storage_.data() + size_; }
    constexpr const_iterator cend() const noexcept { return storage_.data() + size_; }

    // Capacity
    constexpr size_type size() const noexcept { return size_; }
    constexpr static size_type capacity() noexcept { return N; }
    constexpr bool empty() const noexcept { return size_ == 0; }
    constexpr bool full() const noexcept { return size_ == N; }

    // Element access
    constexpr reference operator[](size_type i) {
        return storage_[i]; 
    }
    constexpr const_reference operator[](size_type i) const {
        return storage_[i]; 
    }

    constexpr reference front() {
        return storage_[0]; 
    }
    constexpr const_reference front() const {
        return storage_[0]; 
    }

    constexpr reference back() {
        return storage_[size_ - 1]; 
    }
    constexpr const_reference back() const {
        return storage_[size_ - 1]; 
    }

    constexpr pointer data() noexcept {
        return storage_.data(); 
    }
    constexpr const_pointer data() const noexcept {
        return storage_.data(); 
    }

    // Modifiers
    constexpr void clear() noexcept {
        // For non-trivial T, you might want to destruct old elements.
        // For trivially destructible T, this is fine.
        // If you want to be more general:
        // for (size_type i = 0; i < size_; ++i)
        //   storage_[i].~T(); // (requires placement new if constructed that way)
        size_ = 0;
    }

    constexpr void push_back(const T& value) {
        if (full()) {
            throw std::out_of_range("buffer is full");
        }
        storage_[size_++] = value;
    }

    constexpr void push_back(T&& value) {
        if (full()) {
            throw std::out_of_range("buffer is full");
        }
        storage_[size_++] = std::move(value);
    }

    template<typename... Args>
    constexpr reference emplace_back(Args&&... args) {
        if (full()) {
            throw std::out_of_range("buffer is full");
        }
        new (&storage_[size_]) T(std::forward<Args>(args)...);
        ++size_;
        return back();
    }

    constexpr void pop_back() {
        if (empty()) {
            throw std::out_of_range("buffer is empty");
        }
        // If T is non-trivially destructible, call destructor:
        storage_[--size_].~T();
    }

    // Conversion to std::span for "view" semantics
    constexpr operator std::span<T>() noexcept {
        return std::span<T>(data(), size_);
    }
    constexpr operator std::span<const T>() const noexcept {
        return std::span<const T>(data(), size_);
    }

private:
    std::array<T, N> storage_{};
    size_type size_ = 0;
};

// Free functions to allow range-based for without using member begin/end:
template<typename T, std::size_t N, typename SizeType>
constexpr T* begin(buffer<T, N, SizeType>& b) noexcept {
    return b.begin();
}
template<typename T, std::size_t N, typename SizeType>
constexpr const T* begin(const buffer<T, N, SizeType>& b) noexcept {
    return b.begin();
}
template<typename T, std::size_t N, typename SizeType>
constexpr T* end(buffer<T, N, SizeType>& b) noexcept {
    return b.end();
}
template<typename T, std::size_t N, typename SizeType>
constexpr const T* end(const buffer<T, N, SizeType>& b) noexcept {
    return b.end();
}

}