#pragma once

#include "new_vector.hpp"
#include "new_matrix.hpp"
#include "../utils/sequence_invoker.hpp"

namespace gal::test::new_math
{
	template <typename T>
	constexpr static bool is_math_type_v =
		is_vector_v<T> || is_vector_view_v<T> ||
		is_matrix_v<T> || is_matrix_view_v<T>;

	template <typename T>
	struct math_value_type_helper
	{
		using value_type = T;
	};

	template <typename T>
		requires is_math_type_v<T>
	struct math_value_type_helper<T>
	{
		using value_type = typename T::value_type;
	};

	template <typename T>
	using math_value_type = typename math_value_type_helper<T>::value_type;

	template <typename T1, typename T2>
	struct is_math_same_size : std::false_type {};

	template <typename T1, typename T2>
		requires
		is_vector_type<T1> &&
		is_vector_type<T2> &&
		(T1::data_size == T2::data_size)
	struct is_math_same_size<T1, T2> : std::true_type {};

	template <typename T1, typename T2>
		requires
		is_matrix_type<T1> &&
		is_matrix_type<T2> &&
		(T1::column_size == T2::column_size) &&
		(T1::row_size == T2::row_size)
	struct is_math_same_size<T1, T2> : std::true_type {};

	template <typename T1, typename T2>
	constexpr static bool is_math_same_size_v = is_math_same_size<T1, T2>::value;

	/**
	 * @brief process a vector/matrix with parameters pack
	 * @tparam RetType because there is no way to get the return value type through Pred, it needs to be specified by the user (the user should ensure that RetType is compatible with the actual return type of Pred)
	 * @tparam U vector/matrix type
	 * @tparam Pred processing function type, it is required to accept two parameters (respectively the value_type of vector/matrix and parameters pack)
	 * @tparam Args parameters pack type
	 * @param scalar vector/matrix
	 * @param pred processing function
	 * @param args parameters pack
	 * @return a vector/matrix, the value_type is the type returned by pred
	*/
	template <typename RetType, typename U, typename Pred, typename... Args>
		requires
		is_math_type_v<U> &&
		(sizeof...(Args) == U::data_size)
	constexpr auto ternary_apply(
		const U& scalar,
		Pred&&   pred,
		Args&&...args
		)
	noexcept(noexcept(
		utils::sequence_invoker::ternary_invoke<U::data_size>(
															std::declval<typename U::template copy_type<RetType>&>(),
															std::declval<const U&>(),
															std::declval<decltype(std::forward<Pred>(pred))>(),
															std::declval<decltype(std::forward<Args>(args))>()...
															)
	))
	{
		typename U::template copy_type<RetType> ret{};
		utils::sequence_invoker::ternary_invoke<U::data_size>(
															ret,
															scalar,
															std::forward<Pred>(pred),
															std::forward<Args>(args)...
															);
		return ret;
	}

	/**
	 * @brief process two vectors/matrices to obtain a vector
	 * @tparam RetType because there is no way to get the return value type through Pred, it needs to be specified by the user (the user should ensure that RetType is compatible with the actual return type of Pred)
	 * @tparam U1 vector/matrices type1
	 * @tparam U2 vector/matrices type2
	 * @tparam Pred processing function type, it is required to accept two parameters (respectively the value_type of two vectors/matrices)
	 * @param scalar1 vector/matrices1
	 * @param scalar2 vector/matrices2
	 * @param pred processing function
	 * @return a vector/matrices, the value_type is the type returned by pred
	*/
	template <typename RetType, typename U1, typename U2, typename Pred>
		requires
		is_math_same_size_v<U1, U2>
	constexpr auto ternary_apply_seq(
		const U1& scalar1,
		const U2& scalar2,
		Pred&&    pred
		)
	noexcept(noexcept(
		utils::sequence_invoker::ternary_invoke_seq<U1::data_size>(
																	std::declval<typename U1::template copy_type<
																		RetType>&>(),
																	std::declval<const U1&>(),
																	std::declval<const U2&>(),
																	std::declval<decltype(std::forward<Pred>(pred))>()
																)
	))
	{
		typename U1::template copy_type<RetType> ret{};
		utils::sequence_invoker::ternary_invoke_seq<U1::data_size>(
																	ret,
																	scalar1,
																	scalar2,
																	std::forward<Pred>(pred)
																);
		return ret;
	}

	/**
	 * @brief process a vector/matrix with a duplicate arg to obtain a vector/matrix
	 * @tparam RetType because there is no way to get the return value type through Pred, it needs to be specified by the user (the user should ensure that RetType is compatible with the actual return type of Pred)
	 * @tparam U  vector/matrix type
	 * @tparam T duplicate arg type
	 * @tparam Pred processing function type, it is required to accept two parameters (respectively the value_type of vector/matrix and duplicate arg)
	 * @param scalar vector/matrix
	 * @param dup_arg duplicate arg
	 * @param pred processing function
	 * @return a vector/matrix, the value_type is the type returned by pred
	*/
	template <typename RetType, typename U, typename T, typename Pred>
		requires is_math_type_v<U>
	constexpr auto ternary_apply_dup(
		const U& scalar,
		T        dup_arg,
		Pred&&   pred
		)
	noexcept(noexcept(
		utils::sequence_invoker::ternary_invoke_dup<U::data_size>(
																std::declval<typename U::template copy_type<RetType>
																	&>(),
																std::declval<const U&>(),
																std::declval<T>(),
																std::declval<decltype(std::forward<Pred>(pred))>()
																)
	))
	{
		typename U::template copy_type<RetType> ret{};
		utils::sequence_invoker::ternary_invoke_dup<U::data_size>(
																ret,
																scalar,
																dup_arg,
																std::forward<Pred>(pred)
																);
		return ret;
	}

