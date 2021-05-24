//
// Created by marcel on 24.05.21.
//

#ifndef FICTION_TRAITS_HPP
#define FICTION_TRAITS_HPP

#include <mockturtle/traits.hpp>

namespace fiction
{

#pragma region has_is_wire
template <class Ntk, class = void>
struct has_is_wire : std::false_type
{};

template <class Ntk>
struct has_is_wire<Ntk, std::void_t<decltype(std::declval<Ntk>().is_wire(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_wire_v = has_is_wire<Ntk>::value;
#pragma endregion

}  // namespace fiction

#endif  // FICTION_TRAITS_HPP
