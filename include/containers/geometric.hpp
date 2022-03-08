#ifndef GEOMETRIC_H
#define GEOMETRIC_H

#include<bit>
#include<cstdlib>
#include<cstdint>
#include<algorithm>
#include<new>

namespace shv
{
namespace geometric{
	using handle_t=uint8_t;

	template<
		unsigned int lg_max_blocksize,
		unsigned int lg_min_blocksize>
	struct handle_operations{
		static constexpr unsigned int max_handle_count=std::min(static_cast<unsigned int>(sizeof(size_t)*8),lg_max_blocksize-lg_min_blocksize);
		static constexpr unsigned int lg_max_handle_count=std::min(std::max(std::bit_width(max_handle_count),3U),6U);

		using mask_t=
			std::conditional_t<lg_max_handle_count==3,uint8_t,
			std::conditional_t<lg_max_handle_count==4,uint16_t,
			std::conditional_t<lg_max_handle_count==5,uint32_t,
				uint_least64_t>
		>>;

		static inline void maskset(mask_t& x,handle_t dex,bool condition=true){
			x|=(mask_t(condition) << dex);
		}
		static inline bool masktest(const mask_t& x,handle_t dex){
			return (x>>dex) & 0x1;
		}
		static inline void maskclear(mask_t& x,handle_t dex,bool condition=true){
			x&=~((mask_t(condition) << dex));
		}

		static constexpr size_t handle2size(handle_t h) {
			return size_t(1) << (static_cast<unsigned int>(h)+lg_min_blocksize);
		}
		static constexpr size_t mask2size(const mask_t& m) {
			return static_cast<size_t>(m) << lg_min_blocksize;
		}
		static constexpr mask_t size2mask(const size_t& sz) {
			return static_cast<mask_t>(sz >> lg_min_blocksize);
		}
	};

	template<class handle_ops>
	struct handle_rank_array_bbsearch{	//Todo: is there a different solution for a stack?
	private:
		using mask_t=typename handle_ops::mask_t;
		static constexpr unsigned int lH=handle_ops::lg_max_handle_count;
		static constexpr unsigned int H=handle_ops::max_handle_count;
		static constexpr unsigned int alignment=std::bit_ceil(lH*sizeof(mask_t)); //make sure this thing doesn't cross a cache line.
		alignas(alignment) std::array<mask_t,lH> nB;
	public:
		handle_rank_array_bbsearch(const handle_t* be,const handle_t* ed){
			std::fill_n(nB.begin(),lH,mask_t(0));
			for(uint8_t rank=0;be != ed;rank++)
			{
				handle_t h=*be++;
				for(unsigned int rbit=0;rbit<lH;rbit++)
				{
					handle_ops::maskset(nB[rbit],h,((~rank) >> rbit) & 1);
					//set the corresponding elements of the rank array.  e.g. B[0bXYZ][h] is set iff h is at index ~0bXYZ...  For example.
					/*
					 * arr=[1 4 3 0 2 5 6 7]
					 *       bit position
					 * r  | 7 6 5 4 3 2 1 0
					 * 0  | 0 0 0 1 1 0 1 1
					 * 1  | 0 0 1 1 0 0 1 1
					 * 2  | 0 1 0 0 1 1 1 0
					 */
				}
			}
		}

		handle_t find_handle(size_t index){

		}
		size_t find_offset(handle_t h)
		{

		}
	};

	template<class handle_ops>
	struct handle_rank_array_lsearch; //linear search and build as you go
	template<class handle_ops>
	struct handle_rank_array_lsearch_simd; //linear search and build as you go, but simd.
	template<class handle_ops>
	struct handle_rank_array_bsearch; //binary search
	template<class handle_ops>
	struct handle_rank_array_bsearch_simd; //binary search




		//Todo: is there a different solution for a stack?

}
}

#endif // GEOMETRIC_H
