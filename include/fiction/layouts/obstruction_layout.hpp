//
// Created by marcel on 02.02.22.
//

#ifndef FICTION_OBSTRUCTION_LAYOUT_HPP
#define FICTION_OBSTRUCTION_LAYOUT_HPP

#include "fiction/traits.hpp"

namespace fiction
{

/**
 * A layout type to layer on top of any coordinate layout. It implements a unified obstruction interface that determines
 * whether a coordinate is blocked by something. That could either be due to prior placement of cells, gates, and wires
 * or because of fabrication defects.
 *
 * Currently, this layout type supports obstruction rules for gate_level_layout and cell_level_layout.
 *
 * @tparam Lyt Any coordinate layout type that is to be extended by an obstruction interface.
 * @tparam has_obstruction_interface Automatically determines whether an obstruction interface is already present.
 */
template <typename Lyt, bool has_obstruction_interface = has_is_obstructed_v<Lyt>>
class obstruction_layout : public Lyt
{};

template <typename Lyt>
class obstruction_layout<Lyt, true> : public Lyt
{
  public:
    explicit obstruction_layout(const Lyt& lyt) : Lyt(lyt) {}
};

template <typename Lyt>
class obstruction_layout<Lyt, false> : public Lyt
{
  public:
    /**
     * Standard constructor for empty layouts.
     */
    obstruction_layout() : Lyt()
    {
        static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    }
    /**
     * Standard constructor that layers the obstruction interface onto an existing layout.
     *
     * @param lyt Existing layout that is to be extended by an obstruction interface.
     */
    explicit obstruction_layout(const Lyt& lyt) : Lyt(lyt)
    {
        static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    }
    /**
     * Checks if the given coordinate is obstructed of some sort.
     *
     * @param c Coordinate to check.
     * @return True iff c is obstructed.
     */
    bool is_obstructed(const coordinate<Lyt> c) const noexcept
    {
        if constexpr (is_gate_level_layout_v<Lyt>)
        {
            return !Lyt::is_empty_tile(c);
        }
        else if constexpr (is_cell_level_layout_v<Lyt>)
        {
            return !Lyt::is_empty_cell(c);
        }

        // more implementations go here

        return false;
    }
};

template <class T>
obstruction_layout(const T&) -> obstruction_layout<T>;

}  // namespace fiction

#endif  // FICTION_OBSTRUCTION_LAYOUT_HPP
