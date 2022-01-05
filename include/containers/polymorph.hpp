#ifndef SHV_POLYMORPH_HPP
#define SHV_POLYMORPH_HPP

#include<any>
#include<utility>
#include<type_traits>

namespace shv
{

template<class T,bool downcast_is_safe=false>
class polymorph
{
private:
	template<class U>
	struct is_polymorph_allowed{
		static constexpr bool value= std::is_base_of_v<T,U> ||(std::is_base_of_v<U,T> && std::is_polymorphic_v<U>);
	};
	template<class U>
	void update_ptr(){
		U* uptr=std::any_cast<U>(&internal);
		if constexpr ((!downcast_is_safe) && std::is_base_of_v<U,T> && std::is_polymorphic_v<U> && !std::is_same_v<U,T>){
			ptr=dynamic_cast<T*>(uptr);
		}
		else{
			ptr=static_cast<T*>(uptr);
		}
	}
public:
	template<class ...Args>
	polymorph(Args&&... args):
		internal{std::make_any<T>(args...)}
	{	update_ptr();	}

	template<class U,typename std::enable_if_t <is_polymorph_allowed<U>::value, int > = 0>
	polymorph(const U& u):internal(u)
	{	update_ptr();	}
	template<class U,typename std::enable_if_t <is_polymorph_allowed<U>::value, int > = 0>
	polymorph(U&& u):internal(std::move(u))
	{	update_ptr();	}
	template<class U,typename std::enable_if_t <is_polymorph_allowed<U>::value, int > = 0>
	polymorph& operator=(const U& u){
		internal=u;
		update_ptr();
		return *this;
	}
	template<class U,typename std::enable_if_t <is_polymorph_allowed<U>::value, int > = 0>
	polymorph& operator=(U&& u){
		internal=std::move(u);
		update_ptr();
		return *this;
	}
	template<class U,typename std::enable_if_t <is_polymorph_allowed<U>::value, int > = 0>
	polymorph(const polymorph<U>& u):internal(u)
	{	update_ptr();	}
	template<class U,typename std::enable_if_t <is_polymorph_allowed<U>::value, int > = 0>
	polymorph(polymorph<U>&& u):internal(std::move(u))
	{	update_ptr();	}
	template<class U,typename std::enable_if_t <is_polymorph_allowed<U>::value, int > = 0>
	polymorph& operator=(polymorph<U>&& u){
		internal=std::move(u);
		update_ptr();
		return *this;
	}
	template<class U,typename std::enable_if_t <is_polymorph_allowed<U>::value, int > = 0>
	polymorph& operator=(const polymorph<U>& u){
		internal=u;
		update_ptr();
		return *this;
	}

	operator T&() {
		return *ptr;
	}
	operator T&&(){
		return std::move(*ptr);
	}
	operator const T&() const{
		return *ptr;
	}

private:
	std::any internal;
	T* ptr;
};

}
#endif // SHV_POLYMORPH_HPP
