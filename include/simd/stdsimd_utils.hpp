#ifndef STDSIMD_UTILS_HPP
#define STDSIMD_UTILS_HPP

#include "min_int.hpp"

#include<experimental/simd>
namespace stdx = std::experimental;

//TODO the mask uint should actually be selected
//using a template based on the size of the simd mask

template<class Mask>
static inline 
min_int_t<Mask::size()> to_uint(const Mask& m)
{

#if defined(__GNUC__)
    return m.__to_bitset().to_ullong();
#elif defined(__clang__)
    typename class Mask::__native_type z=m;
    return static_cast<mask_cpu_t>(z);
#endif

}

template<class Mask>
static inline 
Mask from_uint(const min_int_t<Mask::size()> & m)
{

#if defined(__GNUC__)
    return m.__to_bitset().to_ullong();
#elif defined(__clang__)
    typename class Mask::__native_type z=m;
    return static_cast<mask_cpu_t>(z);
#endif

}


enum _AlignmentType{
	_Unaligned,
	_Vector,
	_Element
};

template<_AlignmentType at>
struct _alignment_tag_selector{
	static_assert(at!=_Unaligned,"Unaligned reads are undefined behavior.");
};
template<>
struct _alignment_tag_selector<_Element>{
	using type=element_aligned_tag;
};
template<>
struct _alignment_tag_selector<_Vector>{
	using type=vector_aligned_tag;
};

template<size_t _Np>
struct aligned_tag
{
	template <typename _Tp, typename _Up = typename _Tp::value_type>
	static constexpr size_t _S_alignment = _Np;
	template <typename _Tp, typename _Up>
	_GLIBCXX_SIMD_INTRINSIC static constexpr _Up*
	_S_apply(_Up* __ptr){
		constexpr size_t _expected_vector_alignment=vector_aligned_tag::_S_alignment<_Tp,_Up>;
		constexpr size_t _expected_element_alignment=element_aligned_tag::_S_alignment<_Tp,_Up>;
		constexpr _AlignmentType _computed_at=
			((_Np % _expected_vector_alignment)==0) ? _Vector :
			((_Np % _expected_element_alignment)==0) ? _Element : _Unaligned;
		return typename _alignment_tag_selector<_computed_at>::type::_S_apply<_Tp,_Up>(__ptr);
	}
};

#include<array>
#include<vector>
#include<cstdint>
#include<cstdlib>
#include<type_traits>

#include<experimental/simd>
namespace stdx = std::experimental;

#include<array>
#include<vector>
#include<cstdint>
#include<cstdlib>
#include<type_traits>


#include<iostream>


//Process for at least N < 2*CurrentKernel-1
template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
std::enable_if_t<(CurK < MinK),size_t>
_for_n_end(size_t i,size_t N,KernelStruct&& ks,Args&&... args)
{
   return i; //error, didn't work, kernel too small
}
template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
std::enable_if_t<(CurK == MinK),size_t>
_for_n_end(size_t i,size_t N,KernelStruct&& ks,Args&&... args)
{
	if(CurK <= N)
	{
		std::forward<KernelStruct>(ks).
			template kernel<CurK,MinK>(i,N,
			std::forward<Args>(args)...);
		i+=CurK;
	}
	return i;
}
template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
std::enable_if_t<(CurK > MinK),size_t>
_for_n_end(size_t i,size_t N,KernelStruct&& ks,Args&&... args)
{
	if(N >= CurK)
	{
		std::forward<KernelStruct>(ks).
			template kernel<CurK,MinK>(i,N,
			std::forward<Args>(args)...);
		i+=CurK;
		N-=CurK;
	}
	i=_for_n_end<CurK/2,MinK>(i,N,ks,args...);
	return i;
}

struct KernelTest
{
	template<size_t Cur,size_t Min>
	void kernel(size_t i,size_t N)
	{
		std::cout << "Chunk:" << Cur << ",i:" << i << ",N:" << N << std::endl;
	}
};
int main()
{
	_for_n_end<128>(0,129,KernelTest{});

	return 0;
}



//add aligned test here.

#endif
