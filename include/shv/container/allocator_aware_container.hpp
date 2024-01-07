#ifndef ALLOCATOR_AWARE_CONTAINER_HPP
#define ALLOCATOR_AWARE_CONTAINER_HPP

#include<memory>


namespace shv
{
template<class Container>
struct allocator_operations
{
public:

};

template<class Allocator>
struct allocator_aware_behaviors
{
public:

private:
	template<bool on_move_assignment>
	static constexpr bool should_propagate_alloc_on_assignment_static() noexcept
	{
		if constexpr (std::allocator_traits<Allocator>::is_always_equal)
		{
			return false;
		}
		else if constexpr (on_move_assignment)
		{
			return std::allocator_traits<Allocator>::propagate_on_move_assignment;
		}
		else {
			return std::allocator_traits<Allocator>::propagate_on_copy_assignment;
		}
	}

	template<bool on_move_assignment,class OtherAllocator>
	static bool should_propagate_alloc_on_assignment(const Allocator& old_alloc,const OtherAllocator& other_alloc) noexcept
	{
		if constexpr (should_propagate_alloc_on_assignment_static<on_move_assignment>())
		{
			return true;
		}
		else{
			return old_alloc!=other_alloc;
		}
	}
	template<bool on_move_assignment,class OtherAllocator>
	static const Allocator& get_propagated_allocator(const Allocator& old_alloc,const OtherAllocator& other_alloc) noexcept
	{
		if constexpr (should_propagate_alloc_on_assignment_static<on_move_assignment>())
		{
			return other_alloc;
		}
		else{
			return old_alloc; //if they're equal then this doesn't matter.
		}
	}

protected:
	template<class OtherAllocator,
			 class ClearFunc,
			 class ReAllocCopyFunc>
	static const Allocator& copy_assignment(
		const Allocator& old_alloc,
		const OtherAllocator& other_alloc,
		ClearFunc&& Clear,
		ReAllocCopyFunc&& ReAllocCopy
		)
	{
		bool prop=should_propagate_alloc_on_assignment<false>(other_alloc);
		Clear(old_alloc,prop); //boolean decides if also delete.
		const Allocator& new_alloc = prop ? other_alloc : old_alloc;
		ReAllocCopy(new_alloc); //realloc if needed and copy
		return new_alloc;
	}


	static constexpr bool nothrow_move_assignment=should_propagate_alloc_on_assignment_static<true>();

	template<
		class OtherAllocator,
		class ClearFunc,
		class MoveFunc,
		class ReAllocMoveFunc>
	static const Allocator& move_assignment(const Allocator& old_alloc,
											const OtherAllocator& other_alloc,
											ClearFunc&& Clear,
											MoveFunc&& Move,
											ReAllocMoveFunc&& ReAllocMove) noexcept(nothrow_move_assignment)
	{
		bool prop=should_propagate_alloc_on_assignment<true>(other_alloc);
		Clear(old_alloc,prop);

		if(prop)
		{
			Move(other_alloc,true);
			return other_alloc;
		}
		else
		{
			ReAllocMove(old_alloc);
			return old_alloc;
		}
	}
};

template<class Allocator>
struct allocator_aware_container: protected Allocator
{
	constexpr allocator_aware_container() noexcept(noexcept(Allocator()))
		:Allocator()
	{}
	constexpr allocator_aware_container( const Allocator& alloc ) noexcept
		:Allocator(
			std::allocator_traits<Allocator>::select_on_container_copy_construction(alloc)
		)
	{}
	constexpr allocator_aware_container( const allocator_aware_container& aac ) noexcept
		:allocator_aware_container(static_cast<const Allocator&>(aac))
	{}
	constexpr allocator_aware_container( Allocator&& alloc ) noexcept
		:Allocator(std::move(alloc))
	{}
	constexpr allocator_aware_container(allocator_aware_container&& aac ) noexcept
		:allocator_aware_container(static_cast<Allocator&&>(aac))
	{}
	constexpr Allocator get_allocator() const noexcept
	{
		return *this;
	}

	//there should be no implicit assignment operators.
	constexpr allocator_aware_container& operator=(allocator_aware_container&& other ) noexcept = delete;
	template<class OtherAllocator>
	constexpr allocator_aware_container& operator=(OtherAllocator&& other ) noexcept = delete;

	constexpr allocator_aware_container& operator=(const allocator_aware_container& other ) noexcept = delete;
	template<class OtherAllocator>
	constexpr allocator_aware_container& operator=(const OtherAllocator& other ) noexcept = delete;

	using allocator_type=Allocator;
protected:


};

}




#endif
