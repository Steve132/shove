#ifndef SHV_ITERATOR_GENERATOR_HPP
#define SHV_ITERATOR_GENERATOR_HPP

namespace shv{



template<class Callable>
concept input_generator_function=requires {
    requires std::is_invocable<Callable>::value;
    requires is_optional<typename std::result_of_t<Callable>>::value;
};

template<input_generator_function Callable,class itag=std::input_iterator_tag>
    requires std::derived_from<itag,std::input_iterator_tag>
class generator_iterator{
    using optional_result_type=typename std::result_of_t<Callable>;
    using result_type=typename is_optional<typename std::result_of_t<Callable>>::type;
    Callable&& f;
protected:
    optional_result_type cur;
    optional_result_type& next() {
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

    generator_iterator(Callable&& tf):f(tf){
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