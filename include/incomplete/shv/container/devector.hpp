#pragma once

#include "uninitialized.hpp"

namespace shv{
	
template<class T,class Allocator>
class devector:
	public std::vector<shv::uninitialized,Allocator>
	>
{
public:

};
}
