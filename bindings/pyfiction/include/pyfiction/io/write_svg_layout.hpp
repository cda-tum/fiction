//
// Created by marcel on 08.07.22.
//

#ifndef PYFICTION_WRITE_SVG_LAYOUT_HPP
#define PYFICTION_WRITE_SVG_LAYOUT_HPP

#include "pybind11/pybind11.h"

#include <fiction/io/write_svg_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

namespace pyfiction
{

inline void write_svg_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using cell_clk_cart_lyt = fiction::cell_level_layout<
        fiction::qca_technology,
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

    py::class_<fiction::write_qca_layout_svg_params>(m, "write_qca_layout_svg_params")
        .def(py::init<>())
        .def_readwrite("simple", &fiction::write_qca_layout_svg_params::simple)

        ;

    void (*write_svg_layout_function_pointer)(const cell_clk_cart_lyt&, const std::string&,
                                              fiction::write_qca_layout_svg_params) =
        &fiction::write_qca_layout_svg<cell_clk_cart_lyt>;

    m.def("write_qca_layout_svg", write_svg_layout_function_pointer, "layout"_a, "filename"_a,
          "params"_a = fiction::write_qca_layout_svg_params{});
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_SVG_LAYOUT_HPP
