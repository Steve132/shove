#ifndef GEOMETRIC_LIST_HPP
#define GEOMETRIC_LIST_HPP

#include<cstdint>
#include<array>
#include<cstdlib>
#include<memory>
#include<algorithm>

namespace shv
{

//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0310r0.pdf
//https://en.algorithmica.org/hpc/data-structures/s-tree/
//max num items? probably not.  Probably just use size_t like everything else.
namespace geometric_list_detail{

template<unsigned int N>
using bitwidth_int=
	std::conditional_t<(N <= 8), uint_least8_t,
		std::conditional_t<(N <= 16),uint_least16_t,
		std::conditional_t<(N <= 32),uint_least32_t,uint_least64_t>
		>>;

using handle_t=uint8_t; //a bucket label.
using block_index_t=uint8_t; //an iterator to a block
using exponent_t=uint8_t; //the exponent for the container size. (found by doing min(h+bottom_bucket_size,max_bucket_size)
											//actually it should be
static constexpr unsigned int lg_min_bucket_size=0;
static constexpr unsigned int lg_max_bucket_size=32;
//maybe round this to a power of 4 or 2 or something for the search eventually.
static constexpr unsigned int num_buckets=std::min(static_cast<unsigned int>(sizeof(size_t)*8),lg_max_bucket_size-lg_min_bucket_size);
using mask_t=bitwidth_int<num_buckets>; //a mask can't ever be bigger than num_buckets
static inline void maskset(mask_t& x,handle_t dex){
	x|=(mask_t(1) << dex);
}
static inline bool masktest(const mask_t& x,handle_t dex){
	return (x>>dex) & 0x1;
}
static inline void maskclear(mask_t& x,handle_t dex){
	x&=~((mask_t(1) << dex));
}
//this should really be traits and be like based on something.  But whatever.
struct handle_ops{
	template<handle_t h>
	static constexpr exponent_t handle2exponent(){
		constexpr exponent_t x=h+lg_min_bucket_size;
		return x >= num_buckets ? num_buckets : x;
	}
	static constexpr exponent_t handle2exponent(handle_t h){
		exponent_t x=h+lg_min_bucket_size;
		return x >= num_buckets ? num_buckets : x;
	}
	template<exponent_t x>
	static constexpr size_t exponent2size(){
		return size_t(1) << x;
	}
	static constexpr size_t exponent2size(exponent_t x){
		return size_t(1) << x;
	}
	static constexpr size_t handle2size(handle_t h) {
		return exponent2size(handle2exponent(h));
	}
	static constexpr size_t mask2size(const mask_t& m) {
		return static_cast<size_t>(m << lg_min_bucket_size);
	}
	static constexpr handle_t smallest_handle(const mask_t& m){
		return std::countr_zero(m);
	}
	static constexpr handle_t next_largest_handle(const mask_t& m){
		return std::bit_ceil(m);
	}
};

template<unsigned int N>
static inline uint8_t find_handle_index(mask_t d,const uint8_t* aptr){
	using U=bitwidth_int<N>;
	U dex=(U)d;
	std::array<uint8_t,N> a{};
	for(size_t i=0;i<N;i++) a[i]=aptr[i];

	std::array<U,N> bs{};
	for(size_t i=0;i<N;i++){
		bs[i]+=((U)1) << a[i];
	}

	uint8_t cnt=0;
	for(size_t i=0;i<N;i++){
		cnt +=(dex >= bs[i]);
	}
	return cnt;
}

template<class T, T... inds, class F>
static inline constexpr void loop(std::integer_sequence<T, inds...>, F&& f) {
	(f(std::integral_constant<T, inds>{}), ...);// C++17 fold expression
}

template<class T, T count, class F>
static inline constexpr void loop(F&& f) {
	loop(std::make_integer_sequence<T, count>{}, std::forward<F>(f));
}


static inline uint8_t find_handle_index2(size_t i,mask_t sz,
	const std::array<uint8_t,num_buckets>& a)
{
	uint8_t sel=std::bit_width(std::min(i,size_t(sz)))/8;
	block_index_t out;
	loop<block_index_t,8>([sel,a,&out](auto x){
		if(sel==x){
			out=find_handle_index<x*8>(x*8,a.begin());
		}
	});
	return out;
}

template<class geo_list,bool IsConst>
class geolist_iterator;

//N must be a power of two
template<class T,class Allocator=std::allocator<T>>
class geometric_block_set: protected Allocator{
private:
	static constexpr handle_t H=num_buckets;
protected:
	using allocator_type=Allocator;
	using allocator_traits=std::allocator_traits<allocator_type>;

