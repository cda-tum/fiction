//
// Created by marcel on 28.06.21.
//

#ifndef FICTION_ARRAY_UTILS_HPP
#define FICTION_ARRAY_UTILS_HPP

#include <array>
#include <utility>

namespace fiction
{

namespace detail
{
/**
 * From https://stackoverflow.com/questions/57756557/initializing-a-stdarray-with-a-constant-value
 */
template <typename T, std::size_t... Is>
constexpr std::array<T, sizeof...(Is)> create_array(T value, [[maybe_unused]] std::index_sequence<Is...>)
{
    return {{(static_cast<void>(Is), value)...}};
}

/**
 * Based on https://stackoverflow.com/questions/57756557/initializing-a-stdarray-with-a-constant-value
 */
template <typename ElementType, typename ArrayType, std::size_t... Is>
constexpr auto convert_array(const ArrayType& a,
                             [[maybe_unused]] std::index_sequence<Is...>) -> std::array<ElementType, sizeof...(Is)>
{
    return {{static_cast<ElementType>(a[Is])...}};
}

/**
 * Based on https://stackoverflow.com/questions/57756557/initializing-a-stdarray-with-a-constant-value
 */
template <typename ElementType, typename T, std::size_t N, std::size_t M, std::size_t... Is>
constexpr auto
convert_array_of_arrays(const std::array<std::array<T, M>, N>& a,
                        [[maybe_unused]] std::index_sequence<Is...>) -> std::array<std::array<ElementType, M>, N>
{
    return {{detail::convert_array<ElementType>(a[Is], std::make_index_sequence<M>())...}};
}

}  // namespace detail

/**
 * Creates an array of size `N` and initializes its fields with value of type `T` at compile time.
 *
 * @tparam N Array size.
 * @tparam T Type of array.
 * @param value Initial value to each field.
 * @return An object of type `std::array<T, N>` that is initialized with `N` copies of value.
 */
template <std::size_t N, typename T>
constexpr auto create_array(const T& value)
{
    return detail::create_array(value, std::make_index_sequence<N>());
}
/**
 * Converts an array of size `N` and type `T` to an array of size `N` and type `ElementType` by applying `static_cast`
 * at compile time.
 *
 * @tparam ElementType New type of each element in the returned array.
 * @tparam T Element type of the input array.
 * @tparam N Size of both the input and the output array.
 * @param a The array to be converted.
 * @return An object of type `std::array<ElementType, N>` that was created by casting each element in `a` to
 * `ElementType`.
 */
template <typename ElementType, typename T, std::size_t N>
constexpr auto convert_array(const std::array<T, N>& a)
{
    return detail::convert_array<ElementType>(a, std::make_index_sequence<N>());
}
/**
 * Same as `convert_array` but for 2D arrays.
 *
 * @tparam ElementType New type of each element in the returned array.
 * @tparam T Element type of the input array.
 * @tparam N Outer size of both the input and the output array.
 * @tparam M Inner size of both the input and the output array.
 * @param a The array to be converted.
 * @return An object of type `std::array<std::array<ElementType, M>, N>` that was created by casting each element in `a`
 * to `ElementType` using `static_cast`.
 */
template <typename ElementType, typename T, std::size_t N, std::size_t M>
constexpr auto convert_array_of_arrays(const std::array<std::array<T, M>, N>& a)
{
    return detail::convert_array_of_arrays<ElementType>(a, std::make_index_sequence<N>());
}

}  // namespace fiction

#endif  // FICTION_ARRAY_UTILS_HPP
