#pragma once

#include "../mask.hpp"
#include <bit>


namespace shv{
    namespace detail{
        template<unsigned int mask_length>
        struct default_mask_attributes{
            using index_type=uint_fast8_t;
            using uint_type=min_uint_t<mask_length,true>;
            static constexpr index_type length=mask_length;
            //static constexpr index_t index_width=std::bit_width(mask_length-1);
        };

        template<typename MaskAttributes,unsigned int NumBlocks>
        struct block_permutation_t: 
            public std::array<typename MaskAttributes::index_type,NumBlocks>
        {
            using mask_attributes_type=MaskAttributes;
            using index_type=typename MaskAttributes::index_type;
            static constexpr index_type mask_length=MaskAttributes::length;
            static constexpr index_type num_blocks=NumBlocks;
        };

        template<typename BlockFinderParams>
        struct block_finder_base{
            using mask_type=typename BlockFinderParams::mask_type;
            using mask_index_type=typename mask_type::index_type;
            using mask_uint_type=typename mask_type::uint_type;
            static constexpr mask_index_type mask_length=mask_type::length;
        };
    }
}

/*
template<typename MaskAttributes,unsigned int NumBlocks>>
struct tuple_size<block_permutation_t<MaskAttributes,NumBlocks>>
    : std::integral_constant<std::size_t, 
    NumBlocks
>{};

template< std::size_t I, class T >
struct tuple_element< I, const T > {
    using type = typename
        std::add_const<typename std::tuple_element<I, T>::type>::type;
};*/

//actually the best way to do this is to specialize the block finder on the basis of a constructor which sets up for alloc and dealloc.