	using value_type=typename allocator_traits::value_type;
	using size_type=typename allocator_traits::size_type;
	using difference_type=typename allocator_traits::difference_type;
	using reference=value_type&;
	using const_reference=const value_type&;
	using pointer=typename allocator_traits::pointer;
	using const_pointer=typename allocator_traits::const_pointer;

	handle_t head;				//this is just so it's not two lookups to get the head and tail handles. They're still always in block_sequence 0 and the end
	handle_t tail;
	block_index_t block_count; //this is chosen so that head and tail handle are in cache if the bucketset itself is...todo pointers?

	size_t head_gap;	//todo should this be size or mask?  size is correct.  mask allows for cache line behavior.
	size_t tail_gap;

	alignas(64) std::array<handle_t,H> block_sequence; //do you need next[h] and prev[h] to cache how to jump for iterators?  That's the simplest but it doesn't have to be fast.
	std::array<T*,H> lower_static_ptr;
	mask_t allocated_mask;	//mask for the ones which have memory.
	mask_t occupied_mask;	//mask for the ones which have any elements including head and tail.
	std::array<block_index_t,H> block_locations;
	std::array<mask_t,H> block_indices;

	block_index_t identify_bucket(size_t i) const
	{//This search can be faster if you put the half and half subsets of the bits in each to get a less than greater than.
	//e.g. [3 0 1 2] -> [1000 0001 0010 0100] -> 1001  vs 0110  is the first compare.    If its less than it's in one half or the other and you can recurse
	//
		//auto diff=head_ptr-lower_static_pointer[block_sequence[0]];
		block_index_t idx=find_handle_index2(i+head_gap,occupied_mask,block_sequence);
		return idx;
	}

	void push_back_bucket()
	{
		handle_t h=next_available_bucket();
		block_sequence[block_count++]=h;
		tail=h;
		tail_gap=handle_ops::handle2size(h);
		rebuild_locations();
	}
	void push_front_bucket()
	{
		handle_t h=next_available_bucket();

		std::rotate(block_sequence.begin(),block_sequence.begin()+block_count,block_sequence.end()+1);
		block_sequence[0]=h;
		head=h;
		head_gap=handle_ops::handle2size(h);
		block_count++;
		rebuild_locations();
	}
	void pop_front_bucket(){
		erase_bucket(0);
	}
	void pop_back_bucket(){
		erase_bucket(block_count-1);
	}
	void erase_bucket(block_index_t i)
	{
		handle_t h=block_sequence[i];
		if(!masktest(occupied_mask,h))
			return;

		size_t N=handle_ops::handle2size(h);
		pointer bdestroy=block_begin(h);
		pointer edestroy=block_end(h);


		for(pointer b=bdestroy;b != edestroy;++b){
			allocator_traits::destroy(*this,b);
		}

		if(block_count==1)
		{
			head_gap=tail_gap=N/2;
			return;
		}

		maskset(occupied_mask,h);
		std::rotate(block_sequence.begin()+i,block_sequence.begin()+i+1, block_sequence.end()+block_count);

		if(i==0)
		{
			head_gap=0;
		}
		else if(i==(block_count-1))
		{
			tail_gap=0;
		}
		block_count--;

		head=block_sequence[0];
		tail=block_sequence[block_count-1];
		rebuild_locations();
		maybe_dealloc();
	}

	pointer head_ptr() {
		return block_begin(head);
	}
	pointer tail_ptr()  {
		return block_end(tail);
	}

	const_pointer head_ptr() const {
		return block_begin(head);
	}
	const_pointer tail_ptr() const {
		return block_end(tail);
	}

