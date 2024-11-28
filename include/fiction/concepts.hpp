//
// Created by marcel on 28.11.24.
//

#ifndef FICTION_CONCEPTS_HPP
#define FICTION_CONCEPTS_HPP

#include "fiction/traits.hpp"

#include <concepts>
#include <cstdlib>
#include <type_traits>

namespace fiction
{

template <typename Lyt>
concept has_north_c = requires(Lyt layout, coordinate<Lyt> coord) {
    { layout.north(coord) };
};

template <typename Lyt>
concept has_east_c = requires(Lyt layout, coordinate<Lyt> coord) {
    { layout.east(coord) };
};

template <typename Lyt>
concept has_south_c = requires(Lyt layout, coordinate<Lyt> coord) {
    { layout.south(coord) };
};

template <typename Lyt>
concept has_west_c = requires(Lyt layout, coordinate<Lyt> coord) {
    { layout.west(coord) };
};

template <typename Lyt>
concept has_cardinal_operations_c = has_north_c<Lyt> && has_east_c<Lyt> && has_south_c<Lyt> && has_west_c<Lyt>;

template <typename Lyt>
concept has_above_c = requires(Lyt layout, coordinate<Lyt> coord) {
    { layout.above(coord) };
};

template <typename Lyt>
concept has_below_c = requires(Lyt layout, coordinate<Lyt> coord) {
    { layout.below(coord) };
};

template <typename Lyt>
concept has_elevation_operations_c = has_above_c<Lyt> && has_below_c<Lyt>;

/**
 * Concept for a coordinate layout.
 */
template <typename Lyt>
concept coordinate_layout_c = requires(Lyt coord_layout) {
    // Type requirements
    typename Lyt::base_type;
    typename Lyt::storage;
    typename aspect_ratio<Lyt>;
    typename coordinate<Lyt>;

    // Static member requirements
    { Lyt::max_fanin_size } -> std::convertible_to<std::size_t>;
    { Lyt::min_fanin_size } -> std::convertible_to<std::size_t>;

    // Member function requirements
    { coord_layout.x() } -> std::same_as<decltype(coordinate<Lyt>::x)>;
    { coord_layout.y() } -> std::same_as<decltype(coordinate<Lyt>::y)>;
    { coord_layout.z() } -> std::same_as<decltype(coordinate<Lyt>::z)>;
    { coord_layout.area() } -> std::convertible_to<double>;

    // Aspect ratio constructibility
    requires std::is_constructible_v<aspect_ratio<Lyt>, coordinate<Lyt>>;
} && has_cardinal_operations_c<Lyt> && has_elevation_operations_c<Lyt>;

}  // namespace fiction

#endif  // FICTION_CONCEPTS_HPP
