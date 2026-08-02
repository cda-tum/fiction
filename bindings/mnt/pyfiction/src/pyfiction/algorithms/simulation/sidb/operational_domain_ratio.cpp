#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/operational_domain_ratio.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
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
void compute_operational_ratio_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("operational_domain_ratio", &fiction::operational_domain_ratio<Lyt, py_tt>, py::arg("lyt"), py::arg("spec"),
          py::arg("pp"), py::arg("params") = fiction::operational_domain_ratio_params{},
          DOC(fiction_operational_domain_ratio));
}

}  // namespace detail

void compute_operational_ratio(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::operational_domain_ratio_params>(m, "operational_domain_ratio_params",
                                                         DOC(fiction_operational_domain_ratio_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("op_domain_params", &fiction::operational_domain_ratio_params::op_domain_params,
                DOC(fiction_operational_domain_ratio_params_op_domain_params));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::compute_operational_ratio_impl<py_sidb_100_lattice>(m);
    detail::compute_operational_ratio_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
