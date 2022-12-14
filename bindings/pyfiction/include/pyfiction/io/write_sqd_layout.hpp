//
// Created by marcel on 08.07.22.
//

#ifndef PYFICTION_WRITE_SQD_LAYOUT_HPP
#define PYFICTION_WRITE_SQD_LAYOUT_HPP

#include "pybind11/pybind11.h"

#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

namespace pyfiction
{

inline void write_sqd_layout(pybind11::module& m)
{
    using namespace pybind11::literals;

    using cell_clk_cart_lyt = fiction::cell_level_layout<
        fiction::sidb_technology,
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

    void (*write_sqd_layout_function_pointer)(const cell_clk_cart_lyt&, const std::string&) =
        &fiction::write_sqd_layout<cell_clk_cart_lyt>;

    m.def("write_sqd_layout", write_sqd_layout_function_pointer, "layout"_a, "filename"_a);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_SQD_LAYOUT_HPP
