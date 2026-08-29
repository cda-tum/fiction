//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/model/nm_position.hpp>
#include <fiction/technology/sidb/surfaces/lattice_orientations.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void sidb_nm_position(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("sidb_nm_position", &fiction::sidb::model::nm_position<Lyt>, py::arg("lyt"), py::arg("c"),
          DOC(fiction_sidb_model_nm_position));
}

}  // namespace detail

void sidb_nm_position(nanobind::module_& m)
{
    detail::sidb_nm_position<py_charge_distribution_surface>(m);
    detail::sidb_nm_position<py_sidb_layout>(m);
    detail::sidb_nm_position<py_sidb_lattice<fiction::sidb::surfaces::lattice_100>>(m);
    detail::sidb_nm_position<py_sidb_lattice<fiction::sidb::surfaces::lattice_111>>(m);
}

}  // namespace pyfiction
