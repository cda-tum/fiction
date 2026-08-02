//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/technology/sidb_nm_position.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void sidb_nm_position(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("sidb_nm_position", &fiction::sidb_nm_position<Lyt>, py::arg("lyt"), py::arg("c"),
          DOC(fiction_sidb_nm_position));
}

}  // namespace detail

void sidb_nm_position(nanobind::module_& m)
{
    detail::sidb_nm_position<py_charge_distribution_surface>(m);
    detail::sidb_nm_position<py_sidb_layout>(m);
    detail::sidb_nm_position<py_sidb_lattice<fiction::sidb_100_lattice>>(m);
    detail::sidb_nm_position<py_sidb_lattice<fiction::sidb_111_lattice>>(m);
}

}  // namespace pyfiction
