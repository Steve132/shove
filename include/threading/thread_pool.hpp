#ifndef SHV_POOL_HPP
#define SHV_POOL_HPP

#include<thread>
#include<type_traits>
#include<vector>
#include<future>

namespace shv {

template<class ThreadType>
struct thread_pool {
protected:
	static unsigned int default_nthreads(unsigned int n){
		if(n==0) n=std::thread::hardware_concurrency();
		if(n==0) n=1;
		return n;
	}
public:
	std::vector<ThreadType> threads;
	thread_pool()=default;

	template<class Callable,class ...Args>
	thread_pool(unsigned int num_threads,Callable&& c,Args&& ...args) {
		num_threads=default_nthreads(num_threads);
		threads.reserve(num_threads); //reserve capacity but don't actually construct elements.
		for(unsigned int i=0;i<num_threads;i++) {
			threads.emplace_back(std::forward<Callable>(c),std::forward<Args>(args)...);
		}
	}
};



struct worker_pool: protected thread_pool<std::jthread>{
protected:
	//wrap_deferred.
	template<class Callable>
	std::future<
		std::invoke_result_t<Callable>
	> insert_immediately(Callable&& cb)
	{
		std::packaged_task task(std::forward<Callable>(cb));
	}
public:
	template<class Callable>
	std::future<
		std::invoke_result_t<Callable>
	> async(Callable&&)
	{

	}


};

//
//struct taskgraph_pool: protected




}

#endif

int main()
{
	shv::worker_pool p;
	return 0;
}
