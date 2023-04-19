//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_AREA_HPP
#define PYFICTION_AREA_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/area.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void area(pybind11::module& m)
{
    using namespace pybind11::literals;

    using tech = fiction::technology<Lyt>;

    m.def(
        "area",
        [](const Lyt& lyt, const double width = tech::CELL_WIDTH, const double height = tech::CELL_HEIGHT,
           const double hspace = tech::CELL_HSPACE, const double vspace = tech::CELL_VSPACE)
        {
            fiction::area_stats<double>        stats{};
            fiction::area_params<tech, double> params{width, height, hspace, vspace};
            fiction::area<Lyt, double>(lyt, params, &stats);

            return stats.area;
        },
        "layout"_a, "width"_a = tech::CELL_WIDTH, "height"_a = tech::CELL_HEIGHT, "hspace"_a = tech::CELL_HSPACE,
        "vspace"_a = tech::CELL_VSPACE, DOC(fiction_area));
}

}  // namespace detail

inline void area(pybind11::module& m)
{
    detail::area<py_qca_layout>(m);
    detail::area<py_inml_layout>(m);
    detail::area<py_sidb_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_AREA_HPP
