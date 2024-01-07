#pragma once


#include "mask.hpp"


namespace shv{
    namespace impl{

        template<typename MaskType,unsigned int NumBlocks>
        struct block_permutation_t: public std::array<typename MaskType::index_t,NumBlocks>{
            using mask_type=MaskType;
            using mask_index_t=typename mask_type::index_t;
            static constexpr mask_index_t mask_length=mask_type::length;
            static constexpr mask_index_t num_blocks=NumBlocks;
        };

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

        template<typename BlockFinderParams>
        struct block_finder_base{
            using mask_type=typename BlockFinderParams::mask_type;
            using mask_index_t=typename mask_type::index_t;
            using mask_uint_t=typename mask_type::uint_t;
            static constexpr mask_index_t mask_length=mask_type::length;
            static constexpr mask_index_t num_blocks=BlockFinderParams::num_blocks;
        };

        
        template<typename BlockFinderParams>
        struct NaiveBlockFinder: public block_finder_base<BlockFinderParams>
        {
        protected:
            using base_t=block_finder_base<BlockFinderParams>;
        public:
            using mask_type=typename base_t::mask_type;
            using mask_index_t=typename base_t::mask_index_t;
            using mask_uint_t=typename base_t::mask_uint_t;

            //std::array<mask_index_t,num_blocks> block_lgsizes; //order of blocks. 
            //num_blocks could be used instead as a dynamic popcount select 
            //this could even be compile time to make things insane.
            block_permutation_t<typename base_t::mask_type,base_t::num_blocks> block_lgsizes;

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

        template<typename MaskType,unsigned int num_layers>
        struct LgTreeBlockFinder
        {
        public:
            using mask_type=MaskType;
            using mask_index_t=typename MaskType::index_t;
            using mask_uint_t=typename MaskType::uint_t;

        protected:
            //static constexpr mask_index_t num_layers=std::bit_width(num_blocks);

            //std::array<mask_index_t,num_blocks> block_lgsizes; //order of blocks. 
            //num_blocks could be used instead as a dynamic popcount select 
            //this could even be compile time to make things insane.
            //block_permutation_t<typename base_t::mask_type,base_t::num_blocks> block_lgsizes;

            std::array<mask_type,num_layers> block_tree_islower; //for each block bit, which direction in the tree does it go.
            mask_type valid_mask; //this would be the highest level.

            template<mask_uint_t higher_layer_index>
            constexpr void find_block_mask_iteration(mask_uint_t& P,mask_uint_t& lookup,std::integral_constant<mask_uint_t,higher_layer_index>) const {
                mask_uint_t C=block_tree_islower[higher_layer_index] & P;
                
                //should this be cmp instead of a mask? e.g. 
                
                constexpr bool use_branchless=true;
                if constexpr (!use_branchless){
                    if(lookup < C){ 
                        lookup-=C; 
                        C=~C;
                    }
                    P&=C;
                }
                else{
                    mask_uint_t T=-mask_uint_t(lookup < C);
                    lookup-=T & C;
                    P = P & (C ^ T);
                }
            }
            
            template<mask_index_t ...Is>
            constexpr mask_uint_t find_block_mask(mask_uint_t lookup) const {
                mask_uint_t P=valid_mask;
                ( find_block_mask_iteration(P,lookup,std::integral_constant<mask_uint_t, num_layers-1-Is>{ } ),... );
                return P;
            }
        public:
            template<unsigned int num_blocks> //throw an error if the number of blocks isn't the same as the number of layers.
            //num_blocks
            constexpr LgTreeBlockFinder(const block_permutation_t<mask_type,num_blocks>& block_lgsizes){
                //construct the tree
                //todo confirm that mask is initialized to 0 which it is so this is fine.
                
                for(mask_index_t i=0;i<num_blocks;i++){
                    mask_index_t cur_block_size=block_lgsizes[i];
                    mask_index_t ni=~i;
                    valid_mask.set(cur_block_size);
                    for(mask_index_t j=0;j<num_layers;j++){
                        mask_uint_t& cur_layer_mask=block_tree_islower[j];
                        cur_layer_mask |= mask_uint_t((ni >> j) & 0x1) << cur_block_size;
                    }
                }
            }
            constexpr mask_index_t find_block(mask_uint_t address) const {
                 //return the index of the block or mask_width if not found.
                 return std::countr_zero(find_block_mask(address));
            }
        };
        /*
            Alternative algorithm:

            I have  x < s0 
                    x < s1
                    x < s2   as the conditions for the block being in the lower half of the tree.
                             literally though I can get the powers of two which are more than the address 
                             as -bit_ceil(x)  then this is the mask of powers of two which are >= the address.  
                             e.g.   00001xxx -> 00001000 -> 00000111 -> 11111000

                             Then I wanna find the first one that is true but ORDERED BY THE PERMUTATION order...
                             Like, find first of x >= s0, x >= s1 etc.     So the idea is

                             cmpmask=-bit_ceil(x)
                             ordered_cmpmask=permute(cmpmask,permutation)
                             permutation_offset=clz(ordered_cmpmask)
                             unpermute_index(permutation_offset)

                             but since I'm actually lookign for the index offset it might be possible to do this even faster.
                             



        */


}
}


