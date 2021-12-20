//
// Created by marcel on 27.10.21.
//

#ifndef FICTION_AREA_HPP
#define FICTION_AREA_HPP

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
    AreaType width{static_cast<AreaType>(Technology::cell_width)},
        height{static_cast<AreaType>(Technology::cell_height)};
    /**
     * Horizontal and vertical spacing between cells.
     */
    AreaType hspace{static_cast<AreaType>(Technology::cell_hspace)},
        vspace{static_cast<AreaType>(Technology::cell_vspace)};
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

template <typename Lyt, typename AreaType = double>
void area(const Lyt& lyt, area_params<technology<Lyt>, AreaType>& ps = {}, area_stats<AreaType>* pst = nullptr)
{
    static_assert(fiction::is_cell_level_layout_v<Lyt>, "GateLyt is not a cell-level layout");

    area_stats st{};

    st.area = (static_cast<AreaType>(lyt.x() + 1) * ps.width + static_cast<AreaType>(lyt.x()) * ps.hspace) *
              (static_cast<AreaType>(lyt.y() + 1) * ps.height + static_cast<AreaType>(lyt.y()) * ps.vspace);

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_AREA_HPP
