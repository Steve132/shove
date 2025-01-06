
#include <shv/container/geometric/list.hpp>
#include <shv/container/geometric/src/block_set.hpp>
#include <shv/container/geometric/src/find_block.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <iomanip>
template<unsigned int sz,unsigned num_blocks>
struct perm_config{
	struct Permutation: public shv::detail::block_permutation_t<
			shv::detail::default_mask_attributes<sz>,
			num_blocks>
	{
		using base_type=shv::detail::block_permutation_t<
				shv::detail::default_mask_attributes<sz>,
				num_blocks>;
		using index_type=typename base_type::index_type;

	private:
		auto get(const index_type& i) const noexcept{
			return base_type::operator[](i);
		}
	public:

		template<typename URBG>
		Permutation(URBG&& g){
			using index_type=typename base_type::index_type;

			std::array<index_type,sz> possible;
			index_type ind=0;

			for(auto& p : possible){
				p=ind++;
			}

			std::shuffle(possible.begin(),possible.end(),std::forward<URBG>(g));

			for(size_t i=0;i<num_blocks;i++){
				base_type::operator[](i)=possible[i];
			}
		}

		friend std::ostream& operator<<(std::ostream& out,const Permutation& p){
			out << "(";
			bool first=true;
			for(const auto& pi : p){
				if(!first) out << ",";
				first=false;
				out << std::setw(2) << (unsigned int)pi;
			}
			out << ")";
			return out;
		}
		size_t total_size() const noexcept{
			size_t tsz=0;
			for(size_t i=0;i<num_blocks;i++){
				tsz+=1ULL << base_type::operator[](i);
			}
			return tsz;
		}
		template<class IndexFunc>
		size_t find_failures_blocksize_finder_single(IndexFunc&& blocksize_finder) const {
			size_t total_sz=total_size();

			size_t prev_blocksum=0;
			size_t current_perm_index=0;
			size_t num_failures=0;
			
			for(size_t addr=0;addr<total_sz;addr++){
				size_t block_size_test=blocksize_finder(addr);
				size_t cur_blocksel=get(current_perm_index);
				size_t blocksum=1ULL << cur_blocksel;
				if(addr>=prev_blocksum+blocksum){
					prev_blocksum+=blocksum;
					current_perm_index++;
				}
				if(block_size_test!=blocksum){
					std::cerr << "Error at address " << addr << " expected block size " << blocksum << " got " << block_size_test << std::endl;
					std::cerr << "Current block index " << current_perm_index << " current block size " << blocksum << std::endl;
					std::cerr << "Permutation " << *this << std::endl;
					std::cerr << "Total size " << total_sz << std::endl;
					std::cerr << "Blocksum " << blocksum << std::endl;
					std::cerr << "Prev blocksum " << prev_blocksum << std::endl;
					std::cerr << "Current block index " << current_perm_index << std::endl;
					std::cerr << "Current block size " << blocksum << std::endl;
					std::cerr << "Current block selection " << cur_blocksel << std::endl;
					num_failures++;
				}
			}
			return num_failures;
		}
	};

	static void run_test(size_t N){
		std::default_random_engine re(0); //(std::random_device{}());
		constexpr unsigned int num_selected=sz;

		auto single_test=[&re](){
			Permutation perm(re);
			std::cout << perm << std::endl;

			perm.find_failures_blocksize_finder_single([&perm](size_t addr){
				return perm.find_block(addr);
			});
		};

		for(size_t i=0;i<N;i++){
			single_test();
		}
	}



};

int main()
{
	perm_config<10,4>::run_test(8);
	return 0;
}
