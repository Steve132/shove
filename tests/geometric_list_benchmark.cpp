#include<array>
#include<cstdint>
#include<algorithm>
#include<random>
#include<iostream>


static constexpr unsigned int LG_NUM_BLOCKS=3;
static constexpr unsigned int NUM_BLOCKS=1U << LG_NUM_BLOCKS;
using handle_t=uint8_t;
using index_t=uint_fast8_t;

using mask_t=std::uint8_t;
static constexpr unsigned int MASK_SIZE=sizeof(mask_t)*8;

template<class T,unsigned int N>
struct list_t: public std::array<T,N>
{
	using base=std::array<T,N>;
	uint_fast8_t sz;
	list_t():base{} {
		sz=0;
	}
	constexpr auto static_size() const noexcept {
		return N;
	};
	auto static_end() const noexcept {
		return base::begin()+base::size();
	}
	constexpr auto size() const noexcept {
		return sz;
	}
	auto end() const noexcept {
		return base::begin()+size();
	}
	auto end() noexcept {
		return base::begin()+size();
	}
};



using hlist_t=list_t<handle_t,NUM_BLOCKS>;

hlist_t make_list(std::mt19937_64& gen64)
{
	mask_t subset=gen64();
	hlist_t l;
	for(handle_t i=0;i<NUM_BLOCKS;i++) {
		if((subset >> i) & 1){
			l[l.sz++]=i;
		}
	}
	std::shuffle(l.begin(),l.end(),gen64);
	return l;
}

std::vector<mask_t> gen_testcases(size_t N,std::mt19937_64& gen64,bool sorted=false){
	std::vector<mask_t> cases(N);
	std::uniform_int_distribution<> distrib(0, N-1);
	std::generate_n(cases.begin(),N,
					[&](){
						return distrib(gen64);
					}
					);
	if(sorted) std::sort(cases.begin(),cases.end());
	return cases;
}

template<unsigned int N>
using mlist_t=list_t<mask_t,N>;

mlist_t<NUM_BLOCKS> make_compressed_toplist(const hlist_t& h)
{
	mlist_t<NUM_BLOCKS> topdex;
	mask_t c=0;
	for(int i=0;i<h.static_size();i++){
		c+=mask_t(1) << h[i];
		topdex[i]=(i < h.size()) ? c : ~mask_t(0);
	}
	topdex.sz=h.sz;
	return topdex;
}
mlist_t<NUM_BLOCKS> make_ranklist(const hlist_t& h)
{
	mlist_t<NUM_BLOCKS> rankdex;
	for(int i=0;i<h.size();i++){
		rankdex[h[i]]=i;
	}
	rankdex.sz=h.sz;
	return rankdex;
}

struct implementation{
	hlist_t handles;
};

struct ground_truth: public implementation{
	list_t<mask_t,NUM_BLOCKS> topdex;
	list_t<mask_t,NUM_BLOCKS> rankdex;
	ground_truth(const hlist_t& th):
		  topdex{make_compressed_toplist(th)},
		  rankdex{make_ranklist(th)},
		  implementation{th}
	{}
	handle_t find_handle(mask_t m) const{
		mask_t soFar=0;
		for(size_t i=0;i<topdex.size();i++){
			soFar|=mask_t(1) << handles[i];
			if(m < soFar){
				return handles[i];
			}
		}
		return handles[handles.sz-1];
	}
	mask_t find_bottom_so_far(handle_t hs) const{
		mask_t soFar=0;
		for(size_t i=0;i<topdex.size();i++){
			if(hs==handles[i]){
				return soFar;
			}
			soFar|=mask_t(1) << handles[i];

		}
		return topdex[topdex.sz-1];
	}
};


struct binary_search
{
	list_t<mask_t,NUM_BLOCKS> topdex;
	hlist_t h;
	binary_search(const hlist_t& th):topdex(make_compressed_toplist(th)),h{th}
	{}

	//8
	handle_t operator()(mask_t m) const noexcept{
		index_t d=0;    //0b01111111->0b00111111 OR 0b10111111     0b10111111
				//0b10000000  11000000

		d|=(m >= topdex[NUM_BLOCKS/2]) << 2;
		d|=(m >= topdex[d]) << 1;
		d|=(m >= topdex[d]) << 0;

		return h[d];
	}
};
/*
 * 	x=NOT(nB[0]) if a[0] else nB[0]
	if(a[0]):
		result=nB[0]

x=NOT(nB[1]) if a[1] else nB[1]
	result = x & result
	  if(a[1]):
	  result|=nB[1]

	x=NOT(nB[2]) if a[2] else nB[2]
	result = x & result
	  if(a[2]):
	  result|=nB[2]*/
template<class CallableMethod>
bool verify_implementation_find_handle(const ground_truth& gth,CallableMethod&& mth,const std::vector<mask_t>& index)
{
	for(const mask_t& m : index){
		auto gv=gth.find_handle(m);
		auto mv=mth.find_handle(m);
		if(gv != mv) {
			mth.find_handle(m);
			return false;
		}
	}
	return true;
}

int main(int argc,char** argv)
{
	std::mt19937_64 mt(10);
	auto lst=make_list(mt);
	auto tc=gen_testcases(100,mt,false);

	for(auto index : tc){
		std::cout << static_cast<size_t>(index) << std::endl;
	}

	binary_search bs(lst);
	ground_truth gt(lst);

	if(verify_implementation(gt,bs,tc))
	{
		std::cout << "Verified." << std::endl;
	}
	else
	{
		std::cout << "Not verified" << std::endl;
	}


	return 0;
}