	const_pointer block_begin(handle_t h) const{
		return block_begin_i(h);
	}
	pointer block_begin(handle_t h){
		return block_begin_i(h);
	}
	pointer block_end(handle_t h) const{
		return block_end_i(h);
	}
	pointer block_end(handle_t h){
		return block_end_i(h);
	}

	pointer block_begin_i(handle_t h) const {
		pointer p=lower_static_ptr[h];
		if(h==head) {
			p+=head_gap;
		}
		return p;
	}
	pointer block_end_i(handle_t h) const {
		pointer p=lower_static_ptr[h]+handle_ops::handle2size(h);
		if(h==tail){
			p-=tail_gap;
		}
		return p;
	}
private:
	block_index_t find_handle(handle_t h)
	{
		if(!masktest(occupied_mask,h)) return block_count;
		return block_locations[h];
	}

	void rebuild_locations()
	{
		for(block_index_t i=0;i<block_count;i++){
			block_locations[block_sequence[i]]=i;
		}
	}

	void blank(handle_t h)
	{
		//while(block_count) bucket_deallocate(block_handles[block_count--]);
		occupied_mask=0;
		block_count=0;
		lower_static_ptr={};

		bucket_allocate(h);
		maskset(occupied_mask,h);
	}

	handle_t next_available_bucket(mask_t m=~mask_t(1)) {
		mask_t avail=(~occupied_mask) & m;
		if((avail) == 0) {
			if(m==~mask_t(1)){
				//TODO: out of memory.
			}
			else {
				return 0xFF; //none are available matching the criteria
			}
		}
		mask_t preallocated=(allocated_mask & avail);
		if(preallocated)
		{
			return handle_ops::next_largest_handle(preallocated);
		}
		handle_t h=handle_ops::smallest_handle(avail);
		bucket_allocate(h);
		return h;
	}

	void maybe_dealloc()
	{
		return;  //TODO: we don't want to deallocate in case of a reserve.

		int cnt=0;
		handle_t m1=0,m2=0;
		for(unsigned int i=block_count;i<H;i++)
		{
			handle_t h=block_sequence[i];
			if(masktest(allocated_mask,h))
			{
				if(h > m2) {
					std::swap(h,m2);
					if(m2 > m1) std::swap(m2,m1);
				}
				bucket_deallocate(h);
			}
		}
	}


	void bucket_allocate(handle_t h)
	{
		auto N=handle_ops::handle2size(h);
		if(!masktest(allocated_mask,h)) {
			lower_static_ptr[h]=allocator_traits::allocate(*this,N);
		}
		maskset(allocated_mask,h);
	}

	void bucket_deallocate(handle_t h)
	{
		auto N=handle_ops::handle2size(h);
		if(masktest(allocated_mask,h)) {
			allocator_traits::deallocate(*this,lower_static_ptr[h],N);
		}
		maskclear(allocated_mask,h);
	}

protected: //LIST INTERFACE
	geometric_block_set()
	{
		allocated_mask=0;
		blank(0);
	}
	size_t size() const{
		return static_cast<size_t>(handle_ops::mask2size(occupied_mask))
			   -static_cast<size_t>(head_gap)
			   -static_cast<size_t>(tail_gap);
	}

	template<class ...Args>
	void emplace_back(Args&& ...args)
	{
		if(tail_gap == 0)
		{
			push_back_bucket();
		}
		tail_gap--;
		allocator_type::construct(*this,tail_ptr(),std::forward<Args>(args)...);
	}
	void push_back(const T& t)
	{
		emplace_back(t);
	}
	template<class ...Args>
	void emplace_front(Args&& ...args)
	{
		if(head_gap == 0)
		{
			push_front_bucket();
		}
		head_gap--;
		allocator_type::construct(*this,head_ptr(),std::forward<Args>(args)...);
	}
	void push_front(const T& t)
	{
		emplace_front(t);
	}
	size_type max_size() const noexcept
	{
		return size_t(1) << (num_buckets+lg_max_bucket_size+lg_min_bucket_size);
	}
	void reserve(size_t s)
	{
		while(handle_ops::mask2size(allocated_mask) < s){
			next_available_bucket();
		}
	}
	void resize(size_type s)
	{
		reserve(s);
		//Todo just copy using the iterator stuff.
	}
	void resize( size_type s, const value_type& value )
	{
		reserve(s);
	}
};

template<class geolist,class pointer_type>
struct iterator {
public:
	using value_type = typename geolist::value_type;
	using reference = typename geolist::reference;
	using pointer = typename geolist::pointer_type;
	using iterator_category = std::random_access_iterator_tag;
	using iterator_concept = std::random_access_iterator_tag;
	using difference_type = typename geolist::difference_type;
protected:
	const geolist* parent;
	pointer* ptr;
	handle_t h;
	//Todo this could also parent->static_index(h); which is a lookup into a table.  Same table
	//for binary search for O(1) implementation potentially. (but worse for cache)

