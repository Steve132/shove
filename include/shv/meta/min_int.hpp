#ifndef MIN_INT_HPP
#define MIN_INT_HPP

#include<type_traits>
#include<cstdint>


template<unsigned int num_bits,bool fast=false,class Enable=void>
struct min_uint{};

template<unsigned int num_bits>
struct min_uint<num_bits,false,std::enable_if_t< (num_bits <= 8) > >
{	using type=uint8_t;  };

template<unsigned int num_bits>
struct min_uint<num_bits,false,std::enable_if_t< (num_bits > 8) && (num_bits <= 16) > >
{	using type=uint16_t;  };

template<unsigned int num_bits>
struct min_uint<num_bits,false,std::enable_if_t< (num_bits > 16) && (num_bits <= 32) > >
{	using type=uint32_t;  };

template<unsigned int num_bits>
struct min_uint<num_bits,false,std::enable_if_t< (num_bits > 32) && (num_bits <= 64) > >
{	using type=uint64_t;  };

template<unsigned int num_bits>
struct min_uint<num_bits,true,std::enable_if_t< (num_bits <= 8) > >
{	using type=uint_fast8_t;  };

template<unsigned int num_bits>
struct min_uint<num_bits,true,std::enable_if_t< (num_bits > 8) && (num_bits <= 16) > >
{	using type=uint_fast16_t;  };

template<unsigned int num_bits>
struct min_uint<num_bits,true,std::enable_if_t< (num_bits > 16) && (num_bits <= 32) > >
{	using type=uint_fast32_t;  };

template<unsigned int num_bits>
struct min_uint<num_bits,true,std::enable_if_t< (num_bits > 32) && (num_bits <= 64) > >
{	using type=uint_fast64_t;  };

template<unsigned int num_bits,bool fast>
using min_uint_t=typename min_uint<num_bits,fast>::type;

#endif
