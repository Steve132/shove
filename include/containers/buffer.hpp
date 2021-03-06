#ifndef SHV_BUFFER_HPP
#define SHV_BUFFER_HPP

#include "allocator_aware_container.hpp"

namespace shv
{
template<class T,class Allocator=std::allocator<T>>
class buffer: protected allocator_aware_container<Allocator>
{
private:
	using atraits=std::allocator_traits<Allocator>;
	using aac=allocator_aware_container<Allocator>;
	using aa_behaviors=allocator_aware_behaviors<Allocator>;



public:
	using value_type=T;
	using allocator_type=Allocator;
	using size_type=std::size_t;
	using difference_type=std::ptrdiff_t;
	using reference=value_type&;
	using const_reference=const value_type&;
	using pointer=typename atraits::pointer;
	using const_pointer=typename atraits::const_pointer;

	using iterator=typename atraits::pointer;
	using const_iterator=typename atraits::const_pointer;
	using reverse_iterator=std::reverse_iterator<iterator>;
	using const_reverse_iterator=std::reverse_iterator<const_iterator>;
public:

	constexpr buffer() noexcept(noexcept(Allocator()))
	{}

	constexpr explicit buffer( const Allocator& alloc ) noexcept:
		aac(alloc)
	{}

	constexpr buffer( size_type count,
					 const T& value,
					 const Allocator& alloc = Allocator()):
		aac(alloc)
	{
		assign(count,value);
	}

	constexpr explicit buffer( size_type count,
							  const Allocator& alloc = Allocator() ):
		buffer(count,alloc)
	{}

	template< class InputIt >
	constexpr buffer( InputIt first, InputIt last,
					 const Allocator& alloc = Allocator() ):
		  aac(alloc)
	{
		assign(first,last);
	}

	constexpr buffer( const buffer& other ):
		  aac(aa_behaviors::select_on_copy_constructor(other.get_allocator()))
	{}
	constexpr buffer( const buffer& other, const Allocator& alloc ):
		aac(alloc)
	{}

	constexpr buffer( buffer&& other ) noexcept
	{}
	constexpr buffer( buffer&& other, const Allocator& alloc );

	constexpr buffer( std::initializer_list<T> init,
					 const Allocator& alloc = Allocator() );

	constexpr void assign( size_type count, const T& value );

	template< class InputIt >
	constexpr void assign( InputIt first, InputIt last );

	constexpr void assign( std::initializer_list<T> ilist );

	constexpr buffer& operator=( const buffer& other );

	constexpr buffer& operator=( buffer&& other ) noexcept(
		aa_behaviors::nothrow_move_assignment
	);

	constexpr buffer& operator=( std::initializer_list<T> ilist );
	//constexpr
	~buffer()  //TODO C++20
	{}
	using aac::get_allocator;
};
}

#endif
