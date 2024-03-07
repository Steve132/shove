#pragma once

#include "../mask.hpp"


namespace shv{
    namespace impl{
        template<typename MaskType,unsigned int NumBlocks>
        struct block_permutation_t: public std::array<typename MaskType::index_t,NumBlocks>{
            using mask_type=MaskType;
            using mask_index_t=typename mask_type::index_t;
            static constexpr mask_index_t mask_length=mask_type::length;
            static constexpr mask_index_t num_blocks=NumBlocks;
        };
        template<typename BlockFinderParams>
        struct block_finder_base{
            using mask_type=typename BlockFinderParams::mask_type;
            using mask_index_t=typename mask_type::index_t;
            using mask_uint_t=typename mask_type::uint_t;
            static constexpr mask_index_t mask_length=mask_type::length;
            static constexpr mask_index_t num_blocks=BlockFinderParams::num_blocks;
        };
    }

}


//actually the best way to do this is to specialize the block finder on the basis of a constructor which sets up for alloc and dealloc.

//simd binary search, simd linear search (when the mask_length is small enough), and galois field permutation are all options.
//simd binary search is probably the best option for large mask_lengths?
//simd linear search is probably the best option for small mask_lengths?
//galois field permutation is probably the best option for medium mask_lengths?
//it also might be possible to speed up this by automatically looking at the implementation where the
//block list permutation is sorted then it would actually fastest to do the bitwise operations etc.
//similarly for accelerated cases where only subparts of the permutation are sorted.
//also, explicit instructions on some architectures for bit permutations
//also bit permutation can be done in 2 instructions with pdep pext
//also bit permutation can be done in bitwise lg2 parallel probably...maybe with unique masks for the type of permutation. 

#endif