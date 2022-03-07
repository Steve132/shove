

template<class T>
struct minihive:
    public std::pmr::list<T>;  //iterators and everything work the same way from this root.
{
protected:
	using base_type=std::pmr::forward_list<T>;
	using iterator=base_type::iterator;
	
	minihive():
		std::pmr::polymorphic_allocator<T>(inner_pool),
		ptree(inner_pool)
	{}

	template<class ...Args>
	iterator emplace(Args&&... args )	{
		std::pmr::forward_list<T> staging(inner_pool); //we need to splice the node into the list at the right spot after allocaton
		staging.emplace_back(std::forward<Args>(args)...);
		T* address=&*(staging.begin()); //get the address of the allocated node.
		auto nearest=neighborfinder.upper_bound(address);
		base_type::splice(nearest.second,staging.begin()); //splice the node into the list such that it iterates 
		auto just_inserted=nearest.second-1;			//in memory order like hive
		neighborfinder.emplace(address,just_inserted);
		return just_inserted;
	}

	iterator insert(const T& item)	{
		return emplace(item);
	}

	iterator get_iterator(const T* address) {
		auto nfit=neighborfinder.find(address);
		if(nfit == neighborinder.end() return base_type::end();
		return nfit;
	}

private:

	//hide invalid inserts
	using base_type::emplace_front;
	using base_type::emplace_after;
	using base_type::insert_front;
	using base_type::insert_after;
	using base_type::pop_front;

	std::pmr::unsynchronized_pool_resource inner_pool;
	std::map<T*,base_type::iterator> neighborfinder;
};

