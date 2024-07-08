#pragma once

#include<memory>

namespace shv
{
template<class T>
struct alignas(alignof(T)) uninitialized{
private:
    //marked indeterminate?
    //alignas(T) std::byte repr_data[sizeof(T)];

    constexpr T* iget(){
        return std::launder(reinterpret_cast<T*>(this));
    }
    constexpr const T* const_iget() const{
        return std::launder(reinterpret_cast<const T*>(this));
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

   template<class Other>
   constexpr uninitialized& operator=(const Other& other) noexcept {};
   template<class Other>
   constexpr uninitialized& operator=(Other&& other) noexcept {};
   template<class Other>
   constexpr uninitialized(const Other& other) noexcept {};
   template<class Other>
   constexpr uninitialized(Other&& other) noexcept {};
};

template<class Allocator>
struct uninitialized_allocator_adaptor:
    public Allocator
{
    using allocator_type = Allocator;
    using value_type = typename Allocator::value_type;
    using pointer = typename Allocator::pointer;
    using size_type = typename Allocator::size_type;

    uninitialized_allocator_adaptor() = default;
    uninitialized_allocator_adaptor(const Allocator& a) : Allocator(a) {}

    template<class U>
    struct rebind
    {
        using other = uninitialized_allocator_adaptor<typename Allocator::template rebind<U>::other>;
    };

    pointer allocate(size_type n)
    {
        return Allocator::allocate(n);
    }

    void deallocate(pointer p, size_type n)
    {
        Allocator::deallocate(p, n);
    }

    template<class U, class... Args>
    void base_construct(U* p, Args&&... args){
        Allocator::construct(p, std::forward<Args>(args)...);
    }

    template<class U>
    void base_destroy(U* p){
        Allocator::destroy(p);
    }
    template<class U, class... Args>
    constexpr void construct(U* p, Args&&... args) noexcept {}

    template<class U>
    constexpr void destroy(U* p) noexcept {}

};


}