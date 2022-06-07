#pragma once

#ifndef GAL_ALLOCATOR_NO_TRACE
#include <iostream>// for std::clog
#include <galToolbox/string/format.hpp>
#include <galToolbox/utils/source_location.hpp>
#define GAL_ALLOCATOR_TRACE_DO(...) __VA_ARGS__
#else
	#define GAL_ALLOCATOR_TRACE_DO(...)
#endif

namespace gal::toolbox::memory
{
	template<typename T>
	struct default_allocator
	{
		using allocator_type = std::allocator<T>;
		using allocator_traits = std::allocator_traits<allocator_type>;

		using value_type = typename allocator_traits::value_type;
		using pointer = typename allocator_traits::pointer;
		using size_type = typename allocator_traits::size_type;

		[[no_unique_address]] allocator_type allocator;

		[[nodiscard]] constexpr auto allocate(
				size_type n
				GAL_ALLOCATOR_TRACE_DO(
						,
						const std_source_location& location = std_source_location::current()))
		{
			auto* ret = allocator.template allocate<alignof(T)>(n * sizeof(T));

			GAL_ALLOCATOR_TRACE_DO(
					std::clog << std_format::format(
						"Allocate {} object(s) at {} ({} byte(s) per object), total used {} bytes. Allocate at: [file:{}][line:{}, column: {}][function:{}]\n",
						n,
						static_cast<void*>(ret),
						sizeof(value_type),
						sizeof(value_type) * n,
						location.file_name(),
						location.line(),
						location.column(),
						location.function_name());)

			return ret;
		}

		constexpr void deallocate(
				T* p,
				size_type n
				GAL_ALLOCATOR_TRACE_DO(
						,
						const std_source_location& location = std_source_location::current()))
		{
			GAL_ALLOCATOR_TRACE_DO(
					std::clog << std_format::format(
						"Deallocate {} object(s) at {} ({} byte(s) per object), total used {} bytes. Deallocate at: [file:{}][line:{}, column: {}][function:{}]\n",
						n,
						static_cast<void*>(p),
						sizeof(value_type),
						sizeof(value_type) * n,
						location.file_name(),
						location.line(),
						location.column(),
						location.function_name());)
			allocator_traits::deallocate(allocator, p, n);
		}

		template<typename U, typename... Args>
		constexpr void construct(U* p, Args&&... args) { allocator_traits::construct(allocator, p, std::forward<Args>(args)...); }

		template<typename U>
		constexpr void destroy(U* p) { allocator_traits::destroy(allocator, p); }

		friend constexpr bool operator==(const default_allocator& lhs, const default_allocator& rhs) noexcept { return lhs.allocator == rhs.allocator; }
	};

	template<typename T1, typename T2>
	constexpr bool operator==(const default_allocator<T1>& lhs, const default_allocator<T2>& rhs) { return lhs.allocator == rhs.allocator; }
}// namespace gal::toolbox::memory

template<typename ValueType>
struct std::allocator_traits<::gal::toolbox::memory::default_allocator<ValueType>>
{
	using allocator_type = ::gal::toolbox::memory::default_allocator<ValueType>;
	using internal_allocator_traits = typename allocator_type::allocator_traits;

	using value_type = typename internal_allocator_traits::value_type;
	using pointer = typename internal_allocator_traits::pointer;
	using const_pointer = typename internal_allocator_traits::const_pointer;
	using void_pointer = typename internal_allocator_traits::void_pointer;
	using const_void_pointer = typename internal_allocator_traits::const_void_pointer;
	using difference_type = typename internal_allocator_traits::difference_type;
	using size_type = typename internal_allocator_traits::size_type;

	using propagate_on_container_copy_assignment = typename internal_allocator_traits::propagate_on_container_copy_assignment;
	using propagate_on_container_move_assignment = typename internal_allocator_traits::propagate_on_container_move_assignment;
	using propagate_on_container_swap = typename internal_allocator_traits::propagate_on_container_swap;
	using is_always_equal = typename internal_allocator_traits::is_always_equal;

	template<typename T>
	using rebind_alloc = ::gal::toolbox::memory::default_allocator<T>;
	template<typename T>
	using rebind_traits = allocator_traits<rebind_alloc<T>>;

	[[nodiscard]] constexpr static pointer allocate(
			allocator_type& a,
			size_type n
			GAL_ALLOCATOR_TRACE_DO(
					,
					const std_source_location& location = std_source_location::current()))
	{
		return a.allocate(n
		                  GAL_ALLOCATOR_TRACE_DO(, location));
	}

	constexpr static void deallocate(
			allocator_type& a,
			pointer p,
			size_type n
			GAL_ALLOCATOR_TRACE_DO(
					,
					const std_source_location& location = std_source_location::current()))
	{
		return a.deallocate(
				p,
				n
				GAL_ALLOCATOR_TRACE_DO(, location));
	}

	template<typename T, typename... Args>
	constexpr static void construct(allocator_type& a, T* p, Args&&... args) { a.construct(p, std::forward<Args>(args)...); }

	template<typename T>
	constexpr static void destroy(allocator_type& a, T* p) { a.destroy(p); }

	constexpr static size_type max_size(const allocator_type& a) noexcept { return internal_allocator_traits::max_size(a.allocator); }

	constexpr static allocator_type select_on_container_copy_construction(const allocator_type& a) { return a; }
};
