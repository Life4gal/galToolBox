#pragma once

#include <array>

#include "vector.hpp"
#include "../iterator/stride_iterator.hpp"

namespace gal::test::math
{
	//===============================
	template <typename T, std::size_t Row, std::size_t Column>
	class matrix;

	template <typename T>
	struct is_matrix : std::false_type {};

	template <typename T, std::size_t Row, std::size_t Column>
	struct is_matrix<matrix<T, Row, Column>> : std::true_type {};

	template <typename T>
	constexpr static bool is_matrix_v = is_matrix<T>::value;

	template <std::size_t Stride, std::size_t Row, std::size_t Column, typename Iterator>
	class matrix_view;

	template <typename T>
	struct is_matrix_view : std::false_type {};

	template <std::size_t Stride, std::size_t Row, std::size_t Column, typename Iterator>
	struct is_matrix_view<matrix_view<Stride, Row, Column, Iterator>> : std::true_type {};

	template <typename T>
	constexpr static bool is_matrix_view_v = is_matrix_view<T>::value;

	/**
	 * @brief specialize integral and floating_point to support corresponding operations
	 * @tparam T matrix/matrix_view
	*/
	template <typename T>
	requires
		(is_matrix_v<T> || is_matrix_view_v<T>) &&
		(
			std::is_integral_v<typename math_trait<T>::value_type> ||
			std::is_floating_point_v<typename math_trait<T>::value_type>
			)
		struct math_invoker_trait<T>;
	//===============================

	/**
	 * @brief specialize the matrix to support the construction of other matrix
	 * @tparam T matrix's value_type
	 * @tparam Row matrix's row size
	 * @tparam Column matrix's column size
	*/
	template <typename T, std::size_t Row, std::size_t Column>
	struct math_trait<matrix<T, Row, Column>>
	{
		using value_type = T;
		constexpr static bool        value = true;
		constexpr static std::size_t size  = Row * Column;
	};

	/**
	 * @brief specialize the matrix_view to support the construction of other matrix
	 * @tparam Stride matrix_view's stride
	 * @tparam Row matrix's row size
	 * @tparam Column matrix's column size
	 * @tparam Iterator type of matrix_view's holding iterator
	*/
	template <std::size_t Stride, std::size_t Row, std::size_t Column, typename Iterator>
	struct math_trait<matrix_view<Stride, Row, Column, Iterator>>
	{
		using value_type = std::iter_value_t<Iterator>;
		constexpr static bool        value = true;
		constexpr static std::size_t size  = Row * Column;
	};

	template <typename T>
		requires
		(is_matrix_v<T> || is_matrix_view_v<T>) &&
		(
			std::is_integral_v<typename math_trait<T>::value_type> ||
			std::is_floating_point_v<typename math_trait<T>::value_type>
		)
	struct math_invoker_trait<T>
	{
		using value_type = typename math_trait<T>::value_type;
		
		static_assert(std::is_arithmetic_v<value_type>);

		// todo: operations
	};

	template <typename T, std::size_t Row, std::size_t Column>
	class matrix
	{
	public:
		using container_type = std::array<T, Row * Column>;
		using value_type = typename container_type::value_type;
		using size_type = typename container_type::size_type;

		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;
		using reverse_iterator = typename container_type::reverse_iterator;
		using const_reverse_iterator = typename container_type::const_reverse_iterator;

		constexpr static size_type row_size    = Column;
		constexpr static size_type column_size = Row;
		constexpr static size_type data_size   = row_size * column_size;

		using self_type = matrix<value_type, column_size, row_size>;
		using self_reference = matrix<value_type, column_size, row_size>&;
		using self_const_reference = const matrix<value_type, column_size, row_size>&;
		using self_rreference = matrix<value_type, column_size, row_size>&&;

		using row_type = vector<value_type, row_size>;
		using column_type = vector<value_type, column_size>;

		using row_view_type = typename row_type::template view_type<1, iterator>;
		using const_row_view_type = typename row_type::template const_view_type<1, const_iterator>;
		using column_view_type = typename column_type::template view_type<row_size, iterator>;
		using const_column_view_type = typename column_type::template const_view_type<row_size, const_iterator>;

		/**
		 * @brief default construction
		*/
		constexpr matrix() noexcept(std::is_nothrow_default_constructible_v<container_type>) = default;

		/**
		 * @brief tuple composition parameter construction
		 * @tparam Tuple Tuple type
		 * @tparam I index sequence
		 * @param tuple args tuple
		*/
		template <typename Tuple, size_type...I>
			requires (sizeof...(I) <= data_size)
		constexpr matrix(Tuple&& tuple, std::index_sequence<I...>)
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
		// requires((matrix_trait<std::remove_cvref_t<Args>>::size + ...) >= data_size)
		constexpr explicit matrix(Args&&...args)
		noexcept(noexcept(std::tuple_cat(to_rref_tuple(std::forward<Args>(args))...)))
			:
			// matrix(std::tuple_cat(to_rref_tuple(std::forward<Args>(args))...), std::make_index_sequence<data_size>{}) {}
			matrix(std::tuple_cat(to_rref_tuple(std::forward<Args>(args))...),
					std::make_index_sequence<std::min(data_size,
													std::tuple_size_v<decltype(
														std::tuple_cat(to_rref_tuple(std::forward<Args>(args))...))>)>
					{}) {}

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


		[[nodiscard]] constexpr reference get(size_type row, size_type column) noexcept
		{
			return data_[row * row_size + column];
		}

		[[nodiscard]] constexpr const_reference get(size_type row, size_type column) const noexcept
		{
			return data_[row * row_size + column];
		}

		[[nodiscard]] constexpr row_type get_row(size_type which_row) const noexcept
		{
			return {
				to_tuple<row_size>(data_.cbegin() + which_row * row_size), std::make_index_sequence<row_size>{}
			};
		}

		[[nodiscard]] constexpr row_view_type get_row_view(size_type which_row) noexcept
		{
			return row_view_type{data_.begin() + which_row * row_size};
		}

		[[nodiscard]] constexpr const_row_view_type get_row_view(size_type which_row) const noexcept
		{
			return const_row_view_type{data_.cbegin() + which_row * row_size};
		}

		[[nodiscard]] constexpr column_type get_column(size_type which_column) const noexcept
		{
			return {
				to_tuple<column_size>(data_.cbegin() + which_column, row_size), std::make_index_sequence<column_size>{}
			};
		}

		[[nodiscard]] constexpr column_view_type get_column_view(size_type which_column) noexcept
		{
			return column_view_type{data_.begin() + which_column};
		}

		[[nodiscard]] constexpr const_column_view_type get_column_view(size_type which_column) const noexcept
		{
			return const_column_view_type{data_.cbegin() + which_column};
		}

	private:
		container_type data_;
	};
}