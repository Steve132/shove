
template<class T,class Allocator=std::allocator<T>>
class circular_deque {
public:
	using value_type=T;
	using allocator_type=Allocator;
	using size_type=std::size_t;
	using difference_type=std::ptrdiff_t;
	using reference=value_type&;
	using const_reference=const value_type&;
	using pointer=std::allocator_traits<Allocator>::pointer;
	using std::allocator_traits<Allocator>::const_pointer;
private:
	
};
