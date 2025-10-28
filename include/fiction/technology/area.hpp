//
// Created by marcel on 27.10.21.
//

#ifndef FICTION_AREA_HPP
#define FICTION_AREA_HPP

#include "fiction/layouts/bounding_box.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>

#include <iostream>

namespace fiction
{

/**
 * Parameters for area computation of cell-level layouts. Default parameters are loaded from the given cell technology.
 *
 * @tparam Technology Cell technology.
 */
template <typename Technology>
struct area_params
{
    /**
     * Width of each cell.
     */
    double width{static_cast<double>(Technology::CELL_WIDTH)};
    /**
     * Height of each cell.
     */
    double height{static_cast<double>(Technology::CELL_HEIGHT)};
    /**
     * Horizontal spacing between cells.
     */
    double hspace{static_cast<double>(Technology::CELL_HSPACE)};
    /**
     * Vertical spacing between cells.
     */
    double vspace{static_cast<double>(Technology::CELL_VSPACE)};
};
/**
 * Statistics class to store area-related information including width, height, and area of a layout.
 * Additionally, it provides a method to print a formatted report of these statistics to an output stream.
 */
struct area_stats
{
    /**
     * Layout width in nm.
     */
    double width{0.0};
    /**
     * Layout height in nm.
     */
    double height{0.0};
    /**
     * Layout area in nm².
     */
    double area{0.0};
    /**
     * Prints a report of the area statistics to the given output stream.
     *
     * @param out Output stream to write the report to. Defaults to `std::cout`.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] Width  = {:.2f} nm", width) << std::endl;
        out << fmt::format("[i] Height = {:.2f} nm", height) << std::endl;
        out << fmt::format("[i] Area   = {:.2f} nm²", area) << std::endl;
    }
};

/**
 * Computes real-world area requirements in nm² of a given cell-level layout. For this purpose, each cell position in
 * the layout is assigned a vertical and horizontal size. Additionally, a spacing between cell positions in horizontal
 * and vertical direction is taken into account.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt The cell-level layout whose area is desired.
 * @param ps Area parameters.
 * @param pst Area statistics.
 * @return Area requirements in nm².
 */
template <typename Lyt>
double area(const Lyt& lyt, const area_params<technology<Lyt>>& ps = {}, area_stats* pst = nullptr) noexcept
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    area_stats st{};

    st.width  = static_cast<double>(lyt.x() + 1) * ps.width + static_cast<double>(lyt.x()) * ps.hspace;
    st.height = static_cast<double>(lyt.y() + 1) * ps.height + static_cast<double>(lyt.y()) * ps.vspace;
    st.area   = st.width * st.height;

    if (pst)
    {
        *pst = st;
    }

    return st.area;
}
/**
 * Computes real-world area requirements in nm² of the bounding box of a given cell-level layout. For this purpose, each
 * cell position in the layout is assigned a vertical and horizontal size. Additionally, a spacing between cell
 * positions in horizontal and vertical direction is taken into account.
 *
 * @tparam Lyt Cell-level layout type.
 * @param bb The bounding box of the cell-level layout whose area is desired.
 * @param ps Area parameters.
 * @param pst Area statistics.
 * @return Area requirements in nm².
 */
template <typename Lyt>
double area(const bounding_box_2d<Lyt>& bb, const area_params<technology<Lyt>>& ps = {},
            area_stats* pst = nullptr) noexcept
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    area_stats st{};

    st.width  = static_cast<double>(bb.get_x_size() + 1) * ps.width + static_cast<double>(bb.get_x_size()) * ps.hspace;
    st.height = static_cast<double>(bb.get_y_size() + 1) * ps.height + static_cast<double>(bb.get_y_size()) * ps.vspace;
    st.area   = st.width * st.height;

    if (pst)
    {
        *pst = st;
    }

    return st.area;
}

}  // namespace fiction

#endif  // FICTION_AREA_HPP
