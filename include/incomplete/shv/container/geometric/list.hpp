#ifndef GEOMETRIC_LIST_H
#define GEOMETRIC_LIST_H

#include <iterator>
#include <memory>
#include <cstddef>
#include <initializer_list>
#include <utility>

namespace shv {

namespace impl{
    template <typename T,typename Allocator,typename TraitsTag>
    class geometric_block_list;
}

template <typename T, typename Allocator = std::allocator<T>>
class geometric_list{
    /*protected:
        using base_blockset_t=impl::geometric_block_set<T,Allocator>;*/
public:
    using value_type=T;
    using allocator_type=Allocator;

    using size_type=typename std::allocator_traits<Allocator>::size_type;
    using difference_type=typename std::allocator_traits<Allocator>::difference_type;
    using reference=T&;
    using const_reference=const T&;
    using pointer=typename std::allocator_traits<Allocator>::pointer;
    using const_pointer=typename std::allocator_traits<Allocator>::const_pointer;

    // Constructors
    geometric_list();
    geometric_list(const geometric_list&);
    geometric_list(geometric_list&&) noexcept;
    explicit geometric_list(const Allocator&);
    geometric_list(size_type count, const value_type& value, const Allocator& alloc = Allocator());
    explicit geometric_list(size_type count, const Allocator& alloc = Allocator());
    template <typename InputIt>
    geometric_list(InputIt first, InputIt last, const Allocator& alloc = Allocator());
    geometric_list(std::initializer_list<value_type> init, const Allocator& alloc = Allocator());

    // Destructor
    ~geometric_list();

    // Assignment operators
    geometric_list& operator=(const geometric_list&);
    geometric_list& operator=(geometric_list&&) noexcept;
    geometric_list& operator=(std::initializer_list<value_type>);

    // Capacity
    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;

    // Element access
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    // Random access
    reference operator[](size_type);
    const_reference operator[](size_type) const;
    reference at(size_type);
    const_reference at(size_type) const;

    // Modifiers
    void clear() noexcept;
    void push_back(const value_type&);
    void push_back(value_type&&);
    void push_front(const value_type&);
    void push_front(value_type&&);
    void pop_back();
    void pop_front();

    template <typename... Args>
    void emplace_front(Args&&... args);

    template <typename... Args>
    void emplace_back(Args&&... args);

    template <typename InputIt>
    void prepend_range(InputIt first, InputIt last);

    template <typename InputIt>
    void append_range(InputIt first, InputIt last);

    void resize(size_type count);
    void resize(size_type count, const value_type& value);
    void reserve(size_type new_cap);
    
    void assign(size_type count, const value_type& value);
    template <typename InputIt>
    void assign(InputIt first, InputIt last);
    void assign(std::initializer_list<T>);

    template <typename InputIt>
    void assign_range(InputIt first, InputIt last);



    // Allocator
    allocator_type get_allocator() const noexcept;

    // Nested iterator class declarations
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;

        iterator() noexcept;
        iterator(const iterator&) noexcept;
        iterator& operator=(const iterator&) noexcept;
        ~iterator();

        reference operator*() const;
        pointer operator->() const;
        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);
        iterator& operator+=(difference_type);
        iterator& operator-=(difference_type);
        iterator operator+(difference_type) const;
        iterator operator-(difference_type) const;
        difference_type operator-(const iterator&) const;
        reference operator[](difference_type) const;
        bool operator==(const iterator&) const;
        bool operator!=(const iterator&) const;
        bool operator<(const iterator&) const;
        bool operator<=(const iterator&) const;
        bool operator>(const iterator&) const;
        bool operator>=(const iterator&) const;
    };
    class const_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;

        const_iterator() noexcept;
        const_iterator(const const_iterator&) noexcept;
        const_iterator(const iterator&) noexcept;
        const_iterator& operator=(const const_iterator&) noexcept;
        const_iterator& operator=(const iterator&) noexcept;
        ~const_iterator();

        const_reference operator*() const;
        const_pointer operator->() const;
        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator--();
        const_iterator operator--(int);
        const_iterator& operator+=(difference_type);
        const_iterator& operator-=(difference_type);
        const_iterator operator+(difference_type) const;
        const_iterator operator-(difference_type) const;
        difference_type operator-(const const_iterator&) const;
        const_reference operator[](difference_type) const;
        bool operator==(const const_iterator&) const;
        bool operator!=(const const_iterator&) const;
        bool operator<(const const_iterator&) const;
        bool operator<=(const const_iterator&) const;
        bool operator>(const const_iterator&) const;
        bool operator>=(const const_iterator&) const;
    };
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        // Iterators
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;
    
};

} // namespace shv

#include "src/list_stubs.hpp"


#endif // GEOMETRIC_LIST_H
