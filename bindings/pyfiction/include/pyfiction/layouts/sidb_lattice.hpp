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

    using py_sidb_lattice = py_sidb_lattice<LatticeOrientation>;

    /**
     * SiDB lattice.
     */
    py::class_<py_sidb_lattice, py_sidb_layout>(m, fmt::format("sidb_lattice{}", orientation).c_str(),
                                                DOC(fiction_cell_level_layout), py::module_local())
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<py_sidb_layout>&, const std::string&>(), "dimension"_a, "name"_a = "",
             DOC(fiction_sidb_lattice))

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
