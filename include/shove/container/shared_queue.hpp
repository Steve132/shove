#ifndef SHARED_QUEUE_HPP
#define SHARED_QUEUE_HPP

#include "static_vector.hpp"
#include <memory>

template<class T,size_t block_size=32,class A=std::allocator<T>>
struct sq_node{
    static constexpr size_t BLOCK_SIZE=block_size;
    std::array<T,block_size> data;
    std::shared_ptr<sq_node> next;
    using allocator_type=typename std::allocator_traits<A>::template rebind_alloc<sq_node>;
};

template<class T,size_t block_size=32,class A=std::allocator<T>>
struct sq_iterator{
    using iterator_concept=std::forward_iterator_tag;
    using iterator_category=std::forward_iterator_tag;
    using value_type=T;
    using reference=T&;
    using pointer=T*;
    using difference_type=ssize_t;

protected:
    using node_type=sq_node<T,block_size,A>;
    using head_node_type=static_vector<T,block_size>;
    std::variant<std::shared_ptr<node_type>,head_node_type*> nodeptr;
    ssize_t location;
    void next(){
        location = (location + 1) % block_size;
            if(location==0){
                std::shared_ptr<node_type> tmp=std::move(nodeptr);
                nodeptr.reset(tmp->next);
                if(!node){
                    location=0; //head ptr.
                    return;
                }
            }
            location = -location-1;
        }
        else{
            location = (location + 1)
        }
    }

public:
    sq_iterator& operator++(int){
        next();
        return *this;
    }
    sq_iterator operator++(){
        sq_iterator a=*this;
        next();
        return a;
    }
    reference operator*() const { return nodeptr->data[location]; }
    pointer operator->() { return nodeptr->data; }

    friend bool operator==(const sq_iterator& a, const sq_iterator& b) { return a.nodeptr == b.nodeptr && a.location==b.location; };
};

template<class T,size_t block_size=32,class A=std::allocator<T>>
struct shared_queue
{
public:
    
protected:

    
    

protected:


};



#endif