//
// Created by Jan Drewniok on 08.04.24.
//

#ifndef PYFICTION_SIDB_LATTICE_HPP
#define PYFICTION_SIDB_LATTICE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/print_layout.hpp>
#include <fiction/technology/sidb_lattice.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename LatticeOrientation>
void sidb_lattice_cell_level_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    // fetch technology name
    auto orientation = std::string{fiction::sidb_lattice_name<LatticeOrientation>};
    std::transform(orientation.begin(), orientation.end(), orientation.begin(), ::tolower);
    std::cout << orientation << std::endl;

    /**
     * FCN cell technology.
     */
    // const py::class_<LatticeOrientation> tech(m, fmt::format("{}_technology", orientation).c_str());

    using py_sidb_lattice = py_sidb_lattice<LatticeOrientation>;

    /**
     * SiDB lattice.
     */
    py::class_<py_sidb_lattice, fiction::cell_level_layout<fiction::sidb_technology,
                                                           fiction::clocked_layout<fiction::tile_based_layout<
                                                               fiction::cartesian_layout<fiction::offset::ucoord_t>>>>>(
        m, fmt::format("{}_lattice", orientation).c_str(), DOC(fiction_cell_level_layout))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<py_sidb_lattice>&, const std::string&>(), "dimension"_a,
             "name"_a = "", DOC(fiction_cell_level_layout_cell_level_layout))

        ;
}

}  // namespace detail

inline void sidb_lattice_cell_level_layouts(pybind11::module& m)
{
    detail::sidb_lattice_cell_level_layout<fiction::sidb_100_lattice>(m);
    detail::sidb_lattice_cell_level_layout<fiction::sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_LATTICE_HPP