	/**
	 * @brief process a vector/matrix with parameters pack
	 * @tparam HasRet does the function need to return a value
	 * @tparam All the superposition method of the return value (bool), is it All(&&) or Any(||)
	 * @tparam U vector/matrix type 
	 * @tparam Pred processing function type, it is required to accept two parameters (respectively the value_type of vector/matrix and parameters pack)
	 * @tparam Args parameters pack type
	 * @param scalar vector/matrix
	 * @param pred processing function
	 * @param args parameters pack
	 * @return bool or void
	*/
	template <bool HasRet, bool All = true, typename U, typename Pred, typename ...Args>
		requires
		is_math_type_v<U> &&
		(sizeof...(Args) == U::data_size)
	constexpr auto binary_apply(
		U&       scalar,
		Pred&&   pred,
		Args&&...args
		)
	noexcept(
		noexcept(
			utils::sequence_invoker::binary_invoke<U::data_size>(
																std::declval<U&>(),
																std::declval<decltype(std::forward<Pred>(pred))>(),
																std::declval<decltype(std::forward<Args>(args))>()...
																)
		)
	)
	{
		if constexpr (HasRet)
		{
			if constexpr (All)
			{
				return utils::sequence_invoker::binary_invoke_all<U::data_size>(
																				scalar,
																				std::forward<Pred>(pred),
																				std::forward<Args>(args)...
																				);
			}
			else
			{
				return utils::sequence_invoker::binary_invoke_any<U::data_size>(
																				scalar,
																				std::forward<Pred>(pred),
																				std::forward<Args>(args)...
																				);
			}
		}
		else
		{
			utils::sequence_invoker::binary_invoke<U::data_size>(
																scalar,
																std::forward<Pred>(pred),
																std::forward<Args>(args)...
																);
		}
	}

	/**
	 * @brief process two vectors/matrices
	 * @tparam HasRet does the function need to return a value
	 * @tparam All the superposition method of the return value (bool), is it All(&&) or Any(||)
	 * @tparam U1 vector/matrix type1
	 * @tparam U2 vector/matrix type2
	 * @tparam Pred processing function type, it is required to accept two parameters (respectively the value_type of two vectors/matrices)
	 * if a return value is required, the function must return bool (or it can be converted to bool)
	 * the superposition method depends on the template parameter All
	 * @param scalar1 vector/matrix1
	 * @param scalar2 vector/matrix2
	 * @param pred processing function
	 * @return bool or void
	*/
	template <bool HasRet, bool All = true, typename U1, typename U2, typename Pred>
		requires is_math_same_size_v<U1, U2>
	// ReSharper disable once CppNotAllPathsReturnValue (actually all paths have return value)
	constexpr std::conditional_t<HasRet, bool, void> binary_apply_seq(
		U1&       scalar1,
		const U2& scalar2,
		Pred&&    pred
		)
	noexcept(
		noexcept(
			utils::sequence_invoker::binary_invoke_seq<U1::data_size>(
																	std::declval<U1&>(),
																	std::declval<const U2&>(),
																	std::declval<decltype(std::forward<Pred>(pred))>()
																	)
		)
	)
	{
		if constexpr (HasRet)
		{
			if constexpr (All)
			{
				return utils::sequence_invoker::binary_invoke_seq_all<U1::data_size>(
																					scalar1,
																					scalar2,
																					std::forward<Pred>(pred)
																					);
			}
			else
			{
				return utils::sequence_invoker::binary_invoke_seq_any<U1::data_size>(
																					scalar1,
																					scalar2,
																					std::forward<Pred>(pred)
																					);
			}
		}
		else
		{
			utils::sequence_invoker::binary_invoke_seq<U1::data_size>(
																	scalar1,
																	scalar2,
																	std::forward<Pred>(pred)
																	);
		}
	}

	template <bool HasRet, bool All = true, typename U, typename T, typename Pred>
		requires is_math_type_v<U>
	// ReSharper disable once CppNotAllPathsReturnValue (actually all paths have return value)
	constexpr std::conditional_t<HasRet, bool, void> binary_apply_dup(
		U&     scalar,
		T      dup_arg,
		Pred&& pred
		)
	noexcept(
		noexcept(
			utils::sequence_invoker::binary_invoke_dup<U::data_size>(
																	std::declval<U&>(),
																	std::declval<T>(),
																	std::declval<decltype(std::forward<Pred>(pred))>()
																	)
		)
	)
	{
		if constexpr (HasRet)
		{
			if constexpr (All)
			{
				return utils::sequence_invoker::binary_invoke_dup_all<U::data_size>(
																					scalar,
																					dup_arg,
																					std::forward<Pred>(pred)
																					);
			}
			else
			{
				return utils::sequence_invoker::binary_invoke_dup_any<U::data_size>(
																					scalar,
																					dup_arg,
																					std::forward<Pred>(pred)
																					);
			}
		}
		else
		{
			utils::sequence_invoker::binary_invoke_dup<U::data_size>(
																	scalar,
																	dup_arg,
																	std::forward<Pred>(pred)
																	);
		}
	}
}