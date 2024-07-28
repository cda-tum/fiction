//
// Created by marcel on 13.01.22.
//

#ifndef FICTION_BOUNDING_BOX_HPP
#define FICTION_BOUNDING_BOX_HPP

#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

namespace fiction
{
/**
 * A 2D bounding box object that computes a minimum-sized box around all non-empty coordinates in a given layout.
 * Layouts can be of arbitrary size and, thus, may be larger than their contained elements. Sometimes, it might be
 * necessary to know exactly which space the associated layout internals occupy. A bounding box computes coordinates
 * that span a minimum-sized rectangle that encloses all non-empty layout coordinates.
 *
 * The bounding box does not automatically updated when the layout changes. Call `update_bounding_box()` to recompute
 * it.
 *
 * @tparam Lyt Gate-level or cell-level layout type.
 */
template <typename Lyt>
class bounding_box_2d
{
  public:
    /**
     * Standard constructor that computes an initial bounding box.
     *
     * @param lyt Gate-level or cell-level layout whose bounding box is desired.
     */
    explicit bounding_box_2d(const Lyt& lyt) noexcept : layout{lyt}
    {
        static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
        update_bounding_box();
    }
    /**
     * The bounding box is not automatically updated when the layout changes. This function recomputes the bounding box.
     */
    void update_bounding_box()
    {
        min = {0, 0, 0};
        max = {0, 0, 0};

        // empty layouts don't need further computation
        if (layout.is_empty())
        {
            if constexpr (is_sidb_defect_surface_v<Lyt>)
            {
                if (layout.num_defects() == 0)
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }

        // the layout is based on SiQAD coordinates
        if constexpr (has_siqad_coord_v<Lyt>)
        {
            auto min_x_cell = std::numeric_limits<decltype(cell<Lyt>::x)>::max();
            auto max_x_cell = std::numeric_limits<decltype(cell<Lyt>::x)>::min();

            auto min_y_cell = std::numeric_limits<decltype(cell<Lyt>::y)>::max();
            auto max_y_cell = std::numeric_limits<decltype(cell<Lyt>::y)>::min();

            auto min_z_cell = 1;
            auto max_z_cell = 0;

            layout.foreach_cell(
                [&min_x_cell, &max_x_cell, &min_y_cell, &max_y_cell, &min_z_cell, &max_z_cell](const auto& c)
                {
                    if (c.x < min_x_cell)
                    {
                        min_x_cell = c.x;
                    }
                    if (c.x > max_x_cell)
                    {
                        max_x_cell = c.x;
                    }

                    if (c.y == min_y_cell && c.z < min_z_cell)
                    {
                        min_z_cell = c.z;
                    }
                    if (c.y < min_y_cell)
                    {
                        min_y_cell = c.y;
                        min_z_cell = c.z;
                    }

                    if (c.y == max_y_cell && c.z > max_z_cell)
                    {
                        max_z_cell = c.z;
                    }
                    if (c.y > max_y_cell)
                    {
                        max_y_cell = c.y;
                        max_z_cell = c.z;
                    }
                });

            const auto min_cell = coordinate<Lyt>{min_x_cell, min_y_cell, min_z_cell};
            const auto max_cell = coordinate<Lyt>{max_x_cell, max_y_cell, max_z_cell};

            min = min_cell;
            max = max_cell;

            if constexpr (is_sidb_defect_surface_v<Lyt>)
            {
                auto min_x_defect = std::numeric_limits<decltype(cell<Lyt>::x)>::max();
                auto max_x_defect = std::numeric_limits<decltype(cell<Lyt>::x)>::min();

                auto min_y_defect = std::numeric_limits<decltype(cell<Lyt>::y)>::max();
                auto max_y_defect = std::numeric_limits<decltype(cell<Lyt>::y)>::min();

                uint8_t min_z_defect = 1;
                uint8_t max_z_defect = 0;

                layout.foreach_sidb_defect(
                    [&min_x_defect, &max_x_defect, &min_y_defect, &max_y_defect, &min_z_defect,
                     &max_z_defect](const auto& defect)
                    {
                        if (defect.first.x < min_x_defect)
                        {
                            min_x_defect = defect.first.x;
                        }
                        if (defect.first.x > max_x_defect)
                        {
                            max_x_defect = defect.first.x;
                        }

                        if (defect.first.y == min_y_defect && defect.first.z < min_z_defect)
                        {
                            min_z_defect = defect.first.z;
                        }
                        if (defect.first.y < min_y_defect)
                        {
                            min_y_defect = defect.first.y;
                            min_z_defect = defect.first.z;
                        }

                        if (defect.first.y == max_y_defect && defect.first.z > max_z_defect)
                        {
                            max_z_defect = defect.first.z;
                        }
                        if (defect.first.y > max_y_defect)
                        {
                            max_y_defect = defect.first.y;
                            max_z_defect = defect.first.z;
                        }
                    });

                const auto min_defect = coordinate<Lyt>{min_x_defect, min_y_defect, min_z_defect};
                const auto max_defect = coordinate<Lyt>{max_x_defect, max_y_defect, max_z_defect};

                min = cell<Lyt>{std::min(min_cell.x, min_defect.x), std::min(min_cell.y, min_defect.y),
                                std::min(min_cell.z, min_defect.z)};
                max = cell<Lyt>{std::max(max_cell.x, max_defect.x), std::max(max_cell.y, max_defect.y),
                                std::max(max_cell.z, max_defect.z)};
            }
        }
        else
        {
            min = coordinate<Lyt>{std::numeric_limits<decltype(coordinate<Lyt>::x)>::max(),
                                  std::numeric_limits<decltype(coordinate<Lyt>::x)>::max()};

            if constexpr (is_gate_level_layout_v<Lyt>)
            {
                layout.foreach_coordinate(
                    [this](const auto& c)
                    {
                        if (!is_empty_coordinate(c))
                        {
                            if (c.x < min.x)
                            {
                                min.x = c.x;
                            }
                            if (c.y < min.y)
                            {
                                min.y = c.y;
                            }
                            if (c.x > max.x)
                            {
                                max.x = c.x;
                            }
                            if (c.y > max.y)
                            {
                                max.y = c.y;
                            }
                        }
                    });
            }

            if constexpr (is_cell_level_layout_v<Lyt>)
            {
                layout.foreach_cell(
                    [this](const auto& c)
                    {
                        if (!layout.is_empty_cell(c))
                        {
                            if (c.x < min.x)
                            {
                                min.x = c.x;
                            }
                            if (c.y < min.y)
                            {
                                min.y = c.y;
                            }
                            if (c.x > max.x)
                            {
                                max.x = c.x;
                            }
                            if (c.y > max.y)
                            {
                                max.y = c.y;
                            }
                        }
                    });
            }

            if constexpr (is_sidb_defect_surface_v<Lyt>)
            {
                auto min_x_defect = std::numeric_limits<decltype(coordinate<Lyt>::x)>::max();
                auto max_x_defect = std::numeric_limits<decltype(coordinate<Lyt>::y)>::min();

                auto min_y_defect = std::numeric_limits<decltype(coordinate<Lyt>::x)>::max();
                auto max_y_defect = std::numeric_limits<decltype(coordinate<Lyt>::y)>::min();

                layout.foreach_sidb_defect(
                    [&min_x_defect, &max_x_defect, &min_y_defect, &max_y_defect](const auto& defect)
                    {
                        if (defect.first.x < min_x_defect)
                        {
                            min_x_defect = defect.first.x;
                        }
                        if (defect.first.x > max_x_defect)
                        {
                            max_x_defect = defect.first.x;
                        }

                        if (defect.first.y < min_y_defect)
                        {
                            min_y_defect = defect.first.y;
                        }

                        if (defect.first.y > max_y_defect)
                        {
                            max_y_defect = defect.first.y;
                        }
                    });
                const auto min_defect = coordinate<Lyt>{min_x_defect, min_y_defect};
                const auto max_defect = coordinate<Lyt>{max_x_defect, max_y_defect};

                min = cell<Lyt>{std::min(min.x, min_defect.x), std::min(min.y, min_defect.y),
                                std::min(min.z, min_defect.z)};
                max = cell<Lyt>{std::max(max.x, max_defect.x), std::max(max.y, max_defect.y),
                                std::max(max.z, max_defect.z)};
            }
        }

        x_size = max.x - min.x;
        y_size = max.y - min.y;
    }
    /**
     * Returns the minimum corner of the bounding box.
     *
     * In a `cartesian_layout<offset::ucoord_t>` object, this location represents the most north-western coordinate of
     * the bounding box enclosing every non-empty coordinate.
     *
     * @return The minimum enclosing coordinate in the associated layout.
     */
    [[nodiscard]] coordinate<Lyt> get_min() const noexcept
    {
        return min;
    }
    /**
     * Returns the maximum corner of the bounding box.
     *
     * In a `cartesian_layout<offset::ucoord_t>` object, this location represents the most south-eastern coordinate of
     * the bounding box enclosing every non-empty coordinate.
     *
     * @return The maximum enclosing coordinate in the associated layout.
     */
    [[nodiscard]] coordinate<Lyt> get_max() const noexcept
    {
        return max;
    }
    /**
     * Returns the horizontal size of the bounding box in layout coordinates.
     *
     * @return Bounding box size along the x-axis.
     */
    [[nodiscard]] auto get_x_size() const noexcept
    {
        return x_size;
    }
    /**
     * Returns the vertical size of the bounding box in layout coordinates.
     *
     * @return Bounding box size along the y-axis.
     */
    [[nodiscard]] auto get_y_size() const noexcept
    {
        return y_size;
    }

  private:
    /**
     * The layout whose bounding box is being computed.
     */
    const Lyt& layout;

    /**
     * The minimum and maximum coordinates of the bounding box.
     */
    coordinate<Lyt> min{0, 0, 0}, max{0, 0, 0};

    /**
     * The horizontal size of the bounding box in layout coordinates.
     */
    decltype(min.x) x_size{};

    /**
     * The vertical size of the bounding box in layout coordinates.
     */
    decltype(min.y) y_size{};

    /**
     * Checks if a given coordinate is empty in the layout.
     *
     * @param c The coordinate to check.
     * @return True if the coordinate is empty, false otherwise.
     */
    [[nodiscard]] bool is_empty_coordinate(const coordinate<Lyt>& c) const noexcept
    {
        static_assert(has_is_empty_tile_v<Lyt> || has_is_empty_cell_v<Lyt>,
                      "Lyt does neither implement the is_empty_tile nor the is_empty_cell function");

        if constexpr (has_is_empty_tile_v<Lyt>)
        {
            return layout.is_empty_tile(c);
        }
        else if constexpr (has_is_empty_cell_v<Lyt>)
        {
            return layout.is_empty_cell(c);
        }

        return false;
    }
};

}  // namespace fiction

#pragma GCC diagnostic pop

#endif  // FICTION_BOUNDING_BOX_HPP
