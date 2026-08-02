#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp>

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
void can_positive_charges_occur_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("can_positive_charges_occur", &fiction::can_positive_charges_occur<Lyt>, py::arg("lyt"),
          py::arg("sim_params"), DOC(fiction_can_positive_charges_occur));
}

}  // namespace detail

void can_positive_charges_occur(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::can_positive_charges_occur_impl<py_sidb_100_lattice>(m);
    detail::can_positive_charges_occur_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
