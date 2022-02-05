#ifndef SHV_TRUE_ATOMIC_HPP
#define SHV_TRUE_ATOMIC_HPP

/*
#ifdef __linux__
#include<linux/cache.h>
#endif

#if defined(__x86_64__) || defined(_M_ARM64) || defined(_M_IX86) || defined(_M_AMD64)
	#define L1_CACHE_BYTES 64
#endif*/

#include<new>
#include<atomic>
#include<cstdint>
#include<array>
#include<type_traits>
#include<memory>


namespace shv
{
#ifdef __cpp_lib_hardware_interference_size
	using std::hardware_constructive_interference_size;
	using std::hardware_destructive_interference_size;
#elif defined(L1_CACHE_BYTES)
	constexpr std::size_t hardware_constructive_interference_size = L1_CACHE_BYTES;
	constexpr std::size_t hardware_destructive_interference_size = L1_CACHE_BYTES;
#else
	constexpr std::size_t hardware_constructive_interference_size = 256;
	constexpr std::size_t hardware_destructive_interference_size = 256;
#endif

template<class T,class Enable=void>
struct alignas(hardware_constructive_interference_size) true_sharing:
	public T
{
public:
	using T::T;
private:
	std::uint8_t pad[hardware_constructive_interference_size % sizeof(T)];
};

template<class T>
struct alignas(hardware_constructive_interference_size)
	true_sharing<T,std::enable_if_t<!std::is_class<T>::value> >
{
public:
	template<class ...Args>
	true_sharing(Args&&... args):
		data(std::forward<Args>(args)...)
	{}

	operator T& () { return data; }
	operator const T& () const { return data ;}
private:
	T data;
	std::uint8_t pad[hardware_constructive_interference_size % sizeof(T)];
};

template<class T>
using true_atomic=true_sharing<std::atomic<T>>;

}

#endif
