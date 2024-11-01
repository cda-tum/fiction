//
// Created by Jan Drewniok on 08.04.24.
//

#ifndef PYFICTION_SIDB_LATTICE_HPP
#define PYFICTION_SIDB_LATTICE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <string>

namespace pyfiction
{

namespace detail
{

/**
 * Template function to bind SiDB lattices (regular and cube-coordinate) to Python.
 *
 * @tparam LatticeOrientation The lattice orientation (e.g., fiction::sidb_100_lattice, fiction::sidb_111_lattice).
 * @tparam LayoutType The layout type (e.g., py_sidb_layout for regular layouts, py_sidb_layout_cube for cube layouts).
 */
template <typename LatticeOrientation, typename LayoutType>
void sidb_lattice_cell_level_layout(pybind11::module& m, std::string lattice_name = "", std::string coordinate_tye = "")
{
    namespace py = pybind11;
    using namespace py::literals;

    // Fetch the lattice orientation name and convert to lowercase
    auto orientation = std::string{fiction::sidb_lattice_name<LatticeOrientation>};
    std::transform(orientation.begin(), orientation.end(), orientation.begin(), ::tolower);

    py::class_<LatticeOrientation, LayoutType>(
        m, fmt::format("sidb_{}_lattice{}", lattice_name, coordinate_tye).c_str(), DOC(fiction_cell_level_layout),
        py::module_local())
        .def(py::init<>())  // Default constructor
        .def(py::init<const fiction::aspect_ratio<LayoutType>&, const std::string&>(), "dimension"_a, "name"_a = "",
             DOC(fiction_sidb_lattice));
}

}  // namespace detail

inline void sidb_lattice(pybind11::module& m)
{
    // Bind the regular 100 and 111 SiDB lattices
    detail::sidb_lattice_cell_level_layout<py_sidb_100_lattice, py_sidb_layout>(m, "100");
    detail::sidb_lattice_cell_level_layout<py_sidb_100_lattice_cube, py_sidb_layout_cube>(m, "100", "_cube");
    detail::sidb_lattice_cell_level_layout<py_sidb_111_lattice, py_sidb_layout>(m, "111");
    detail::sidb_lattice_cell_level_layout<py_sidb_111_lattice_cube, py_sidb_layout_cube>(m, "111", "_cube");
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_LATTICE_HPP
