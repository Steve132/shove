#pragma once

#include <array>
#include <span>
#include <type_traits>
#include <iterator>
#include <cstddef>

#include "min_int.hpp"

namespace shv
{
	/// TODO: add higher rank variants later.
	template <typename R, unsigned int D = 0>
	struct generic_extent;

	template <typename R>
		requires (requires(R& r) { std::span(r); })
	struct generic_extent<R, 0> : std::integral_constant<std::size_t,
														std::dynamic_extent
		>
	{
	};


	// Helper variable template
	template <typename T>
	inline constexpr std::size_t generic_extent_v = generic_extent<T>::value;


	template <typename R>
	using element_type_t = std::remove_reference_t<decltype(std::span(std::declval<R>()))>::element_type;

	template <typename T>
	using fast_size_for_t = uint_fast_can_hold_t<generic_extent_v<T>>;

	template <typename T>
	using fast_index_for_t = std::conditional_t<generic_extent_v<T> != std::dynamic_extent,
												uint_fast_can_hold_t<generic_extent_v<T> - 1>, std::size_t>;
}
