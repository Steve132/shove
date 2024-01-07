#pragma once

namespace shv{

/*iterators shouldnt be invalidated by push or insert of blocks, so iterators should store the current pointer as well
  as the current blockset pointer and the bit index of the pot they are in and the offset

  iterator+ then simply checks if the offset+arg > pot and if so it does the iterator global offset chase behavior.
*/  


/*

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

    */

}