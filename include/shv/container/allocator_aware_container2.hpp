#pragma once

#include<memory>

namespace shv{
    template<class Allocator>
    class allocator_aware_container: protected Allocator{
    private:
        using alloc_base_type=Allocator;
        using traits=std::allocator_traits<Allocator>;
    protected:
        template<class OtherAllocator>
        friend class allocator_aware_container<OtherAllocator>;

        template<class OtherAllocator>
        allocator_aware_container(const OtherAllocator& oa) noexcept(
                noexcept(
                    alloc_base_type(std::declval<OtherAllocator>().select_on_container_copy_construction())
                )
            ):
            alloc_base_type(oa.select_on_container_copy_construction())
        {}

        //Note: All allocators passed to move constructors MUST be compatible with each other
        //As in rebind (this is part of the constraints of allocator)
        //This means that shallow moves are always possible.
        template<class OtherAllocator>
        allocator_aware_container(OtherAllocator&& oa) noexcept(
                noexcept(
                    alloc_base_type(std::move(std::declval<OtherAllocator&&>()))
                )
            ):
            alloc_base_type(std::move(oa))
        {}
private:
        template<class OtherAllocator>
        allocator_aware_container& operator=(const OtherAllocator& oa) noexcept(
                noexcept(
                    alloc_base_type(std::declval<OtherAllocator>().select_on_container_copy_construction())
                )
            )
        {
            if constexpr(traits::propagate_on_container_copy_assignment::value){
                alloc_base_type::operator=(oa);
            }
            return *this;
        }
        template<class OtherAllocator>
        allocator_aware_container& operator=(OtherAllocator&& oa) noexcept(
                noexcept(
                    alloc_base_type(std::move(std::declval<OtherAllocator&&>()))
                )
            )
        {
            if constexpr(traits::propagate_on_container_move_assignment::value){
                alloc_base_type::operator=(std::move(oa));
            }
            return *this;
        }
        
        //shallow_move(T&& self,T&& other)
        //deep_move(T&& self,T&& other)
        //deep_copy(T&& self,)

        /*
            ORIGINAL LOGIC
            move:
            same propogate |
            0 0 |   partial deep move using self as allocate, self as clear_self,other as clear other.
            0 1 |   clear self using self allocator, 
                    shallow move
                    propogate
            1 0 :   shallow move or deep move
            1 1 :   shallow move or deep move
                    propogate
        */
        template<
            class ShallowMoveFunc,
            class ClearSelfFunc,
            class DeepMoveFunc,
            class OtherAllocator>
        void move_assignment(ShallowMoveFunc&& sm,
                             ClearSelfFunc&& cs,
                             DeepMoveFunc&& dm,
                             OtherAllocator&& oa) noexcept(
                noexcept(
                    std::declval<ShallowMove&&>()(std::declval<OtherAllocator&&>())
                )
            )
        /*
            copy:
            same propogate |
            0 0 | partial deep copy from other using self as allocator
            0 1 | (full realloc required)
                    deallocate self using self allocator
                    full deep copy from other using other as allocator
                    propogate
            1 0 : partial deep copy from other using self as allocator
            1 1 : partial deep copy from other using self as allocator
                  propogate
            */

    private:
    public:
        using allocator_type=alloc_base_type;
        allocator_type get_allocator() const noexcept{
            return *this;
        }
    };
}


