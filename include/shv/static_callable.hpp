#pragma once

#include<utility>

namespace shv{

template<class F,class FSig=F>
struct static_callable;

template<class F,class R,class... Args>
struct static_callable<F,R(Args...)>
{
	static R with_end_objptr(Args&&... args,void* objptr)
	{
		F& f = *static_cast<F*>(objptr);
		return f(std::forward<Args>(args)...);
	}
};

}