#pragma once

#include <shv/meta/min_int.hpp>
#include <memory>
#include <array>
#include <type_traits>
#include <cstddef>
#include <cstdint>


namespace shv{
    namespace impl{
        template<uint_fast8_t _lg2_min_size,uint_fast8_t _lg2_max_size>
        struct mask{
        public:
            using index_t=uint_fast8_t;
            constexpr static index_t lg2_min_size=lg2_min_size;
            constexpr static index_t lg2_max_size=lg2_max_size;
            constexpr static uint8_t length=lg2_max_size-lg2_min_size;
            using uint_t=min_uint_t<length,true>;
            constexpr static uint8_t width=sizeof(index_t)*8;
            constexpr static size_t min_size=static_cast<size_t>(1U) << lg2_min_size;
            constexpr static size_t max_size=static_cast<size_t>(1U) << lg2_max_size;
            constexpr static size_t size_from_index(index_t i){
                return static_cast<size_t>(1U) << (i+lg2_min_size);
            }
        private:
            uint_t val=0;
        public:
            constexpr bool operator [](index_t i) const{
                return (i >> 1) & 0x1;
            }
            constexpr void set(index_t i){
                const uint_t bm = static_cast<index_t>(0x1) << i;
                val |= bm;
            }
            constexpr void reset(index_t i){
                const uint_t bm = static_cast<index_t>(0x1) << i;
                val &= ~(bm);
            }

            constexpr operator const uint_t&() const{
                return val;
            }
            constexpr operator uint_t&() {
                return val;
            }
            constexpr operator uint_t&&() &&{
                return val;
            }
            constexpr mask& operator= (uint_t v){
                val=v;
                return *this;
            }

            constexpr mask()=default;
            constexpr mask(const mask& other)=default;
            constexpr mask(mask&& other)=default;
            constexpr explicit mask(uint_t v):val(v){}
            constexpr mask& operator=(const mask& other)=default;
            constexpr mask& operator=(mask&& other)=default;
        };
    }
}