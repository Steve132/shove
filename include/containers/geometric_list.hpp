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


//N must be a power of two
template<class T,class Allocator=std::allocator<T>>
class geometric_bucket_set: protected Allocator{
private:
	static constexpr handle_t H=num_buckets;
public:
	using allocator_type=Allocator;
	using allocator_traits=std::allocator_traits<allocator_type>;
	using pointer_type=T*;

	pointer_type head_ptr;
	pointer_type tail_ptr;
	alignas(64) std::array<handle_t,H> block_sequence;
	block_index_t block_count; //this is chosen so that head and tail handle are in cache if the bucketset itself is...todo pointers?

	std::array<T*,H> lower_static_ptr;
	mask_t allocated_mask;	//mask for the ones which have memory.
	mask_t occupied_mask;	//mask for the ones which have any elements including head and tail.

	geometric_bucket_set()
	{
		allocated_mask=0;
		blank(0);
	}

	void push_back_bucket() //TODO: you could use a size hint here to more intelligently push back the buckets.
	{
		handle_t h=next_available_bucket();

		maskset(occupied_mask,h);
	}
	void push_front_bucket()
	{
		handle_t h=next_available_bucket();

		maskset(occupied_mask,h);
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
		pointer_type bdestroy=lower_static_ptr[h];
		pointer_type edestroy=lower_static_ptr[h]+handle_ops::handle2size(h);

		if(i==0){
			bdestroy=head_ptr;
		}
		if(i==block_count-1){
			edestroy=tail_ptr;
		}
		for(pointer_type b=bdestroy;b != edestroy;++b){
			allocator_traits::destroy(*this,b);
		}

		if(block_count==1)
		{
			head_ptr=tail_ptr=lower_static_ptr[h]+N/2;
			return;
		}
		maskset(occupied_mask,h);
		std::rotate(block_sequence.begin()+i,block_sequence.begin()+i+1, block_sequence.end()+block_count);

		maybe_dealloc();
	}
private:

	void blank(handle_t h)
	{
		//while(block_count) bucket_deallocate(block_handles[block_count--]);
		occupied_mask=0;
		block_count=0;
		lower_static_ptr={};

		bucket_allocate(h);
		maskset(occupied_mask,h);
	}

	handle_t next_available_bucket() {
		mask_t avail=~occupied_mask;
		if(avail == 0) {
			//TODO: out of memory.
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




};

}


/*
template<class T,class Allocator=std::allocator<T>
		 >
class geometric_list: protected Allocator
{
public:
	using allocator_type=Allocator;

private:
	enum class direction_t{
		FORWARD,BACKWARD
	};

	struct nodeblock_t;
	struct node_type{
		block_type block;
		nodeblock_t* node_block;	//optimization this should be a smaller offset so node_type is faster.
	};

		using node_handle_t=int8_t;
	//first and last are always locations 0,1
	//all the others in between are at the end in arbitrary locations.

		template<unsigned int SZ>
		struct nodeblock_t: public
							 std::array<node_type_t,SZ> nodes
	{
		std::array<size_t,SZ> index_lower;
		std::array<size_t,SZ> index_upper;
		std::array<uintptr_t,SZ> pointer_lower;
		std::array<uintptr_t,SZ> pointer_upper;
		std::array<int8_t,MAX_NODES> handles;
		//number of items in the set (always at least two.  0 is current front, 1 is current back,

	};
	//rotate from two past the end
	void push_back(const T& a,const T& init=T{}){
		node_type_t& end=blocks[last_block_id];
		if(end.size()==end.capacity()){
			std::move();
		}
	}
	size_t _size;
public:
	using value_type=T;
	using difference_type=std::vector<T,allocator_type>::size_type;
	using reference=value_type&;
	using const_reference=const value_type&;
	using pointer=std::allocator_traits<allocator_type>::pointer;
	using const_pointer=std::allocator_traits<allocator_type>::const_pointer;

	struct iterator {
	private:
		block_collection_type::iterator bit;
		block_type::iterator it;

	public:
		using value_type = Type;
		using reference = value_type&;
		using pointer = value_type*;
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		iterator& operator++(){
			it++;
			if(it==bit->block.end()){
				bit++;
			}
		}
		//using iterator_concept = std::contiguous_iterator_tag;

		constexpr iterator(Type* iter = nullptr) : m_iterator{ iter } {}

		constexpr bool operator==(const iterator& other) const noexcept { return m_iterator == other.m_iterator; }
		constexpr bool operator!=(const iterator& other) const noexcept { return m_iterator != other.m_iterator; }
		constexpr reference operator*() const noexcept { return *m_iterator; }
		constexpr pointer operator->() const noexcept { return m_iterator; }
		constexpr iterator& operator++() noexcept { ++m_iterator; return *this; }
		constexpr iterator operator++(int) noexcept { iterator tmp(*this); ++(*this); return tmp; }
		constexpr iterator& operator--() noexcept { --m_iterator; return *this; }
		constexpr iterator operator--(int) noexcept { iterator tmp(*this); --(*this); return tmp; }
		constexpr iterator& operator+=(const difference_type other) noexcept { m_iterator += other; return *this; }
		constexpr iterator& operator-=(const difference_type other) noexcept { m_iterator -= other; return *this; }
		constexpr iterator operator+(const difference_type other) const noexcept { return iterator(m_iterator + other); }
		constexpr iterator operator-(const difference_type other) const noexcept { return iterator(m_iterator - other); }
		constexpr iterator operator+(const iterator& other) const noexcept { return iterator(*this + other.m_iterator); }
		constexpr difference_type operator-(const iterator& other) const noexcept { return std::distance(m_iterator, other.m_iterator); }
		constexpr reference operator[](std::size_t index) const { return m_iterator[index]; }
		constexpr bool operator<(const iterator& other) const noexcept { return m_iterator < other.m_iterator; }
		constexpr bool operator>(const iterator& other) const noexcept { return m_iterator > other.m_iterator; }
		constexpr bool operator<=(const iterator& other) const noexcept { return m_iterator <= other.m_iterator; }
		constexpr bool operator>=(const iterator& other) const noexcept { return m_iterator >= other.m_iterator; }
	};
};*/

}


#endif // GEOMETRIC_LIST_HPP
