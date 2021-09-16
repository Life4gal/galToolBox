#pragma once

#include <array>

#include <gal/tuple_maker.hpp>
#include <gal/stride_iterator.hpp>

#include <gal/math_fwd.hpp>

namespace gal::toolbox::utils
{
	/**
	 * @brief specialize the vector/vector_view to support the construction of other vectors
	 * @tparam T vector/vector_view
	*/
	template <math::vector_type_t T>
	struct tuple_maker_trait<T> : std::true_type
	{
		using value_type = typename T::value_type;
		constexpr static std::size_t size = T::data_size;
	};
}

namespace gal::toolbox::math
{
	template <typename T, std::size_t N>
	class vector
	{
	public:
		using container_type = std::array<T, N>;
		using value_type = typename container_type::value_type;
		using size_type = typename container_type::size_type;

		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;
		using reverse_iterator = typename container_type::reverse_iterator;
		using const_reverse_iterator = typename container_type::const_reverse_iterator;

		constexpr static size_type data_size = N;

		template<typename U = value_type>
		using copy_type = vector<U, data_size>;

		// Stride may not be 1, and even Iterator is not std::array<value_type, data_size>::iterator but std::array<value_type, N>::iterator (N != data_size)
		template <std::size_t Stride, typename Iterator = iterator>
		using view_type = vector_view<Stride, data_size / Stride, Iterator>;
		// Stride may not be 1, and even Iterator is not std::array<value_type, data_size>::const_iterator but std::array<value_type, N>::const_iterator (N != data_size)
		template <std::size_t Stride, typename Iterator = const_iterator>
		using const_view_type = vector_view<Stride, data_size, Iterator>;

		/**
		 * @brief default construction
		*/
		constexpr explicit vector() noexcept(std::is_nothrow_default_constructible_v<container_type>) = default;

		/**
		 * @brief tuple composition parameter construction
		 * @tparam Tuple Tuple type
		 * @tparam I index sequence
		 * @param tuple args tuple
		*/
		template <typename Tuple, size_type...I>
		requires (sizeof...(I) <= data_size)
				constexpr explicit vector(Tuple&& tuple, std::index_sequence<I...>)
						noexcept((noexcept(static_cast<value_type>(std::get<I>(std::forward<Tuple>(tuple)))) && ...))
			: data_({static_cast<value_type>(std::get<I>(std::forward<Tuple>(tuple)))...}) {}

		/**
		 * @brief arbitrary parameter construction
		 * It is allowed to use arithmetic data and vector to construct a vector at the same time.
		 * If the provided data cannot satisfy all the data initialization, the remaining data will be initialized by default.
		 * If the provided data is more than the required data, the extra part will be discarded.
		 * @tparam Args args type
		 * @param args args
		*/
		template <typename... Args>
		constexpr explicit vector(Args&&...args)
				noexcept(noexcept(std::tuple_cat(utils::tuple_maker::to_rref_tuple(std::forward<Args>(args))...)))
			: vector(
					  std::tuple_cat(utils::tuple_maker::to_rref_tuple(std::forward<Args>(args))...),
					  std::make_index_sequence<std::min(
							  data_size,
							  std::tuple_size_v<decltype(
									  std::tuple_cat(
											  utils::tuple_maker::to_rref_tuple(
													  std::forward<
															  Args>(args))
													  ...
											  )
											  )>
							  )>{}
			  ) {}

		[[nodiscard]] constexpr copy_type<> copy() const noexcept
		{
			return { *this };
		}

		template <std::size_t Stride = 1>
		[[nodiscard]] constexpr view_type<Stride> to_view() noexcept
		{
			return view_type<Stride>{data_.begin()};
		}

		template <std::size_t Stride = 1>
		[[nodiscard]] constexpr const_view_type<Stride> to_view() const noexcept
		{
			return const_view_type<Stride>{data_.cbegin()};
		}

		[[nodiscard]] constexpr static size_type size() noexcept
		{
			return data_size;
		}

		[[nodiscard]] constexpr reference operator[](size_type index) noexcept
		{
			return data_[index];
		}

		[[nodiscard]] constexpr const_reference operator[](size_type index) const noexcept
		{
			return data_[index];
		}

		[[nodiscard]] constexpr reference x() noexcept
				requires(data_size >= 1)
		{
			return data_[0];
		}

		[[nodiscard]] constexpr const_reference x() const noexcept
				requires(data_size >= 1)
		{
			return data_[0];
		}

