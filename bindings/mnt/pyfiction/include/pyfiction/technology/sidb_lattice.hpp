//
// Created by Jan Drewniok on 08.04.24.
//

#ifndef PYFICTION_SIDB_LATTICE_HPP
#define PYFICTION_SIDB_LATTICE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/print_layout.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename LatticeOrientation, typename SidbLattice, typename SidbLyt>
void sidb_lattice(pybind11::module& m, const std::string& coord_type)
{
    namespace py = pybind11;

    // fetch technology name
    auto orientation = std::string{fiction::sidb_lattice_name<LatticeOrientation>};
    std::transform(orientation.begin(), orientation.end(), orientation.begin(), ::tolower);

    /**
     * SiDB lattice.
     */
    py::class_<SidbLattice, SidbLyt>(m, fmt::format("sidb_{}_lattice_{}", orientation, coord_type).c_str(),
                                     DOC(fiction_cell_level_layout), py::module_local())
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<fiction::coordinate<SidbLyt>>&, const std::string&>(),
             py::arg("dimension"), py::arg("name") = "", DOC(fiction_sidb_lattice))

        ;
}

}  // namespace detail

inline void sidb_lattices(pybind11::module& m)
{
    detail::sidb_lattice<fiction::sidb_100_lattice, py_sidb_lattice<fiction::sidb_100_lattice, py_offset_coordinate>,
                         py_sidb_layout<py_offset_coordinate>>(m, "offset_coordinates");
    detail::sidb_lattice<fiction::sidb_100_lattice, py_sidb_lattice<fiction::sidb_100_lattice, py_cube_coordinate>,
                         py_sidb_layout<py_cube_coordinate>>(m, "cube_coordinates");
    detail::sidb_lattice<fiction::sidb_111_lattice, py_sidb_lattice<fiction::sidb_111_lattice>,
                         py_sidb_layout<py_offset_coordinate>>(m, "offset_coordinates");
    detail::sidb_lattice<fiction::sidb_111_lattice, py_sidb_lattice<fiction::sidb_111_lattice, py_cube_coordinate>,
                         py_sidb_layout<py_cube_coordinate>>(m, "cube_coordinates");
}

/**
 * A "factory" function that Python users can call as
 *   sidb_lattice(dimension, orientation="100", coordinate_type="offset")
 * to create the correct layout type (offset or cube).
 */
inline void sidb_lattice_factory(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "sidb_lattice",
        [](const py::tuple&   dimension,
           const std::string& orientation = "100",     // default
           const std::string& coord_type  = "offset",  // default
           const std::string& name        = ""         // default
        )
        {
            // Decide which orientation
            if (orientation == "100")
            {
                if (coord_type == "cube")
                {
                    // 1) H-Si(100)-2x1 orientation, cube coordinates
                    const auto ar = extract_aspect_ratio<py_sidb_layout<py_cube_coordinate>>(dimension);
                    return py::cast(py_sidb_100_lattice<py_cube_coordinate>{ar, name});
                }
                else
                {
                    // 2) H-Si(100)-2x1 orientation, offset coordinates
                    const auto ar = extract_aspect_ratio<py_sidb_layout<py_offset_coordinate>>(dimension);
                    return py::cast(py_sidb_100_lattice<py_offset_coordinate>{ar, name});
                }
            }
            else if (orientation == "111")
            {
                if (coord_type == "cube")
                {
                    // 3) H-Si(111)-1x1 orientation, cube coordinates
                    const auto ar = extract_aspect_ratio<py_sidb_layout<py_cube_coordinate>>(dimension);
                    return py::cast(py_sidb_111_lattice<py_cube_coordinate>{ar, name});
                }
                else
                {
                    // 4) H-Si(111)-1x1 orientation, offset coordinates
                    const auto ar = extract_aspect_ratio<py_sidb_layout<py_offset_coordinate>>(dimension);
                    return py::cast(py_sidb_111_lattice<py_offset_coordinate>{ar, name});
                }
            }
            else
            {
                throw std::invalid_argument("[pyfiction] Invalid orientation. Use '100' or '111'.");
            }
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0), py::arg("orientation") = "100",
        py::arg("coordinate_type") = "offset", py::arg("name") = "",
        R"doc(
            Creates and returns a sidb_lattice instance, choosing both the lattice orientation
            and the coordinate system based on the string arguments.

            Valid options for `orientation` are:
                - "100" (default)
                - "111"

            Valid options for `coordinate_type` are:
                - "offset" (default)
                - "cube"
        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_LATTICE_HPP