	constexpr iterator(const geolist* tp, pointer tptr,handle_t th):
		  parent{tp},ptr{ tptr },h{th}
	{}

public:
	constexpr iterator() noexcept =default;
	constexpr iterator(const iterator& it) noexcept =default;
	constexpr iterator(iterator&& it) noexcept =default;

	constexpr bool operator==(const iterator& other) const noexcept{ return ptr == other.ptr; }
	constexpr bool operator!=(const iterator& other) const noexcept{ return ptr != other.ptr; }

	constexpr reference operator*() const noexcept{ return *ptr; }
	constexpr pointer operator->() const noexcept { return ptr; }

	constexpr iterator& operator++() noexcept {
		++ptr;
		if(ptr == parent->block_end(h))
		{
			if(h==parent->tail){
				return *this;
			}
			h=parent->find_handle(h)+1;
			ptr=parent->block_begin(h);
		}
		return *this;
	}
	constexpr iterator operator++(int) noexcept { iterator tmp(*this); ++(*this); return tmp; }
	constexpr iterator& operator--() noexcept {
		if(ptr == parent->block_begin(h))
		{
			if(h==parent->head){
				return *this;
			}
			h=parent->find_handle(h)-1;
			ptr=parent->block_end(h);
		}
		--ptr;
		return *this;
	}
	constexpr iterator operator--(int) noexcept { iterator tmp(*this); ++(*this); return tmp; }


	constexpr iterator& operator+=(const difference_type other) noexcept
	{
		pointer be=parent->block_end(h);
		pointer np=ptr+other;
		if(np < be){
			ptr=np;
		}
		else{

		}
		return *this;
	}
	constexpr iterator operator+(const difference_type other) const noexcept
	{ iterator tmp(*this); tmp+=other; return tmp; }

/*
	constexpr iterator& operator-=(const difference_type other) noexcept { m_iterator -= other; return *this; }
	constexpr iterator operator-(const difference_type other) const noexcept { return iterator(m_iterator - other); }
	constexpr difference_type operator-(const iterator& other) const noexcept { return std::distance(m_iterator, other.m_iterator); }
	constexpr reference operator[](std::size_t index) const { return m_iterator[index]; }





	constexpr bool operator<(const iterator& other) const noexcept { return m_iterator < other.m_iterator; }
	constexpr bool operator>(const iterator& other) const noexcept { return m_iterator > other.m_iterator; }
	constexpr bool operator<=(const iterator& other) const noexcept { return m_iterator <= other.m_iterator; }
	constexpr bool operator>=(const iterator& other) const noexcept { return m_iterator >= other.m_iterator; }
*/
};


}



template<class T,class Allocator=std::allocator<T>
		 >
class geometric_list: protected geometric_list_detail::geometric_block_set<T,Allocator>
{
public:
	using allocator_type=Allocator;
	using allocator_traits=std::allocator_traits<allocator_type>;

	using value_type=typename allocator_traits::value_type;
	using size_type=typename allocator_traits::size_type;
	using difference_type=typename allocator_traits::difference_type;
	using reference=value_type&;
	using const_reference=const value_type&;
	using pointer=typename allocator_traits::pointer;
	using const_pointer=typename allocator_traits::const_pointer;

};
}


#endif // GEOMETRIC_LIST_HPP
