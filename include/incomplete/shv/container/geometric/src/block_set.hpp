
#ifndef GEOMETRIC_LIST_BLOCK_SET_HPP
#define GEOMETRIC_LIST_BLOCK_SET_HPP

#include "mask.hpp"
#include <memory>

namespace shv{
    namespace impl{

        template<typename traits_tag>
        struct geometric_block_set_traits;


        template<typename T,typename Allocator,typename TraitsTag=typename std::allocator_traits<Allocator>::value_type>
        class geometric_block_set{ //this should be an allocator-aware container.
        public:
            using allocator_type = Allocator;

            using value_type = typename std::allocator_traits<Allocator>::value_type;
            using size_type = typename std::allocator_traits<Allocator>::size_type;
            using difference_type = typename std::allocator_traits<Allocator>::difference_type;
            using reference = typename std::allocator_traits<Allocator>::reference;
            using const_reference = typename std::allocator_traits<Allocator>::const_reference;
            using pointer = typename std::allocator_traits<Allocator>::pointer;
            using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

            using traits_tag=TraitsTag;
            using traits=geometric_block_set_traits<traits_tag>;

            using mask_type=typename traits::mask_type;

            using mask_index_t=typename mask_type::index_t;
            using mask_uint_t=typename mask_type::uint_t;
            static constexpr mask_index_t mask_length=mask_type::length;
            static constexpr mask_index_t num_blocks=mask_length;

            std::array<std::unique_ptr<value_type>,num_blocks> bases; //pointer to the block with size 2^i.
            mask_type occupied;
        private:
        public:
            //constexpr?
            void allocate_block(mask_index_t i){
                bases[i]=allocator_type::allocate(mask_type::size_from_index(i));
                occupied.set(i);
            }
            void deallocate_block(mask_index_t i){
                allocator_type::deallocate(bases[i],mask_type::size_from_index(i));
                occupied.reset(i);
            }
        };

        template<typename T,typename Allocator,typename TraitsTag=typename std::allocator_traits<Allocator>::value_type>
        class geometric_block_list: public geometric_block_set<T,Allocator,TraitsTag>{ //this should be an allocator-aware container.
            using base_t=geometric_block_set<T,Allocator,TraitsTag>;
            //using mask_index_t=typename base_

            //std::array<mask_index_t,num_blocks> block_permutation; //order of blocks.
            

            //{
                //this is where all the fancy algorithms come into play. 
                //we need to find the block that contains the address.
                //galois field permutation of adddress is an option. 
                //so is binary searching with a simd instruction on the permutation.
                //so is looking at the permutation and doing a linear search with a simd instruction.
                //fastest will depend on size and implementation. 
            //base_t::mask_index_t find_block(base_t::mask_uint_t address);
        };


    }
}



#endif
