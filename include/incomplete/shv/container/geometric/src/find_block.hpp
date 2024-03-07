#pragma once


#include "mask.hpp"


namespace shv{
    namespace impl{
        struct block_finder_algorithm_tag{
            struct simd_binary_search{};
            struct simd_linear_search{};
            struct galois_field_permutation{};
            struct naive_algorithm{};
            struct sorted_search{};
            struct permute_search_default{};
        };
        template<typename MaskType>
        struct block_finder_algorithm_selector{
            using tag=void;
        };



        
}
}


