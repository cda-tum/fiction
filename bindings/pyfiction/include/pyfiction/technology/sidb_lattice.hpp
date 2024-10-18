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
void sidb_lattice_cell_level_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace py::literals;

    // Fetch the lattice orientation name and convert to lowercase
    auto orientation = std::string{fiction::sidb_lattice_name<LatticeOrientation>};
    std::transform(orientation.begin(), orientation.end(), orientation.begin(), ::tolower);

    // Define the type alias for the specific SiDB lattice layout being bound
    using py_sidb_lattice = fiction::sidb_lattice<LatticeOrientation, LayoutType>;

    /**
     * SiDB lattice.
     */
    if constexpr (fiction::has_cube_coord_v<LayoutType> && fiction::is_sidb_lattice_100_v<LatticeOrientation>)
    {
        py::class_<py_sidb_lattice, LayoutType>(m, "sidb_100_lattice_cube",
                                                DOC(fiction_cell_level_layout), py::module_local())
            .def(py::init<>())  // Default constructor
            .def(py::init<const fiction::aspect_ratio<LayoutType>&, const std::string&>(), "dimension"_a, "name"_a = "",
                 DOC(fiction_sidb_lattice));
    }
    else if constexpr (fiction::has_cube_coord_v<LayoutType> && fiction::is_sidb_lattice_111_v<LatticeOrientation>)
    {
        py::class_<py_sidb_lattice, LayoutType>(m, "sidb_111_lattice_cube",
                                                DOC(fiction_cell_level_layout), py::module_local())
            .def(py::init<>())  // Default constructor
            .def(py::init<const fiction::aspect_ratio<LayoutType>&, const std::string&>(), "dimension"_a, "name"_a = "",
                 DOC(fiction_sidb_lattice));
    }
    else if constexpr (!fiction::has_cube_coord_v<LayoutType> && fiction::is_sidb_lattice_100_v<LatticeOrientation>)
    {
        py::class_<py_sidb_lattice, LayoutType>(m, "sidb_100_lattice",
                                                DOC(fiction_cell_level_layout), py::module_local())
            .def(py::init<>())  // Default constructor
            .def(py::init<const fiction::aspect_ratio<LayoutType>&, const std::string&>(), "dimension"_a, "name"_a = "",
                 DOC(fiction_sidb_lattice));
    }
    else if constexpr (!fiction::has_cube_coord_v<LayoutType> && fiction::is_sidb_lattice_111_v<LatticeOrientation>)
    {
        py::class_<py_sidb_lattice, LayoutType>(m, "sidb_111_lattice",
                                                DOC(fiction_cell_level_layout), py::module_local())
            .def(py::init<>())  // Default constructor
            .def(py::init<const fiction::aspect_ratio<LayoutType>&, const std::string&>(), "dimension"_a, "name"_a = "",
                 DOC(fiction_sidb_lattice));
    }
}


}  // namespace detail


inline void sidb_lattices_100_cube(pybind11::module& m)
{
    // Bind the cube-coordinate 100 and 111 SiDB lattices
    detail::sidb_lattice_cell_level_layout<fiction::sidb_100_lattice, py_sidb_layout_cube>(m);
}

inline void sidb_lattices_111(pybind11::module& m)
{

    detail::sidb_lattice_cell_level_layout<fiction::sidb_111_lattice, py_sidb_layout>(m);
}

inline void sidb_lattices_100(pybind11::module& m)
{
    // Bind the regular 100 and 111 SiDB lattices
    detail::sidb_lattice_cell_level_layout<fiction::sidb_100_lattice, py_sidb_layout>(m);
}


inline void sidb_lattices_111_cube(pybind11::module& m)
{
    // Bind the regular 100 and 111 SiDB lattices
    detail::sidb_lattice_cell_level_layout<fiction::sidb_111_lattice, py_sidb_layout_cube>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_LATTICE_HPP
