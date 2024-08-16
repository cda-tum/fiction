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

        // Helper function to update bounding box
        auto update_min_max = [](auto& min_coord, auto& max_coord, const auto& coord)
        {
            if constexpr (has_siqad_coord_v<Lyt>)
            {
                auto coord_siqad = siqad::to_fiction_coord<cube::coord_t>(coord);
                auto min_coord_cube = siqad::to_fiction_coord<cube::coord_t>(min_coord);
                auto max_coord_cube = siqad::to_fiction_coord<cube::coord_t>(max_coord);

                min_coord_cube.x = std::min(min_coord_cube.x, coord_siqad.x);
                max_coord_cube.x = std::max(max_coord_cube.x, coord_siqad.x);
                min_coord_cube.y = std::min(min_coord_cube.y, coord_siqad.y);
                max_coord_cube.y = std::max(max_coord_cube.y, coord_siqad.y);

                min_coord = fiction::siqad::to_siqad_coord(min_coord_cube);
                max_coord = fiction::siqad::to_siqad_coord(max_coord_cube);
            }
            else
            {
                min_coord.x = std::min(min_coord.x, coord.x);
                max_coord.x = std::max(max_coord.x, coord.x);
                min_coord.y = std::min(min_coord.y, coord.y);
                max_coord.y = std::max(max_coord.y, coord.y);
            }
        };

        if constexpr (has_siqad_coord_v<Lyt>)
        {
            auto min_cell = coordinate<Lyt>{std::numeric_limits<decltype(cell<Lyt>::x)>::max(),
                                            std::numeric_limits<decltype(cell<Lyt>::y)>::max(), 1};  // Initial z = 1
            auto max_cell = coordinate<Lyt>{std::numeric_limits<decltype(cell<Lyt>::x)>::min(),
                                            std::numeric_limits<decltype(cell<Lyt>::y)>::min(), 0};  // Initial z = 0

            layout.foreach_cell([&](const auto& c) { update_min_max(min_cell, max_cell, c); });

            min = min_cell;
            max = max_cell;

            if constexpr (is_sidb_defect_surface_v<Lyt>)
            {
                // Defect handling for SiDB defect surfaces
                auto min_defect = min_cell;
                auto max_defect = max_cell;

                layout.foreach_sidb_defect([&](const auto& defect)
                                           { update_min_max(min_defect, max_defect, defect.first); });

                // Adjust min and max based on defects
                min = {std::min(min.x, min_defect.x), std::min(min.y, min_defect.y), std::min(min.z, min_defect.z)};
                max = {std::max(max.x, max_defect.x), std::max(max.y, max_defect.y), std::max(max.z, max_defect.z)};
            }
        }
        else
        {
            // Default coordinate system handling
            min = {std::numeric_limits<decltype(coordinate<Lyt>::x)>::max(),
                   std::numeric_limits<decltype(coordinate<Lyt>::y)>::max()};
            max = {std::numeric_limits<decltype(coordinate<Lyt>::x)>::min(),
                   std::numeric_limits<decltype(coordinate<Lyt>::y)>::min()};

            if constexpr (is_gate_level_layout_v<Lyt>)
            {
                layout.foreach_coordinate(
                    [&](const auto& c)
                    {
                        if (!is_empty_coordinate(c))
                        {
                            update_min_max(min, max, c);
                        }
                    });
            }

            if constexpr (is_cell_level_layout_v<Lyt>)
            {
                layout.foreach_cell(
                    [&](const auto& c)
                    {
                        if (!layout.is_empty_cell(c))
                        {
                            update_min_max(min, max, c);
                        }
                    });
            }

            if constexpr (is_sidb_defect_surface_v<Lyt>)
            {
                layout.foreach_sidb_defect([&](const auto& defect) { update_min_max(min, max, defect.first); });
            }
        }

        // Final bounding box dimensions
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
