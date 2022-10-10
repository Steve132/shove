#ifndef PEEK_ITERATOR_HPP
#define PEEK_ITERATOR_HPP

namespace utok{
    namespace impl{
        template<class It>
        class forward_adapter_impl
        {
        public:
            using iterator_category=std::forward_iterator_tag;
            using value
            protected:
                std::shared_ptr<It> current;
                struct peek_buf{
                    //this should be a hive lol.  Well, specifically it should be my geometric_list.  D'oh.
                    //still, can get halfway decent performance using a deque
                    //oops can't because iterator invalidation (boo)
                    struct chain_node{}
                    static constexpr size_t BLOCK_SIZE=32;
                    std::forward_list<std::array<BLOCK_SIZE>> blocks;
                    It current;
                };

            public:
                
                peek_iterator_impl() = default;
                constexpr explicit peek_iterator_impl(It itr) : current(itr) {}
                template<class U>
                    requires (!std::is_same_v<U, It> && std::convertible_to<const U&, It>)
                constexpr explicit peek_iterator_impl(const U& other) : current(other.base()) {}
            
                constexpr decltype(auto) operator*() const {
                    return *std::prev(current); // <== returns the content of prev
                }
            
                constexpr peek_iterator_impl& operator++() { --current; return *this; }
                constexpr peek_iterator_impl operator++(int) { auto tmp = *this; ++(*this); return tmp; }
            
                constexpr peek_iterator_impl& operator--() { ++current; return *this; }
                constexpr peek_iterator_impl operator--(int) { auto tmp = *this; --(*this); return tmp; }
            
                constexpr It base() const { return current; }
            
                // Other member functions, friend functions, and member typedefs are not shown here.
            };
        }
};

#endif // PEEK_ITERATOR_HPP
