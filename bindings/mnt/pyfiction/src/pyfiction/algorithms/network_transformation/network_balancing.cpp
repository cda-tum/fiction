#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/network_transformation/network_balancing.hpp>

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

template <typename Ntk>
void network_balancing_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("network_balancing", &fiction::network_balancing<py_logic_network, Ntk>, py::arg("network"),
          py::arg("params") = fiction::network_balancing_params{}, DOC(fiction_network_balancing));

    m.def("is_balanced", &fiction::is_balanced<Ntk>, py::arg("network"),
          py::arg("params") = fiction::network_balancing_params{}, DOC(fiction_is_balanced));
}

}  // namespace detail

void network_balancing(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::network_balancing_params>(m, "network_balancing_params", DOC(fiction_network_balancing_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("unify_outputs", &fiction::network_balancing_params::unify_outputs,
                DOC(fiction_network_balancing_params_unify_outputs))

        ;

    detail::network_balancing_impl<py_logic_network>(m);
}

}  // namespace pyfiction
