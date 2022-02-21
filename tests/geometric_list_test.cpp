#include "containers/geometric_list.hpp"

int main()
{
	shv::geometric_list_detail::geometric_bucket_set<int> bs;
	bs.push_front_bucket();
	return 0;
}
