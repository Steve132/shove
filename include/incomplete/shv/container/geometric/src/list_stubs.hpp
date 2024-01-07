#pragma once


namespace shv {

template <typename T, typename A>
inline geometric_list<T,A>::geometric_list(){}

template <typename T, typename A>
geometric_list<T,A>::geometric_list(const geometric_list&){}

template <typename T, typename A>
geometric_list<T,A>::geometric_list(geometric_list&&) noexcept{}

template <typename T, typename A>
geometric_list<T,A>::geometric_list(const A&){}

template <typename T, typename A>
geometric_list<T,A>::geometric_list(size_type count, const value_type& value, const A& alloc){}

template <typename T, typename A>
geometric_list<T,A>::geometric_list(size_type count, const A& alloc){}

template <typename T, typename A>
template <typename InputIt>
geometric_list<T,A>::geometric_list(InputIt first, InputIt last, const A& alloc){}

template <typename T, typename A>
geometric_list<T,A>::geometric_list(std::initializer_list<value_type> init, const A& alloc){}

// Destructor
template <typename T, typename A>
geometric_list<T,A>::~geometric_list(){}

// Assignment operators
template <typename T, typename A>
geometric_list<T,A>& geometric_list<T,A>::operator=(const geometric_list&){}
template <typename T, typename A>
geometric_list<T,A>& geometric_list<T,A>::operator=(geometric_list&&) noexcept{}
template <typename T, typename A>
geometric_list<T,A>& geometric_list<T,A>::operator=(std::initializer_list<value_type>){}

// Capacity
template <typename T, typename A>
bool geometric_list<T,A>::empty() const noexcept{}

template <typename T, typename A>
typename geometric_list<T,A>::size_type geometric_list<T,A>::size() const noexcept{}

template <typename T, typename A>
typename geometric_list<T,A>::size_type geometric_list<T,A>::max_size() const noexcept{}

// Element access
template <typename T, typename A>
typename geometric_list<T,A>::reference geometric_list<T,A>::front(){}

template <typename T, typename A>
typename geometric_list<T,A>::const_reference geometric_list<T,A>::front() const{}

template <typename T, typename A>
typename geometric_list<T,A>::reference geometric_list<T,A>::back(){}

template <typename T, typename A>
typename geometric_list<T,A>::const_reference geometric_list<T,A>::back() const{}

// Random access
template <typename T, typename A>
typename geometric_list<T,A>::reference geometric_list<T,A>::operator[](size_type){}

template <typename T, typename A>
typename geometric_list<T,A>::const_reference geometric_list<T,A>::operator[](size_type) const{}

template <typename T, typename A>
typename geometric_list<T,A>::reference geometric_list<T,A>::at(typename geometric_list<T,A>::size_type){}

template <typename T, typename A>
typename geometric_list<T,A>::const_reference geometric_list<T,A>::at(typename geometric_list<T,A>::size_type) const{}

// Modifiers
template <typename T, typename A>
void geometric_list<T,A>::clear() noexcept{}

template <typename T, typename A>
void geometric_list<T,A>::push_back(const value_type&){}

template <typename T, typename A>
void geometric_list<T,A>::push_back(value_type&&){}

template <typename T, typename A>
void geometric_list<T,A>::push_front(const value_type&){}

template <typename T, typename A>
void geometric_list<T,A>::push_front(value_type&&){}

template <typename T, typename A>
void geometric_list<T,A>::pop_back(){}

template <typename T, typename A>
void geometric_list<T,A>::pop_front(){}

template <typename T, typename A>
template <typename... Args>
void geometric_list<T,A>::emplace_front(Args&&... args){}

template <typename T, typename A>
template <typename... Args>
void geometric_list<T,A>::emplace_back(Args&&... args){}

template <typename T, typename A>
template <typename InputIt>
void geometric_list<T,A>::prepend_range(InputIt first, InputIt last){}

template <typename T, typename A>
template <typename InputIt>
void geometric_list<T,A>::append_range(InputIt first, InputIt last){}

template <typename T, typename A>
void geometric_list<T,A>::resize(size_type count){}

template <typename T, typename A>
void geometric_list<T,A>::resize(size_type count, const value_type& value){}

template <typename T, typename A>
void geometric_list<T,A>::reserve(size_type new_cap){}

template <typename T, typename A>
void geometric_list<T,A>::assign(size_type count, const value_type& value){}

template <typename T, typename A>
template <typename InputIt>
void geometric_list<T,A>::assign(InputIt first, InputIt last){}

template <typename T, typename A>
void geometric_list<T,A>::assign(std::initializer_list<T>){}

template <typename T, typename A>
template <typename InputIt>
void geometric_list<T,A>::assign_range(InputIt first, InputIt last){}

// Iterators
template <typename T, typename A>
typename geometric_list<T,A>::iterator geometric_list<T,A>::begin() noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::const_iterator geometric_list<T,A>::begin() const noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::iterator geometric_list<T,A>::end() noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::const_iterator geometric_list<T,A>::end() const noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::reverse_iterator geometric_list<T,A>::rbegin() noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::const_reverse_iterator geometric_list<T,A>::rbegin() const noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::reverse_iterator geometric_list<T,A>::rend() noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::const_reverse_iterator geometric_list<T,A>::rend() const noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::const_iterator geometric_list<T,A>::cbegin() const noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::const_iterator geometric_list<T,A>::cend() const noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::const_reverse_iterator geometric_list<T,A>::crbegin() const noexcept{}
template <typename T, typename A>
typename geometric_list<T,A>::const_reverse_iterator geometric_list<T,A>::crend() const noexcept{}

// A
template <typename T, typename A>
typename geometric_list<T,A>::allocator_type geometric_list<T,A>::get_allocator() const noexcept{}

} // namespace shv

#include "list_iterator_stubs.hpp"
