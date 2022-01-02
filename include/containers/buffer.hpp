template<class T,class Allocator>
class buffer
{
public:
	using value_type=T;
	using allocator_type=Allocator;
	using size_type=std::size_t;
	using difference_type=std::ptrdiff_t;
	using reference=value_type&;
	using const_reference=const value_type&;
	using pointer=std::allocator_traits<Allocator>::pointer;
	using std::allocator_traits<Allocator>::const_pointer;

	using iterator=std::allocator_traits<Allocator>::pointer;
	using const_iterator=std::allocator_traits<Allocator>::const_pointer;
	using reverse_iterator=std::reverse_iterator<iterator>;
	using const_reverse_iterator=std::reverse_iterator<const_iterator>;
private:
	using atraits=std::allocator_traits<Allocator>;
	
	//This is for the empty base optimization so if allocator is empty it does what it's supposed to.
	//https://en.cppreference.com/w/cpp/language/ebo
	struct _impl:public Allocator{
		size_t 
	} m_data;
};

