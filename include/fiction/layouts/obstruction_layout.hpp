//
// Created by marcel on 02.02.22.
//

#ifndef FICTION_OBSTRUCTION_LAYOUT_HPP
#define FICTION_OBSTRUCTION_LAYOUT_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/hash.hpp"

#include <memory>
#include <type_traits>
#include <utility>

#include <phmap.h>

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
template <typename Lyt, bool has_obstruction_interface =
                            std::conjunction_v<has_is_obstructed_coordinate<Lyt>, has_is_obstructed_connection<Lyt>>>
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
    struct obstruction_layout_storage
    {
        phmap::parallel_flat_hash_set<typename Lyt::coordinate> obstructed_coordinates{};

        phmap::parallel_flat_hash_set<std::pair<typename Lyt::coordinate, typename Lyt::coordinate>>
            obstructed_connections{};
    };

    using storage = std::shared_ptr<obstruction_layout_storage>;

    /**
     * Standard constructor for empty layouts.
     */
    obstruction_layout() : Lyt(), strg{std::make_shared<obstruction_layout_storage>()}
    {
        static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    }
    /**
     * Standard constructor that layers the obstruction interface onto an existing layout.
     *
     * @param lyt Existing layout that is to be extended by an obstruction interface.
     */
    explicit obstruction_layout(const Lyt& lyt) : Lyt(lyt), strg{std::make_shared<obstruction_layout_storage>()}
    {
        static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    }
    /**
     * Marks the given coordinate as obstructed.
     *
     * @param c Coordinate to obstruct.
     */
    void obstruct_coordinate(const typename Lyt::coordinate& c) noexcept
    {
        strg->obstructed_coordinates.insert(c);
    }
    /**
     * Marks the connection from coordinate src to coordinate tgt as obstructed.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     */
    void obstruct_connection(const typename Lyt::coordinate& src, const typename Lyt::coordinate& tgt) noexcept
    {
        strg->obstructed_connections.insert({src, tgt});
    }
    /**
     * Clears all obstructed coordinates that were manually marked via obstruct_coordinate.
     */
    void clear_obstructed_coordinates() noexcept
    {
        strg->obstructed_coordinates.clear();
    }
    /**
     * Clears all obstructed connections that were manually marked via obstruct_connections.
     */
    void clear_obstructed_connections() noexcept
    {
        strg->obstructed_connections.clear();
    }
    /**
     * Checks if the given coordinate is obstructed of some sort.
     *
     * @param c Coordinate to check.
     * @return True iff c is obstructed.
     */
    [[nodiscard]] bool is_obstructed_coordinate(const typename Lyt::coordinate& c) const noexcept
    {
        if (strg->obstructed_coordinates.count(c) > 0)
        {
            return true;
        }
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
    /**
     * Checks if the given coordinate-coordinate connection is obstructed of some sort.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     * @return True iff the connection from c1 to c2 is obstructed.
     */
    [[nodiscard]] bool is_obstructed_connection(const typename Lyt::coordinate& src,
                                                const typename Lyt::coordinate& tgt) const noexcept
    {
        return strg->obstructed_connections.count({src, tgt}) > 0;
    }

  private:
    storage strg;
};

template <class T>
obstruction_layout(const T&) -> obstruction_layout<T>;

}  // namespace fiction

#endif  // FICTION_OBSTRUCTION_LAYOUT_HPP