		[[nodiscard]] constexpr reference r() noexcept
				requires(data_size >= 1)
		{
			return data_[0];
		}

		[[nodiscard]] constexpr const_reference r() const noexcept
				requires(data_size >= 1)
		{
			return data_[0];
		}

		[[nodiscard]] constexpr reference s() noexcept
				requires(data_size >= 1)
		{
			return data_[0];
		}

		[[nodiscard]] constexpr const_reference s() const noexcept
				requires(data_size >= 1)
		{
			return data_[0];
		}

		[[nodiscard]] constexpr reference y() noexcept
				requires(data_size >= 2)
		{
			return data_[1];
		}

		[[nodiscard]] constexpr const_reference y() const noexcept
				requires(data_size >= 2)
		{
			return data_[1];
		}

		[[nodiscard]] constexpr reference g() noexcept
				requires(data_size >= 2)
		{
			return data_[1];
		}

		[[nodiscard]] constexpr const_reference g() const noexcept
				requires(data_size >= 2)
		{
			return data_[1];
		}

		[[nodiscard]] constexpr reference t() noexcept
				requires(data_size >= 2)
		{
			return data_[1];
		}

		[[nodiscard]] constexpr const_reference t() const noexcept
				requires(data_size >= 2)
		{
			return data_[1];
		}

		[[nodiscard]] constexpr reference z() noexcept
				requires(data_size >= 3)
		{
			return data_[2];
		}

		[[nodiscard]] constexpr const_reference z() const noexcept
				requires(data_size >= 3)
		{
			return data_[2];
		}

		[[nodiscard]] constexpr reference b() noexcept
				requires(data_size >= 3)
		{
			return data_[2];
		}

		[[nodiscard]] constexpr const_reference b() const noexcept
				requires(data_size >= 3)
		{
			return data_[2];
		}

		[[nodiscard]] constexpr reference p() noexcept
				requires(data_size >= 3)
		{
			return data_[2];
		}

		[[nodiscard]] constexpr const_reference p() const noexcept
				requires(data_size >= 3)
		{
			return data_[2];
		}

		[[nodiscard]] constexpr reference w() noexcept
				requires(data_size >= 4)
		{
			return data_[3];
		}

		[[nodiscard]] constexpr const_reference w() const noexcept
				requires(data_size >= 4)
		{
			return data_[3];
		}

		[[nodiscard]] constexpr reference a() noexcept
				requires(data_size >= 4)
		{
			return data_[3];
		}

		[[nodiscard]] constexpr const_reference a() const noexcept
				requires(data_size >= 4)
		{
			return data_[3];
		}

		[[nodiscard]] constexpr reference q() noexcept
				requires(data_size >= 4)
		{
			return data_[3];
		}

		[[nodiscard]] constexpr const_reference q() const noexcept
				requires(data_size >= 4)
		{
			return data_[3];
		}

		[[nodiscard]] constexpr iterator begin() noexcept
		{
			return data_.begin();
		}

		[[nodiscard]] constexpr const_iterator begin() const noexcept
		{
			return data_.begin();
		}

		[[nodiscard]] constexpr iterator end() noexcept
		{
			return data_.end();
		}

		[[nodiscard]] constexpr const_iterator end() const noexcept
		{
			return data_.end();
		}

