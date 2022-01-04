#ifndef SHV_ALLOCATOR_ADAPTOR_HPP
#define SHV_ALLOCATOR_ADAPTOR_HPP

#include<memory>



//Much thanks from https://memory.foonathan.net/md_doc_internal_usage.html to explain this.
//https://stackoverflow.com/questions/27471053/example-usage-of-propagate-on-container-move-assignment

//According to libCXX the copy constructor is a copy not a move.

//And according to gcc we can use is always same OR the is copy to copy.

template<class Allocator,
	template<class ...HostTypes> class HostStruct,
	class ...HTypes
>
class allocator_helper:Allocator
{
private:
	using alloc_type=Allocator;
	using alloc_traits=std::allocator_traits<Allocator>;
public:
	HostStruct<Allocator,HTypes...> host;

	template<class... ConstructorArgs>
	explicit allocator_helper(const Allocator& alloc=Allocator{},ConstructorArgs&& ...hargs) noexcept:
		alloc_type(alloc),
		host(*this,hargs...)
	{}

	template<class Allocator2,
		template<class ...HostTypes2> class HostStruct2,
		class ...HTypes2
	>
	allocator_helper(const allocator_helper<Allocator2,HostStruct2,HTypes2...>& other):
		alloc_type(alloc_traits::select_on_container_copy_construction(other)),
		host(*this,other.host)
	{}

	allocator_helper(allocator_helper&& other):
		alloc_type(std::move(other)),
		host(*this,std::move(other.host))
	{}

	template<class Allocator2,
		template<class ...HostTypes2> class HostStruct2,
		class ...HTypes2
	>
	allocator_helper& operator=(const allocator_helper<Allocator2,HostStruct2,HTypes2...>& other)
	{
		if constexpr (alloc_traits::propagate_on_container_copy_assignment::value
			&& !(alloc_traits::is_always_equal::value)
			)
		{
			if(alloc_type::operator!=(other))
			{
				alloc_type alloc(other);
				host.copy(*this,alloc,other.host); //old,new
				alloc_type::operator=(std::move(alloc));
				return *this;
			}
		}
		host.copy(*this,*this,other.host);
		return *this;
	}

	allocator_helper& operator=(allocator_helper&& other)
		noexcept(alloc_traits::propagate_on_container_move_assignment::value)
	{
		if constexpr (alloc_traits::is_always_equal::value)
		{
			host.move(*this,other.host);
			return *this;
		}
		else if constexpr (alloc_traits::propagate_on_container_move_assignment::value)
		{
			if(alloc_type::operator!=(other))
			{
				host.copy(*this,other,other.host);
				alloc_type::operator=(std::move(other));
				return *this;
			}
		}
		host.move(*this,other.host);

		return *this;
	}

	friend void swap(allocator_helper &a, allocator_helper &b) noexcept
	{
		using std::swap;
		if constexpr (alloc_traits::propagate_on_container_move_assignment::value
			&& !(alloc_traits::is_always_equal::value) )
		{
			std::swap(static_cast<alloc_type&>(a),static_cast<alloc_type&>(b));
		}
		swap(a.host, b.host);
	}

	~allocator_helper() noexcept
	{
		host.destructor(*this);
	}

};


template<class ALL,class T>
struct SampleHost{
	SampleHost(ALL& all)
	{}
	void destructor(ALL& all)
	{

	}
};

int foo()
{
	allocator_helper<std::allocator<int>,
		SampleHost,
		int
	> ah;
	return 0;
}

#endif
