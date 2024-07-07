#pragma once

#include<memory>

namespace shv
{
template<class T>
struct alignas(alignof(T)) uninitialized{
private:
    //marked indeterminate?
    alignas(T) std::byte repr_data[sizeof(T)];

    constexpr T* iget(){
        return std::launder(reinterpret_cast<T*>(repr_data));
    }
    constexpr const T* const_iget() const{
        return std::launder(reinterpret_cast<const T*>(repr_data));
    }
public:
    constexpr T* get() noexcept{
        return iget();
    }
    constexpr const T* get() const noexcept{
        return const_iget();
    }
    constexpr operator T&() noexcept{
        return *iget();
    }
    constexpr operator const T&() const noexcept{
        return *const_iget();
    }
    constexpr operator T* &() noexcept{
        return iget();
    }
    constexpr operator const T* &() const noexcept{
        return const_iget();
    }

    /*template<class Alloc,class... Args>
    void construct_alloc(Alloc& alloc,Args&&... args){
        std::allocator_traits<Alloc>::construct(alloc,iget(),std::forward<Args>(args)...);
    }
    template<class Alloc>
    void destroy_alloc(Alloc& alloc){
        std::allocator_traits<Alloc>::destroy(alloc,iget());
    }*/

    uninitialized()=default;
    
    uninitialized(const uninitialized&)=delete;
    uninitialized(uninitialized&&)=delete;
    uninitialized& operator=(const uninitialized&)=delete;
    uninitialized& operator=(uninitialized&&)=delete;

    ~uninitialized()=default;
};
}