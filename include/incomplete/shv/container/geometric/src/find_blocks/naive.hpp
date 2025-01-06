

#pragma once
#include "base.hpp"

namespace shv{
    namespace impl{
        template<typename NaiveBlockFinderParams>
        struct NaiveBlockFinder
        {
        public:
            using params_type=NaiveBlockFinderParams;
            using mask_index_t=typename params_type::mask_index_t;
            using mask_uint_t=typename params_type::mask_uint_t;

            static constexpr mask_index_t mask_length=base_t::mask_length;
            static constexpr mask_index_t num_blocks=base_t::num_blocks;

            //std::array<mask_index_t,num_blocks> block_lgsizes; //order of blocks. 
            //num_blocks could be used instead as a dynamic popcount select 
            //this could even be compile time to make things insane.
            block_permutation_t<params_type,base_t::num_blocks> block_lgsizes;

            NaiveBlockFinder(const block_permutation_t<typename base_t::mask_type,base_t::num_blocks>& block_lgsizes2):block_lgsizes(block_lgsizes2){}
            
            mask_index_t find_block(mask_uint_t address){
                mask_uint_t cur_offset=0;
                for(mask_index_t i=0;i<base_t::num_blocks;i++){
                    cur_offset+=(static_cast<mask_uint_t>(1) << block_lgsizes[i]);
                    if(address < cur_offset){
                        return i;
                    }
                }
                return base_t::mask_length;
            }
        };
    }
}

