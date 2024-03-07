
#include <shv/container/geometric/list.hpp>
#include <shv/container/geometric/src/block_set.hpp>
#include <shv/container/geometric/src/find_block.hpp>
#include <iostream>

int main()
{
	using mask_type=shv::impl::mask<0,16>;
	shv::impl::block_permutation_t<mask_type,4> perm;
	perm[0]=3;
	perm[1]=0;
	perm[2]=1;
	perm[3]=2;
	shv::impl::LgTreeBlockFinder<mask_type,3> finder(perm);
	std::cout << "Output" << (int)finder.find_block(5);
	return 0;
}
