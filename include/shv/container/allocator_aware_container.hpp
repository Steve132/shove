#pragma once

#include<memory>

namespace shv{
    template<class Allocator>
    class allocator_aware_container: 
        protected Allocator{ //specifically for empty base optimization but maybe this makes the design weird.
    private:
        using alloc_base_type=Allocator;
        using traits=std::allocator_traits<Allocator>;
    protected:
        using Allocator::Allocator;

        constexpr allocator_aware_container(const allocator_aware_container& oa) noexcept(
                noexcept(
                    alloc_base_type(traits::select_on_container_copy_construction(oa))
                )
            ):
            alloc_base_type(traits::select_on_container_copy_construction(oa))
        {}

        //Note: All allocators passed to move constructors MUST be compatible with each other
        //As in rebind (this is part of the constraints of allocator)
        //This means that shallow moves are always possible.
        constexpr allocator_aware_container(allocator_aware_container&& oa) noexcept(
                noexcept(
                    alloc_base_type(std::move(oa))
                )
            ):
            alloc_base_type(std::move(oa))
        {}
private:
        static constexpr bool _always_move_noexcept=traits::propagate_on_container_move_assignment::value || traits::is_always_equal::value;
        static constexpr bool _always_copy_noexcept=traits::propagate_on_container_move_assignment::value || traits::is_always_equal::value;

        //Note: If the move constructor for the value type is noexcept, then the move constructor for the allocator should be noexcept.           
        //this is weird, it seems like if the move constructor for the value can throw, then the move constructor for the allocator can throw.
        //but libstdc++ ignores this and libc++ only cares about the move constructor for the value prior to C++17.
        //hmm destructors are default noexcept so clear is noexcept.
protected:
        template<
            class MoveFunc,
            class DeepMoveFunc,
            class ClearSelfFunc>
        static constexpr void move_assignment(
                            Allocator& self_alloc,
                            Allocator&& other_alloc,
                            MoveFunc&& move_func,
                            DeepMoveFunc&& deep_move_func,
                            ClearSelfFunc&& clear_self_func) noexcept(
                                _always_move_noexcept
                                && noexcept(move_func())
                            )
        {
            if(!traits::is_always_equal::value && self_alloc!=other_alloc)
            {
                if constexpr(!traits::propagate_on_container_move_assignment::value){
                    deep_move_func();
                    return;
                }
                else{
                    clear_self_func();
                }
            }
            if constexpr(traits::propagate_on_container_move_assignment::value){
                self_alloc=std::move(other_alloc);
            }
            move_func();
        }
        /*
            copy:
            same propogate |
            0 0 | partial deep copy from other using self as allocator
            0 1 | (full realloc required)
                    deallocate self using self allocator
                    propogate
                    deep copy from other using self as allocator
            1 0 : partial deep copy from other using self as allocator
                  propogate
            1 1 : partial deep copy from other using self as allocator
            */
        template<
            class CopyFunc,
            class ClearSelfFunc>
        static constexpr void copy_assignment(
                            Allocator& self_alloc,
                            const Allocator& other_alloc,
                            CopyFunc&& copy_func,
                            ClearSelfFunc&& clear_self_func) noexcept(_always_copy_noexcept
                            && noexcept(copy_func())
                            )
        {
            
            if constexpr(traits::propagate_on_container_copy_assignment::value){
                if(!traits::is_always_equal::value && self_alloc!=other_alloc)
                {
                    clear_self_func();
                }
                self_alloc=other_alloc;
            }
            copy_func();
        }
    protected:
        constexpr allocator_aware_container& =(const allocator_aware_container&)=default;
        allocator_aware_container& =(allocator_aware_container&&) noexcept(
			_always_move_noexcept
		)=default;

    public:
        using allocator_type=alloc_base_type;
        allocator_type get_allocator() const noexcept{
            return *this;
        }
    };
}

