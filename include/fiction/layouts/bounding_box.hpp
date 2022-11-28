//
// Created by marcel on 13.01.22.
//

#ifndef FICTION_BOUNDING_BOX_HPP
#define FICTION_BOUNDING_BOX_HPP

#include "fiction/traits.hpp"

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
 * The bounding box does not automatically updated when the layout changes. Call update_bounding_box() to recompute it.
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
            return;
        }

        // calculate min.x
        for (decltype(min.x) x = 0; x <= layout.x(); ++x)
        {
            bool elem_found = false;
            for (decltype(min.y) y = 0; y <= layout.y(); ++y)
            {
                if (!is_empty_coordinate({x, y}))
                {
                    elem_found = true;
                    break;
                }
            }

            min.x = x;
            if (elem_found)
            {
                break;
            }
        }

        // calculate min.y
        for (decltype(min.y) y = 0; y <= layout.y(); ++y)
        {
            bool elem_found = false;
            for (decltype(min.x) x = 0; x <= layout.x(); ++x)
            {
                if (!is_empty_coordinate({x, y}))
                {
                    elem_found = true;
                    break;
                }
            }

            min.y = y;
            if (elem_found)
            {
                break;
            }
        }

        // calculate max.x
        for (auto x = layout.x(); x >= 0; --x)
        {
            bool elem_found = false;
            for (decltype(max.y) y = 0; y <= layout.y(); ++y)
            {
                if (!is_empty_coordinate({x, y}))
                {
                    elem_found = true;
                    break;
                }
            }

            max.x = x;
            if (elem_found)
            {
                break;
            }
        }

        // calculate max.y
        for (auto y = layout.y(); y >= 0; --y)
        {
            bool elem_found = false;
            for (decltype(max.x) x = 0; x <= layout.x(); ++x)
            {
                if (!is_empty_coordinate({x, y}))
                {
                    elem_found = true;
                    break;
                }
            }

            max.y = y;
            if (elem_found)
            {
                break;
            }
        }

        x_size = max.x - min.x;
        y_size = max.y - min.y;
    }
    /**
     * Returns the minimum corner of the bounding box.
     *
     * In a cartesian_layout<offset::ucoord_t> object, this location represents the most north-western coordinate of the
     * bounding box enclosing every non-empty coordinate.
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
     * In a cartesian_layout<offset::ucoord_t> object, this location represents the most south-eastern coordinate of the
     * bounding box enclosing every non-empty coordinate.
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
    const Lyt& layout;

    coordinate<Lyt> min{0, 0, 0}, max{0, 0, 0};

    decltype(min.x) x_size{};
    decltype(min.y) y_size{};

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

#endif  // FICTION_BOUNDING_BOX_HPP

#pragma GCC diagnostic pop
