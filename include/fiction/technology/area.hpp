//
// Created by marcel on 27.10.21.
//

#ifndef FICTION_AREA_HPP
#define FICTION_AREA_HPP

#include "fiction/layouts/bounding_box.hpp"
#include "fiction/traits.hpp"

#include <cstdint>
#include <ostream>

namespace fiction
{

template <typename Technology, typename AreaType = double>
struct area_params
{
    /**
     * Width and height of each cell.
     */
    AreaType width{static_cast<AreaType>(Technology::CELL_WIDTH)},
        height{static_cast<AreaType>(Technology::CELL_HEIGHT)};
    /**
     * Horizontal and vertical spacing between cells.
     */
    AreaType hspace{static_cast<AreaType>(Technology::CELL_HSPACE)},
        vspace{static_cast<AreaType>(Technology::CELL_VSPACE)};
};

template <typename AreaType = double>
struct area_stats
{
    /**
     * Area requirements in nm².
     */
    AreaType area{0.0};

    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] {} nm²", area) << std::endl;
    }
};

/**
 * Computes real-world area requirements in nm² of a given cell-level layout. For this purpose, each cell position in
 * the layout is assigned a vertical and horizontal size. Additionally, a spacing between cell positions in horizontal
 * and vertical direction is taken into account.
 *
 * @tparam Lyt Cell-level layout type.
 * @tparam AreaType Type for area representation.
 * @param lyt The cell-level layout whose area is desired.
 * @param ps Area parameters.
 * @param pst Area statistics.
 * @return Area requirements in nm².
 */
template <typename Lyt, typename AreaType = double>
AreaType area(const Lyt& lyt, area_params<technology<Lyt>, AreaType>& ps = {},
              area_stats<AreaType>* pst = nullptr) noexcept
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    area_stats st{};

    st.area = (static_cast<AreaType>(lyt.x() + 1) * ps.width + static_cast<AreaType>(lyt.x()) * ps.hspace) *
              (static_cast<AreaType>(lyt.y() + 1) * ps.height + static_cast<AreaType>(lyt.y()) * ps.vspace);

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
 * @tparam AreaType Type for area representation.
 * @param bb The bounding box of the cell-level layout whose area is desired.
 * @param ps Area parameters.
 * @param pst Area statistics.
 * @return Area requirements in nm².
 */
template <typename Lyt, typename AreaType = double>
AreaType area(const bounding_box_2d<Lyt>& bb, area_params<technology<Lyt>, AreaType>& ps = {},
              area_stats<AreaType>* pst = nullptr) noexcept
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    area_stats st{};

    st.area =
        (static_cast<AreaType>(bb.get_x_size() + 1) * ps.width + static_cast<AreaType>(bb.get_x_size()) * ps.hspace) *
        (static_cast<AreaType>(bb.get_y_size() + 1) * ps.height + static_cast<AreaType>(bb.get_y_size()) * ps.vspace);

    if (pst)
    {
        *pst = st;
    }

    return st.area;
}

}  // namespace fiction

#endif  // FICTION_AREA_HPP