		[[nodiscard]] constexpr reverse_iterator rbegin() noexcept
		{
			return data_.rbegin();
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
		{
			return data_.rbegin();
		}

		[[nodiscard]] constexpr reverse_iterator rend() noexcept
		{
			return data_.rend();
		}

		[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
		{
			return data_.rend();
		}

		[[nodiscard]] constexpr const_iterator cbegin() const noexcept
		{
			return data_.cbegin();
		}

		[[nodiscard]] constexpr const_iterator cend() const noexcept
		{
			return data_.cend();
		}

	private:
		container_type data_;
	};

	template <std::size_t Stride, std::size_t Size, typename Iterator>
	class vector_view
	{
	public:
		constexpr static auto stride    = Stride;
		constexpr static auto data_size = Size;

		using view_iterator = iterator::stride_iterator<Stride, Iterator>;
		using iterator_type = typename view_iterator::iterator_type;
		using value_type = typename view_iterator::value_type;
		using difference_type = typename view_iterator::difference_type;
		using reference = typename view_iterator::reference;
		using const_reference = std::add_const_t<reference>;
		using rreference = typename view_iterator::rreference;

		using vector_type = vector<value_type, data_size>;
		using size_type = typename vector_type::size_type;

		template<typename U = value_type>
		using copy_type = vector<U, data_size>;

		constexpr explicit vector_view(iterator_type iterator) noexcept : iterator_(std::move(iterator), data_size) {}

		[[nodiscard]] constexpr copy_type<> copy() const noexcept
		{
			return { utils::tuple_maker::to_tuple<data_size>(iterator_), std::make_index_sequence<data_size>{} };
		}

		[[nodiscard]] constexpr decltype(auto) operator[](size_type index) noexcept
		{
			return iterator_[index];
		}

		[[nodiscard]] constexpr decltype(auto) operator[](size_type index) const noexcept
		{
			return iterator_[index];
		}

		[[nodiscard]] constexpr reference x() noexcept
				requires(data_size >= 1)
		{
			return iterator_[0];
		}

		[[nodiscard]] constexpr const_reference x() const noexcept
				requires(data_size >= 1)
		{
			return iterator_[0];
		}

		[[nodiscard]] constexpr reference r() noexcept
				requires(data_size >= 1)
		{
			return iterator_[0];
		}

		[[nodiscard]] constexpr const_reference r() const noexcept
				requires(data_size >= 1)
		{
			return iterator_[0];
		}

		[[nodiscard]] constexpr reference s() noexcept
				requires(data_size >= 1)
		{
			return iterator_[0];
		}

		[[nodiscard]] constexpr const_reference s() const noexcept
				requires(data_size >= 1)
		{
			return iterator_[0];
		}

		[[nodiscard]] constexpr reference y() noexcept
				requires(data_size >= 2)
		{
			return iterator_[1];
		}

		[[nodiscard]] constexpr const_reference y() const noexcept
				requires(data_size >= 2)
		{
			return iterator_[1];
		}

		[[nodiscard]] constexpr reference g() noexcept
				requires(data_size >= 2)
		{
			return iterator_[1];
		}

		[[nodiscard]] constexpr const_reference g() const noexcept
				requires(data_size >= 2)
		{
			return iterator_[1];
		}

		[[nodiscard]] constexpr reference t() noexcept
				requires(data_size >= 2)
		{
			return iterator_[1];
		}

		[[nodiscard]] constexpr const_reference t() const noexcept
				requires(data_size >= 2)
		{
			return iterator_[1];
		}

		[[nodiscard]] constexpr reference z() noexcept
				requires(data_size >= 3)
		{
			return iterator_[2];
		}

		[[nodiscard]] constexpr const_reference z() const noexcept
				requires(data_size >= 3)
		{
			return iterator_[2];
		}

		[[nodiscard]] constexpr reference b() noexcept
				requires(data_size >= 3)
		{
			return iterator_[2];
		}

		[[nodiscard]] constexpr const_reference b() const noexcept
				requires(data_size >= 3)
		{
			return iterator_[2];
		}

		[[nodiscard]] constexpr reference p() noexcept
				requires(data_size >= 3)
		{
			return iterator_[2];
		}

		[[nodiscard]] constexpr const_reference p() const noexcept
				requires(data_size >= 3)
		{
			return iterator_[2];
		}

		[[nodiscard]] constexpr reference w() noexcept
				requires(data_size >= 4)
		{
			return iterator_[3];
		}

		[[nodiscard]] constexpr const_reference w() const noexcept
				requires(data_size >= 4)
		{
			return iterator_[3];
		}

		[[nodiscard]] constexpr reference a() noexcept
				requires(data_size >= 4)
		{
			return iterator_[3];
		}

		[[nodiscard]] constexpr const_reference a() const noexcept
				requires(data_size >= 4)
		{
			return iterator_[3];
		}

		[[nodiscard]] constexpr reference q() noexcept
				requires(data_size >= 4)
		{
			return iterator_[3];
		}

		[[nodiscard]] constexpr const_reference q() const noexcept
				requires(data_size >= 4)
		{
			return iterator_[3];
		}

		// stl compatible, can also be used for for-loop
		[[nodiscard]] constexpr auto begin() const noexcept
		{
			return iterator_.begin();
		}

		// stl compatible, can also be used for for-loop
		[[nodiscard]] constexpr auto end() const noexcept
		{
			return iterator_.end();
		}

	private:
		view_iterator iterator_;
	};
}