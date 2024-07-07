#ifndef SHV_TRUE_ATOMIC_HPP
#define SHV_TRUE_ATOMIC_HPP


#ifdef __linux__
#include<linux/cache.h>
#endif

#if defined(__x86_64__) || defined(_M_ARM64) || defined(_M_IX86) || defined(_M_AMD64)
	#define L1_CACHE_BYTES 64
#endif

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

template<class T>
using no_false_sharing=alignas(hardware_constructive_interference_size) T;

namespace {
struct empty{};
}

#ifdef _MSVC_VER
using cache_boundary=[[msvc::no_unique_address]] empty;
#else
using cache_boundary=[[no_unique_address]] empty;
#endif

template<class T>
using true_atomic=no_false_sharing<std::atomic<T>>;

}

#endif
