#ifndef SHV_STABLE_ITERATOR_HPP
#define SHV_STABLE_ITERATOR_HPP

#include <type_traits>
#include <iterator>

namespace shv
{
template<class Container,class IteratorType>
struct stable_iterator_traits;

template<
    class Container,
    class IteratorType=typename Container::iterator
>
struct stable_iterator{
private:
    static_assert(
        std::is_convertible_v<
            typename std::iterator_traits<IteratorType>::iterator_category,
            std::random_access_iterator_tag>,
        "A stable_iterator can only be constructed from a random_access_iterator"
    );
    using difference_type = std::iterator_traits<IteratorType>::difference_type;
    using iterator=stable_iterator<Container,IteratorType>;

public:
    stable_iterator(Container& t_container_ref,difference_type n):
        m_distance(n),
        m_container_ref{t_container_ref}
    {}
    stable_iterator(Container& t_container_ref,IteratorType it):
    stable_iterator(
        t_container_ref,
        std::distance(
            stable_iterator_traits<Container,IteratorType>::begin(t_container_ref),it)
            )
    {}
    
    iterator & operator++(int) { m_distance++; return *this; }
    iterator & operator++() { m_distance++; return *this; }
    iterator & operator--(int) { m_distance--; return *this; }
    iterator & operator--() { m_distance--; return *this; }
    iterator & operator+=(difference_type n) { m_distance+=n; return *this; }
    iterator & operator-=(difference_type n) { m_distance-=n; return *this; }

    auto operator*() { return *get(); }
    auto operator->() { return &*get(); }
    auto operator*() const { return *get(); }
    auto operator->() const { return &*get(); }

    iterator operator+(difference_type n) const {
        return iterator(m_container_ref,m_distance+n);
    }
    iterator operator-(difference_type n) const {
        return iterator(m_container_ref,m_distance-n);
    }
    iterator operator-(iterator o) const {
        return iterator(m_container_ref,m_distance-o.m_distance);
    }
    auto operator[](difference_type n) const {
        return *(operator+(n));
    }
    std::strong_ordering operator<=>(const iterator& that) const {
        if(auto cmp = m_distance <=> that.m_distance; cmp != 0 ) return cmp;

        auto b1=stable_iterator_traits<Container,IteratorType>::begin(m_container_ref);
        auto b2=stable_iterator_traits<Container,IteratorType>::begin(that.m_container_ref);
        if (auto cmp = b1 <=> b2; cmp != 0)
            return cmp;
    }
    size_t index() const {
        return m_distance;
    }
    //TODO: use enable_if to implement correct traits for contiguous_iterator_tag
protected:
    IteratorType get() const {
        return std::next(
            stable_iterator_traits<Container,IteratorType>
                ::begin(m_container_ref),
            m_distance);
    }
    
    difference_type m_distance;
    Container& m_container_ref;
};

template<class C,class I>
stable_iterator<C,I> operator+(typename std::iterator_traits<I>::difference_type n,
    stable_iterator<C,I> s) {
    return stable_iterator<C,I>(s.m_conainer_ref,s.m_distance+n);
}

template<
    class Container,
    class IteratorType=typename Container::iterator
>
stable_iterator<Container,IteratorType> make_stable_iterator(Container& c,IteratorType it)
{
    return stable_iterator<Container,IteratorType>(c,it);
}


template<class Container>
struct stable_iterator_traits<Container,typename Container::iterator>{
    static typename Container::iterator begin(Container& cr){
        return std::begin(cr);
    }
};
template<class Container>
struct stable_iterator_traits<Container,typename Container::const_iterator>{
    static typename Container::iterator begin(Container& cr){
        return std::cbegin(cr);
    }
};
template<class Container>
struct stable_iterator_traits<Container,typename Container::reverse_iterator>{
    static typename Container::iterator begin(Container& cr){
        return std::rbegin(cr);
    }
};
template<class Container>
struct stable_iterator_traits<Container,typename Container::const_reverse_iterator>{
    static typename Container::iterator begin(Container& cr){
        return std::crbegin(cr);
    }
};
}
