//
// Created by Jan Drewniok on 08.04.24.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename LatticeOrientation>
void sidb_lattice_cell_level_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    // fetch technology name
    auto orientation = std::string{fiction::sidb_lattice_name<LatticeOrientation>};
    std::transform(orientation.begin(), orientation.end(), orientation.begin(), ::tolower);

    using py_sidb_lattice = py_sidb_lattice<LatticeOrientation>;

    /**
     * SiDB lattice.
     */
    py::class_<py_sidb_lattice, py_sidb_layout>(m, fmt::format("sidb_{}_lattice", orientation).c_str(),
                                                DOC(fiction_cell_level_layout))
        .def(py::init<>(), "Default constructor.")
        .def(py::init<const fiction::aspect_ratio<py_sidb_layout>&, const std::string&>(), py::arg("dimension"),
             py::arg("name") = "", DOC(fiction_sidb_lattice))
        .def("clone", &py_sidb_lattice::clone)

        ;
}

}  // namespace detail

void sidb_lattices(nanobind::module_& m)
{
    detail::sidb_lattice_cell_level_layout<fiction::sidb_100_lattice>(m);
    detail::sidb_lattice_cell_level_layout<fiction::sidb_111_lattice>(m);
}

}  // namespace pyfiction
