/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Compile-time network extensions to the mockturtle trait system.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include <mockturtle/traits.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

namespace fiction
{

/**
 * mockturtle extensions
 */

#pragma region has_is_po
template <class Ntk, class = void>
struct has_is_po : std::false_type
{};

template <class Ntk>
struct has_is_po<Ntk, std::void_t<decltype(std::declval<Ntk>().is_po(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_po_v = has_is_po<Ntk>::value;
#pragma endregion

#pragma region has_is_buf
template <class Ntk, class = void>
struct has_is_buf : std::false_type
{};

template <class Ntk>
struct has_is_buf<Ntk, std::void_t<decltype(std::declval<Ntk>().is_buf(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_buf_v = has_is_buf<Ntk>::value;
#pragma endregion

#pragma region has_is_inv
template <class Ntk, class = void>
struct has_is_inv : std::false_type
{};

template <class Ntk>
struct has_is_inv<Ntk, std::void_t<decltype(std::declval<Ntk>().is_inv(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_inv_v = has_is_inv<Ntk>::value;
#pragma endregion

#pragma region has_is_fanout
template <class Ntk, class = void>
struct has_is_fanout : std::false_type
{};

template <class Ntk>
struct has_is_fanout<Ntk, std::void_t<decltype(std::declval<Ntk>().is_fanout(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_fanout_v = has_is_fanout<Ntk>::value;
#pragma endregion

#pragma region has_is_nand
template <class Ntk, class = void>
struct has_is_nand : std::false_type
{};

template <class Ntk>
struct has_is_nand<Ntk, std::void_t<decltype(std::declval<Ntk>().is_nand(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_nand_v = has_is_nand<Ntk>::value;
#pragma endregion

#pragma region has_is_nor
template <class Ntk, class = void>
struct has_is_nor : std::false_type
{};

template <class Ntk>
struct has_is_nor<Ntk, std::void_t<decltype(std::declval<Ntk>().is_nor(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_nor_v = has_is_nor<Ntk>::value;
#pragma endregion

#pragma region has_is_xnor
template <class Ntk, class = void>
struct has_is_xnor : std::false_type
{};

template <class Ntk>
struct has_is_xnor<Ntk, std::void_t<decltype(std::declval<Ntk>().is_xnor(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_xnor_v = has_is_xnor<Ntk>::value;
#pragma endregion

#pragma region has_is_le
template <class Ntk, class = void>
struct has_is_le : std::false_type
{};

template <class Ntk>
struct has_is_le<Ntk, std::void_t<decltype(std::declval<Ntk>().is_le(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_le_v = has_is_le<Ntk>::value;
#pragma endregion

#pragma region has_is_ge
template <class Ntk, class = void>
struct has_is_ge : std::false_type
{};

template <class Ntk>
struct has_is_ge<Ntk, std::void_t<decltype(std::declval<Ntk>().is_ge(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_ge_v = has_is_ge<Ntk>::value;
#pragma endregion

#pragma region has_is_gt
template <class Ntk, class = void>
struct has_is_gt : std::false_type
{};

template <class Ntk>
struct has_is_gt<Ntk, std::void_t<decltype(std::declval<Ntk>().is_gt(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_gt_v = has_is_gt<Ntk>::value;
#pragma endregion

#pragma region has_is_lt
template <class Ntk, class = void>
struct has_is_lt : std::false_type
{};

template <class Ntk>
struct has_is_lt<Ntk, std::void_t<decltype(std::declval<Ntk>().is_lt(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_lt_v = has_is_lt<Ntk>::value;
#pragma endregion

#pragma region has_is_and3
template <class Ntk, class = void>
struct has_is_and3 : std::false_type
{};

template <class Ntk>
struct has_is_and3<Ntk, std::void_t<decltype(std::declval<Ntk>().is_and3(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_and3_v = has_is_and3<Ntk>::value;
#pragma endregion

#pragma region has_is_xor_and
template <class Ntk, class = void>
struct has_is_xor_and : std::false_type
{};

template <class Ntk>
struct has_is_xor_and<Ntk, std::void_t<decltype(std::declval<Ntk>().is_xor_and(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_xor_and_v = has_is_xor_and<Ntk>::value;
#pragma endregion

#pragma region has_is_or_and
template <class Ntk, class = void>
struct has_is_or_and : std::false_type
{};

template <class Ntk>
struct has_is_or_and<Ntk, std::void_t<decltype(std::declval<Ntk>().is_or_and(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_or_and_v = has_is_or_and<Ntk>::value;
#pragma endregion

#pragma region has_is_onehot
template <class Ntk, class = void>
struct has_is_onehot : std::false_type
{};

template <class Ntk>
struct has_is_onehot<Ntk, std::void_t<decltype(std::declval<Ntk>().is_onehot(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_onehot_v = has_is_onehot<Ntk>::value;
#pragma endregion

#pragma region has_is_gamble
template <class Ntk, class = void>
struct has_is_gamble : std::false_type
{};

template <class Ntk>
struct has_is_gamble<Ntk, std::void_t<decltype(std::declval<Ntk>().is_gamble(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_gamble_v = has_is_gamble<Ntk>::value;
#pragma endregion

#pragma region has_create_dot
template <class Ntk, class = void>
struct has_create_dot : std::false_type
{};

template <class Ntk>
struct has_create_dot<Ntk, std::void_t<decltype(std::declval<Ntk>().create_dot(
                               std::declval<mockturtle::signal<Ntk>>(), std::declval<mockturtle::signal<Ntk>>(),
                               std::declval<mockturtle::signal<Ntk>>()))>> : std::true_type
{};

template <class Ntk>
inline constexpr bool has_create_dot_v = has_create_dot<Ntk>::value;
#pragma endregion

#pragma region has_is_dot
template <class Ntk, class = void>
struct has_is_dot : std::false_type
{};

template <class Ntk>
struct has_is_dot<Ntk, std::void_t<decltype(std::declval<Ntk>().is_dot(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_dot_v = has_is_dot<Ntk>::value;
#pragma endregion

#pragma region has_is_mux
template <class Ntk, class = void>
struct has_is_mux : std::false_type
{};

template <class Ntk>
struct has_is_mux<Ntk, std::void_t<decltype(std::declval<Ntk>().is_mux(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_mux_v = has_is_mux<Ntk>::value;
#pragma endregion

#pragma region has_is_and_xor
template <class Ntk, class = void>
struct has_is_and_xor : std::false_type
{};

template <class Ntk>
struct has_is_and_xor<Ntk, std::void_t<decltype(std::declval<Ntk>().is_and_xor(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_and_xor_v = has_is_and_xor<Ntk>::value;
#pragma endregion

#pragma region has_foreach_pi
template <class Ntk, class = void>
struct has_foreach_real_pi : std::false_type
{};

template <class Ntk>
struct has_foreach_real_pi<Ntk, std::void_t<decltype(std::declval<Ntk>().foreach_real_pi(
                                    std::declval<void(mockturtle::node<Ntk>, uint32_t)>()))>> : std::true_type
{};

template <class Ntk>
inline constexpr bool has_foreach_real_pi_v = has_foreach_real_pi<Ntk>::value;
#pragma endregion

#pragma region has_index_to_node
template <class Ntk, class = void>
struct has_get_real_pi : std::false_type
{};

template <class Ntk>
struct has_get_real_pi<Ntk, std::void_t<decltype(std::declval<Ntk>().get_real_pi(mockturtle::node<Ntk>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_get_real_pi_v = has_get_real_pi<Ntk>::value;
#pragma endregion

#pragma region has_num_real_pis
template <class Ntk, class = void>
struct has_num_real_pis : std::false_type
{};

template <class Ntk>
struct has_num_real_pis<Ntk, std::void_t<decltype(std::declval<Ntk>().num_real_pis())>> : std::true_type
{};

template <class Ntk>
inline constexpr bool has_num_real_pis_v = has_num_real_pis<Ntk>::value;
#pragma endregion

#pragma region has_num_virtual_pis
template <class Ntk, class = void>
struct has_num_virtual_pis : std::false_type
{};

template <class Ntk>
struct has_num_virtual_pis<Ntk, std::void_t<decltype(std::declval<Ntk>().num_virtual_pis())>> : std::true_type
{};

template <class Ntk>
inline constexpr bool has_num_virtual_pis_v = has_num_virtual_pis<Ntk>::value;
#pragma endregion

#pragma region has_is_virtual_pi
template <class Ntk, class = void>
struct has_is_virtual_pi : std::false_type
{};

template <class Ntk>
struct has_is_virtual_pi<
    Ntk, std::void_t<decltype(std::declval<Ntk>().is_virtual_pi(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_virtual_pi_v = has_is_virtual_pi<Ntk>::value;
#pragma endregion

#pragma region has_is_real_pi
template <class Ntk, class = void>
struct has_is_real_pi : std::false_type
{};

template <class Ntk>
struct has_is_real_pi<Ntk, std::void_t<decltype(std::declval<Ntk>().is_real_pi(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_real_pi_v = has_is_real_pi<Ntk>::value;
#pragma endregion

#pragma region is_virtual_network_type
template <class Ntk, class = void>
struct is_virtual_network_type : std::false_type
{};

template <class Ntk>
struct is_virtual_network_type<
    Ntk, std::enable_if_t<Ntk::is_virtual_network_type, std::void_t<decltype(Ntk::is_virtual_network_type)>>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool is_virtual_network_type_v = is_virtual_network_type<Ntk>::value;
#pragma endregion

#pragma region has_update_ranks
template <class Ntk, class = void>
struct has_update_ranks : std::false_type
{};

template <class Ntk>
struct has_update_ranks<Ntk, std::void_t<decltype(std::declval<Ntk>().update_ranks())>> : std::true_type
{};

template <class Ntk>
inline constexpr bool has_update_ranks_v = has_update_ranks<Ntk>::value;
#pragma endregion

}  // namespace fiction
