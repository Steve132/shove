#ifndef GEOMETRIC_LIST_HPP
#define GEOMETRIC_LIST_HPP

#include<cstdint>
#include<array>
#include<cstdlib>
#include<memory>
#include<algorithm>
#include<bitset>

namespace shv
{

//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0310r0.pdf
//max num items? probably not.  Probably just use size_t like everything else.
namespace detail{

using handle_t=uint8_t; //a bucket index.
using exponent_t=uint8_t; //the exponent for the container size. (found by doing min(h+bottom_bucket_size,max_bucket_size)
											//actually it should be
static constexpr unsigned int lg_min_bucket_size=0;
static constexpr unsigned int lg_max_bucket_size=32;
//maybe round this to a power of 4 or 2 or something for the search eventually.
static constexpr unsigned int num_buckets=std::min(static_cast<unsigned int>(sizeof(size_t)*8),lg_max_bucket_size-lg_min_bucket_size);
using mask_t=std::bitset<num_buckets>; //a mask can't ever be bigger than num_buckets

//this should really be traits and be like based on something.  But whatever.
struct bucketset_size_params{
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
		return static_cast<size_t>(m.to_ullong() << lg_min_bucket_size);
	}
	static constexpr handle_t smallest_handle(const mask_t& m){
		return std::countr_zero(m.to_ullong());
	}
	static constexpr handle_t largest_handle(const mask_t& m){
		return std::bit_ceil(m.to_ullong());
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

	handle_t head,tail; //this is chosen so that head and tail handle are in cache if the bucketset itself is.

	pointer_type head_ptr;
	pointer_type tail_ptr;

	//these arrays might be able to be allocated on the heap to save stack space.  Theyre only needed in edge cases..or could be small size versions for low occupancy.
	//or whatever.
	//ooh you could do compaction like an indirection buffer. (out of cache?) hmm benchmarking is needed.
	std::array<T*,H> lower_ptr;
	std::array<size_t,H> lower_static_index; //this is the total index space counting from the front...but the head changes the resulting index too.
							//does it make the most sense for this to be signed? then the end of the head is zero and you don't have to always reindex.
							//answer: no.  Its always from the start of the empty space of the head block.  Then the head size can be subtracted.

	std::array<handle_t,H> prev,next;

	mask_t allocated_mask;	//mask for the ones which have memory.
	mask_t full_mask;		//mask for the ones which are full and not head and not tail (because of binary, this is also the full count inside)
							//(no...it's gotta be << min_block first))
	mask_t occupied_mask;	//mask for the ones which have any elements including head and tail.

	//size_t tail_size;		//faster counter for sum of tail and head
	//size_t head_size;		//this can be used instead of (tail_ptr-lower_pointer[tail])+(lower_pointer[head]-head_ptr)
							//however this data structure can be made atomic pretty easily with compare and swap and if so then this needs to go back to the old version
	geometric_bucket_set()
	{
		blank();
	}
	void blank(){
		allocated_mask=0;
		occupied_mask=0;
		full_mask=0;
		head=0;
		tail=0; //pretty cool, all zeros is a valid state.
		lower_ptr={};
		lower_static_index={};
		prev={};
		next={};
	}

	handle_t next_available_bucket() {
		if(occupied_mask.none()) {
			return 0;
		}
		mask_t avail=~occupied_mask;
		if(avail == 0) {
			//TODO: out of memory.
		}
		mask_t preallocated=(allocated_mask & avail);
		if(preallocated.any())
		{
			return bucketset_size_params::smallest_handle(preallocated);
		}
		handle_t h=bucketset_size_params::smallest_handle(avail);
		bucket_allocate(h);
		return h;
	}

	void push_back_bucket() //TODO: you could use a size hint here to more intelligently push back the buckets.
	{
		handle_t h=next_available_bucket();
		next[tail]=h;
		prev[h]=tail;

		if(occupied_mask[tail])
		{
			//warning this should NOT happen if tail isn't full.
			full_mask[tail]=true;
			size_t oldsize=bucketset_size_params::handle2size(tail);
			lower_static_index[h]=lower_static_index[tail]+oldsize;
		}
		next[h]=h; //it's the tail now.
		tail_ptr=lower_ptr[h];
		tail=h;
	}
	void push_front_bucket()
	{
		handle_t h=next_available_bucket();
		prev[head]=h;
		next[h]=head;

		//warning this should NOT happen if head isn't full.

		//TODO: is this faster if you pointer walk it or iterate with a mask or iterate over all but filter?
		//godbolt says the roll filter can vectorize

		size_t newsize=bucketset_size_params::handle2size(h);
		if(occupied_mask[head]) //if the current head is occupied
		{
			full_mask[head]=true;
			for(unsigned int i=0;i<num_buckets;i++)
			{
				if(full_mask[i]) lower_static_index[i]+=newsize;
			}
		}

		lower_static_index[h]=0;

		prev[h]=h;
		head_ptr=lower_ptr[h]+newsize;
		head=h;
	}

	void bucket_allocate(handle_t h)
	{
		auto N=bucketset_size_params::handle2size(h);
		if(!allocated_mask[h]) {
			lower_ptr[h]=allocator_traits::allocate(*this,N);
		}
		allocated_mask[h]=true;
	}

	void bucket_deallocate(handle_t h)
	{
		auto N=bucketset_size_params::handle2size(h);
		if(allocated_mask[h]) {
			allocator_traits::deallocate(*this,lower_ptr[h],N);
		}
		allocated_mask[h]=false;
	}

	void bucket_destroy(handle_t h)
	{
		size_t N=bucketset_size_params::handle2size(h);
		if(h==tail){

		}
		full_mask[h]=false;
		occupied_mask[h]=false;
		bucket_deallocate(h);

	}

	size_t full_size() const{
		return bucketset_size_params::mask2size(full_mask);
	}
	size_t tail_size() const{
		return tail_ptr-lower_ptr[tail];
	}
	size_t head_size() const{
		size_t N=bucketset_size_params::handle2size(head);
		return (lower_ptr[head]+N)-head_ptr; //should this be cached into a variable?
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
