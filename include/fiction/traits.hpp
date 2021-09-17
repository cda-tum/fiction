//
// Created by marcel on 24.05.21.
//

#ifndef FICTION_TRAITS_HPP
#define FICTION_TRAITS_HPP

#include <mockturtle/traits.hpp>

#include <cstdint>
#include <string>

namespace fiction
{

/**
 * Coordinate layouts
 */

template <typename Lyt>
using aspect_ratio = typename Lyt::aspect_ratio;

template <typename Lyt>
using coordinate = typename Lyt::coordinate;

#pragma region has_north
template <class Lyt, class = void>
struct has_north : std::false_type
{};

template <class Lyt>
struct has_north<Lyt, std::void_t<decltype(std::declval<Lyt>().north(std::declval<coordinate<Lyt>>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_north_v = has_north<Lyt>::value;
#pragma endregion

#pragma region has_east
template <class Lyt, class = void>
struct has_east : std::false_type
{};

template <class Lyt>
struct has_east<Lyt, std::void_t<decltype(std::declval<Lyt>().east(std::declval<coordinate<Lyt>>()))>> : std::true_type
{};

template <class Lyt>
inline constexpr bool has_east_v = has_east<Lyt>::value;
#pragma endregion

#pragma region has_south
template <class Lyt, class = void>
struct has_south : std::false_type
{};

template <class Lyt>
struct has_south<Lyt, std::void_t<decltype(std::declval<Lyt>().south(std::declval<coordinate<Lyt>>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_south_v = has_south<Lyt>::value;
#pragma endregion

#pragma region has_west
template <class Lyt, class = void>
struct has_west : std::false_type
{};

template <class Lyt>
struct has_west<Lyt, std::void_t<decltype(std::declval<Lyt>().west(std::declval<coordinate<Lyt>>()))>> : std::true_type
{};

template <class Lyt>
inline constexpr bool has_west_v = has_west<Lyt>::value;
#pragma endregion

#pragma region has_cardinal_operations
template <class Lyt, class = void>
struct has_cardinal_operations : std::false_type
{};

template <class Lyt>
struct has_cardinal_operations<
    Lyt, std::enable_if_t<std::conjunction_v<has_north<Lyt>, has_east<Lyt>, has_south<Lyt>, has_west<Lyt>>>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_cardinal_operations_v = has_cardinal_operations<Lyt>::value;
#pragma endregion

#pragma region has_above
template <class Lyt, class = void>
struct has_above : std::false_type
{};

template <class Lyt>
struct has_above<Lyt, std::void_t<decltype(std::declval<Lyt>().above(std::declval<coordinate<Lyt>>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_above_v = has_above<Lyt>::value;
#pragma endregion

#pragma region has_below
template <class Lyt, class = void>
struct has_below : std::false_type
{};

template <class Lyt>
struct has_below<Lyt, std::void_t<decltype(std::declval<Lyt>().below(std::declval<coordinate<Lyt>>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_below_v = has_below<Lyt>::value;
#pragma endregion

#pragma region has_elevation_operations
template <class Lyt, class = void>
struct has_elevation_operations : std::false_type
{};

template <class Lyt>
struct has_elevation_operations<Lyt, std::enable_if_t<std::conjunction_v<has_above<Lyt>, has_below<Lyt>>>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_elevation_operations_v = has_elevation_operations<Lyt>::value;
#pragma endregion

#pragma region is_coordinate_layout
template <class Ntk, class = void>
struct is_coordinate_layout : std::false_type
{};

template <class Lyt>
struct is_coordinate_layout<
    Lyt,
    std::enable_if_t<
        std::conjunction_v<std::is_constructible<aspect_ratio<Lyt>, coordinate<Lyt>>, has_cardinal_operations<Lyt>,
                           has_elevation_operations<Lyt>>,
        std::void_t<typename Lyt::base_type, aspect_ratio<Lyt>, coordinate<Lyt>, typename Lyt::storage,
                    decltype(Lyt::max_fanin_size), decltype(Lyt::min_fanin_size), decltype(std::declval<Lyt>().x()),
                    decltype(std::declval<Lyt>().y()), decltype(std::declval<Lyt>().z()),
                    decltype(std::declval<Lyt>().area()), decltype(std::declval<Lyt>().resize(aspect_ratio<Lyt>()))>>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool is_coordinate_layout_v = is_coordinate_layout<Lyt>::value;
#pragma endregion

#pragma region has_foreach_coordinate
template <class Lyt, class = void>
struct has_foreach_coordinate : std::false_type
{};

template <class Lyt>
struct has_foreach_coordinate<
    Lyt, std::void_t<decltype(std::declval<Lyt>().foreach_coordinate(std::declval<void(coordinate<Lyt>, uint32_t)>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_foreach_coordinate_v = has_foreach_coordinate<Lyt>::value;
#pragma endregion

#pragma region has_foreach_adjacent_coordinate
template <class Lyt, class = void>
struct has_foreach_adjacent_coordinate : std::false_type
{};

template <class Lyt>
struct has_foreach_adjacent_coordinate<
    Lyt, std::void_t<decltype(std::declval<Lyt>().foreach_adjacent_coordinate(
             std::declval<coordinate<Lyt>>(), std::declval<void(coordinate<Lyt>, uint32_t)>()))>> : std::true_type
{};

template <class Lyt>
inline constexpr bool has_foreach_adjacent_coordinate_v = has_foreach_adjacent_coordinate<Lyt>::value;
#pragma endregion

/**
 * Tile-based layouts
 */

template <typename Lyt>
using tile = typename Lyt::tile;

#pragma region is_tile_based_layout
template <class Ntk, class = void>
struct is_tile_based_layout : std::false_type
{};

template <class Lyt>
struct is_tile_based_layout<Lyt,
                            std::enable_if_t<is_coordinate_layout_v<Lyt>,
                                             std::void_t<typename Lyt::base_type, tile<Lyt>, typename Lyt::storage>>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool is_tile_based_layout_v = is_tile_based_layout<Lyt>::value;
#pragma endregion

#pragma region has_foreach_tile
template <class Lyt, class = void>
struct has_foreach_tile : std::false_type
{};

template <class Lyt>
struct has_foreach_tile<
    Lyt, std::void_t<decltype(std::declval<Lyt>().foreach_tile(std::declval<void(tile<Lyt>, uint32_t)>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_foreach_tile_v = has_foreach_tile<Lyt>::value;
#pragma endregion

#pragma region has_foreach_adjacent_tile
template <class Lyt, class = void>
struct has_foreach_adjacent_tile : std::false_type
{};

template <class Lyt>
struct has_foreach_adjacent_tile<Lyt, std::void_t<decltype(std::declval<Lyt>().foreach_adjacent_tile(
                                          std::declval<tile<Lyt>>(), std::declval<void(tile<Lyt>, uint32_t)>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_foreach_adjacent_tile_v = has_foreach_adjacent_tile<Lyt>::value;
#pragma endregion

/**
 * Clocked layouts
 */

template <typename Lyt>
using clock_zone = typename Lyt::clock_zone;

#pragma region is_clocked_layout
template <class Ntk, class = void>
struct is_clocked_layout : std::false_type
{};

template <class Lyt>
struct is_clocked_layout<
    Lyt, std::enable_if_t<is_coordinate_layout_v<Lyt>,
                          std::void_t<typename Lyt::base_type, clock_zone<Lyt>, typename Lyt::clocking_scheme_t,
                                      typename Lyt::clock_number_t, typename Lyt::degree_t, typename Lyt::storage,
                                      decltype(std::declval<Lyt>().get_clock_number(clock_zone<Lyt>())),
                                      decltype(std::declval<Lyt>().num_clocks()),
                                      decltype(std::declval<Lyt>().is_regularly_clocked()),
                                      decltype(std::declval<Lyt>().is_clocking_scheme(std::string()))>>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool is_clocked_layout_v = is_clocked_layout<Lyt>::value;
#pragma endregion

#pragma region has_is_incoming_clocked
template <class Lyt, class = void>
struct has_is_incoming_clocked : std::false_type
{};

template <class Lyt>
struct has_is_incoming_clocked<Lyt, std::void_t<decltype(std::declval<Lyt>().is_incoming_clocked(
                                        std::declval<clock_zone<Lyt>>(), std::declval<clock_zone<Lyt>>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_is_incoming_clocked_v = has_is_incoming_clocked<Lyt>::value;
#pragma endregion

#pragma region has_is_outgoing_clocked
template <class Lyt, class = void>
struct has_is_outgoing_clocked : std::false_type
{};

template <class Lyt>
struct has_is_outgoing_clocked<Lyt, std::void_t<decltype(std::declval<Lyt>().is_outgoing_clocked(
                                        std::declval<clock_zone<Lyt>>(), std::declval<clock_zone<Lyt>>()))>>
        : std::true_type
{};

template <class Lyt>
inline constexpr bool has_is_outgoing_clocked_v = has_is_outgoing_clocked<Lyt>::value;
#pragma endregion

#pragma region has_synchronization_elements
template <class Lyt, class = void>
struct has_synchronization_elements : std::false_type
{};

template <class Lyt>
struct has_synchronization_elements<
    Lyt, std::enable_if_t<
             is_clocked_layout_v<Lyt>,
             std::void_t<typename Lyt::sync_elem_t, typename Lyt::base_type, typename Lyt::storage,
                         decltype(std::declval<Lyt>().assign_synchronization_element(
                             std::declval<clock_zone<Lyt>>(), std::declval<typename Lyt::sync_elem_t>())),
                         decltype(std::declval<Lyt>().is_synchronization_element(std::declval<clock_zone<Lyt>>())),
                         decltype(std::declval<Lyt>().get_synchronization_element(std::declval<clock_zone<Lyt>>())),
                         decltype(std::declval<Lyt>().num_se())>>> : std::true_type
{};

template <class Lyt>
inline constexpr bool has_synchronization_elements_v = has_synchronization_elements<Lyt>::value;
#pragma endregion

/**
 * Cell-level layouts
 */

template <typename Lyt>
using cell = typename Lyt::cell;

template <typename Lyt>
using technology = typename Lyt::technology;

#pragma region is_cell_level_layout
template <class Ntk, class = void>
struct is_cell_level_layout : std::false_type
{};

template <class Lyt>
struct is_cell_level_layout<
    Lyt,
    std::enable_if_t<is_clocked_layout_v<Lyt>,
                     std::void_t<typename Lyt::base_type, cell<Lyt>, typename Lyt::cell_type, typename Lyt::cell_mode,
                                 technology<Lyt>, typename Lyt::storage,
                                 decltype(std::declval<Lyt>().assign_cell_type(cell<Lyt>(), typename Lyt::cell_type())),
                                 decltype(std::declval<Lyt>().get_cell_type(cell<Lyt>())),
                                 decltype(std::declval<Lyt>().is_empty_cell(cell<Lyt>())),
                                 decltype(std::declval<Lyt>().assign_cell_mode(cell<Lyt>(), typename Lyt::cell_mode())),
                                 decltype(std::declval<Lyt>().get_cell_mode(cell<Lyt>())),
                                 decltype(std::declval<Lyt>().assign_cell_name(cell<Lyt>(), std::string())),
                                 decltype(std::declval<Lyt>().get_cell_name(cell<Lyt>()))>>> : std::true_type
{};

template <class Lyt>
inline constexpr bool is_cell_level_layout_v = is_cell_level_layout<Lyt>::value;
#pragma endregion

#pragma region has_set_layout_name
template <class Ntk, class = void>
struct has_set_layout_name : std::false_type
{};

template <class Ntk>
struct has_set_layout_name<Ntk, std::void_t<decltype(std::declval<Ntk>().set_layout_name(std::string()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_set_layout_name_v = has_set_layout_name<Ntk>::value;
#pragma endregion

#pragma region has_get_layout_name
template <class Ntk, class = void>
struct has_get_layout_name : std::false_type
{};

template <class Ntk>
struct has_get_layout_name<Ntk, std::void_t<decltype(std::declval<Ntk>().get_layout_name())>> : std::true_type
{};

template <class Ntk>
inline constexpr bool has_get_layout_name_v = has_get_layout_name<Ntk>::value;
#pragma endregion

/**
 * Gate-level layouts
 */

#pragma region is_gate_level_layout
template <class Ntk, class = void>
struct is_gate_level_layout : std::false_type
{};

template <class Lyt>
struct is_gate_level_layout<
    Lyt, std::enable_if_t<std::conjunction_v<is_clocked_layout<Lyt>, mockturtle::is_network_type<Lyt>>,
                          std::void_t<typename Lyt::base_type, tile<Lyt>, typename Lyt::storage>>> : std::true_type
{};

template <class Lyt>
inline constexpr bool is_gate_level_layout_v = is_gate_level_layout<Lyt>::value;
#pragma endregion

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

}  // namespace fiction

#endif  // FICTION_TRAITS_HPP
