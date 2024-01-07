#ifndef SHV_ITERATOR_GENERATOR_HPP
#define SHV_ITERATOR_GENERATOR_HPP

#include "../meta/optional.hpp"
#include <iterator>
#include <ranges>

namespace shv{

namespace concepts{

template<class Callable>
concept input_generator_function=requires {
    requires std::is_invocable<Callable>::value;
};

}

//https://brevzin.github.io/c++/2021/11/21/conditional-members/

//if the callable is move only then it's an input generator iterator.
//if the callable is copyable then it's a forward generator iterator.
//if the callable can be invoked with T then it's an output generator iterator. 
//if the callable is copyable and can be invoked with T than overload the copy operators.

//the expression generator_iterator() with no template args always creates a sentinel.

template<class C=void>
class generator_iterator{};

template<class Callable>
    requires std::is_invocable_v<Callable>
class generator_iterator<Callable>{
protected:
    Callable f;

//if 
   // static constexpr //option callable
    
    std::optional<value_type> cur;

    void next() requires std::invoke_result_t<Callable>
    {
        cur=f();
        return cur;
    }
public:
    using difference_type=std::ptrdiff_t;
    using value_type=result_type;
    using pointer=value_type*;
    using reference=value_type&;
    using iterator_category=std::input_iterator_tag; //could technically be input, output, or forward.
    using iterator_concept=std::input_iterator_tag;

    generator_iterator(Callable&& tf):f(std::forward<Callable>(tf)){
        next();
    }
    generator_iterator()=default; 

    bool operator==(const generator_iterator& o) const{
        if(!cur) {
            return !o.cur;
        } else if(!o.cur) {
            return false;
        }
        else{
            return *cur == *o.cur;
        }
    }
    bool operator==(const std::default_sentinel_t& o) const{
        return !cur;
    }
    template<class T>
    bool operator!=(T&& t) const { return !(operator==(std::forward(t)));}

    reference operator*() const { return *cur; }
    pointer operator->() { return &*cur; }
    generator_iterator& operator++() { next(); return *this; }  
    generator_iterator operator++(int) { generator_iterator tmp = *this; next(); return tmp; }
};



template<class Callable>
auto generator_range(Callable&& c){
    return std::ranges::subrange((generator_iterator<Callable>(c)),generator_iterator<Callable>());
}

}

#endif