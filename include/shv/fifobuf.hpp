#pragma once

#include <streambuf>
#include <array>
#include <cstddef>
#include <atomic>
#include <algorithm> // std::min
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <type_traits>

#include "min_int.hpp"
#include "array_traits.hpp"

namespace shv
{
	/*
	pos_type tellp();
	pos_type tellg(); call pubseekoff(0, cur, in) or pubseekoff(0, cur, out) to get the current position of the get or put pointer, respectively.
	*/

	template <class SpanLike, class Traits = std::char_traits<shv::element_type_t<SpanLike>>>
	class basic_fifobuf : public std::basic_streambuf<shv::element_type_t<SpanLike>, Traits>
	{
		public:
			using char_type = shv::element_type_t<SpanLike>;
			// Standard type aliases
			using traits_type = Traits;
			using int_type = typename traits_type::int_type;
			using pos_type = typename traits_type::pos_type;
			using off_type = typename traits_type::off_type;

		protected:
			using base_type = std::basic_streambuf<char_type, Traits>;
			void debug_sync() const;

		public:
			SpanLike buffer;

			constexpr std::streamsize capacity() const noexcept
			{
				return static_cast<std::streamsize>(std::size(buffer));
			}

			std::streamsize size() const noexcept
			{
				state_indexes s = get_state_indexes();
				fifo_info_t info(s, capacity());
				return info.size;
			}

		protected:
			static constexpr auto ext = decltype(std::span(buffer))::extent;
			using fast_size_type = shv::uint_fast_can_hold_t<
				(ext == std::dynamic_extent) ? std::dynamic_extent : static_cast<std::size_t>(ext) * 2
			>;

			struct state_indexes
			{
				fast_size_type h_beg, h, h_end, t_beg, t, t_end;
			};


			constexpr state_indexes get_state_indexes() const noexcept //this needs to be inline for sure.)
			{
				state_indexes s;
				auto bptr = buffer.data();
				s.h_beg = static_cast<fast_size_type>(base_type::eback() - bptr);
				s.h = static_cast<fast_size_type>(base_type::gptr() - bptr);
				s.h_end = static_cast<fast_size_type>(base_type::egptr() - bptr);
				s.t_beg = static_cast<fast_size_type>(base_type::pbase() - bptr);
				s.t = static_cast<fast_size_type>(base_type::pptr() - bptr);
				s.t_end = static_cast<fast_size_type>(base_type::epptr() - bptr);
				return s;
			}


			struct fifo_info_t
			{
				fast_size_type head = 0;
				fast_size_type size = 0; //if not segmented this is 0.
				fast_size_type capacity = 0;

				constexpr fifo_info_t() noexcept = default;

				constexpr fifo_info_t(fast_size_type cap) noexcept:
					capacity(cap)
				{
				}

				//segmented means that the buffer is split into two segments, specifically

				constexpr fifo_info_t(const state_indexes& s, fast_size_type cap) noexcept:
					capacity(cap)
				{
					if (s.h == s.t)
					{
						if (s.h_beg < s.t_end)
						{
							size = 0;
							head = 0; //empty resets the head to 0. Always.
						}
						else
						{
							size = cap - s.h + s.t;
							head = s.h;
						}
					}
					else
					{
						if (s.h < s.t)
						{
							size = s.t - s.h;
							head = s.h;
						}
						else
						{
							size = cap - s.h + s.t;
							head = s.h;
						}
					}
					if (head == cap)
					{
						head = 0;
					}
				}

				state_indexes normalize(bool pbackfail=false) const
				{
					state_indexes s;
					fast_size_type lhead = head;
					if (lhead == capacity)
					{
						lhead = 0;
					}

					fast_size_type tabove = lhead + size;

					if (tabove >= capacity)
					{
						s.t_beg = 0;
						s.t = tabove - capacity;
						//000
						if(!pbackfail){
							s.t_end = lhead;
							s.h_beg = lhead;
						}
						else{
							s.t_end = s.t;
							s.h_beg = s.t;
						}
						s.h = lhead;
						//s.h
						s.h_end = capacity;
					}
					else
					{
						s.h_beg = 0;
						//000
						s.h = lhead;
						s.h_end = tabove;
						s.t_beg = tabove;
						s.t = tabove;
						//000
						s.t_end = capacity;
					}

					return s;
				}
			};

		public:
#ifndef NDEBUG
			mutable state_indexes debug_indexes;
			mutable fifo_info_t debug_info;
#endif

		protected:
			void set_state_indexes(const state_indexes& s)
			{
				//t_beg must be t on set.
				auto bptr = buffer.data();
				base_type::setg(bptr + s.h_beg, bptr + s.h, bptr + s.h_end);
				base_type::setp(bptr + s.t_beg, bptr + s.t_end);
				base_type::pbump(s.t - s.t_beg);
				debug_sync();
			}

		protected:
			std::streamsize showmanyc() override //guaranteed that underflow wont return eof.  Not guaranteed that underflow won't be called.
			{
				return size();
			}

		protected:
			int_type overflow(int_type ch = traits_type::eof()) override
			{
				state_indexes s = get_state_indexes();
				fifo_info_t info(s, capacity());

				debug_sync();

				if (info.size >= info.capacity)
				{
					return traits_type::eof();
				}
				s = info.normalize();
				set_state_indexes(s);
				if (ch != traits_type::eof())
				{
					*base_type::pptr() = static_cast<char_type>(ch);
					base_type::pbump(1);
				}

				debug_sync();
				return traits_type::not_eof(ch);
			}

		private:


		protected:
			int_type underflow() override
			{
				state_indexes s = get_state_indexes();
				fifo_info_t info(s, capacity());
				s = info.normalize();
				set_state_indexes(s);
				if (info.size == 0)
				{
					return traits_type::eof();
				}
				return traits_type::to_int_type(buffer[s.h]);
			}

		private:

		public:
			/*TODO: worry about this later
			virtual int_type pbackfail( int_type c = Traits::eof() ) override
			{
				_state_indexes s = _get_state_indexes();
				if(!_normalize_on_pbackfail(s,info)){
					return Traits::eof();
				}
				if(s.h==0){
					s.h=sN-1;
					s.h_end=sN;
					s.h_beg=s.t;
					s.t_end=s.t;
					s.t_beg=0;
				}
				else{
					if(s.h_beg < s.t_end){
						--s.h;
						s.h_beg=0;

					s.h_end=s.t;
					s.t_
				}


				_set_state_indexes(s);
				return c;
			}
			*/
		public:
			template <class... Args>
			constexpr basic_fifobuf(Args&&... args) noexcept : buffer(std::forward<Args>(args)...)
			{
				base_type::setg(buffer.data(), buffer.data(), buffer.data());
				base_type::setp(buffer.data(), buffer.data() + buffer.size());
				debug_sync();
			}

			void setstate(size_t head, size_t size)
			{
				size_t C = capacity();
				head %= C;

				fifo_info_t new_info(C);
				new_info.head = static_cast<fast_size_type>(head);
				new_info.size = std::min(size, new_info.capacity - new_info.head);
				state_indexes s = new_info.normalize();
				set_state_indexes(s);
			}
	};

	template <class SpanLike, class Traits>
	inline void basic_fifobuf<SpanLike, Traits>::debug_sync() const
	{
#ifndef NDEBUG
		debug_indexes = get_state_indexes();
		debug_info = fifo_info_t(debug_indexes, capacity());
#endif
	}

	template <class SpanLike>
	using fifobuf = basic_fifobuf<SpanLike>;
}
