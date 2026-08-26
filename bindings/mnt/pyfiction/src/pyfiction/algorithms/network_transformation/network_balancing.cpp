#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/synthesis/network_balancing.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void network_balancing_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("network_balancing", &fiction::synthesis::network_balancing<py_logic_network, Ntk>, py::arg("network"),
          py::arg("params") = fiction::synthesis::network_balancing_params{}, DOC(fiction_network_balancing));

    m.def("is_balanced", &fiction::synthesis::is_balanced<Ntk>, py::arg("network"),
          py::arg("params") = fiction::synthesis::network_balancing_params{}, DOC(fiction_is_balanced));
}

}  // namespace detail

void network_balancing(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::synthesis::network_balancing_params>(m, "network_balancing_params",
                                                             DOC(fiction_network_balancing_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("unify_outputs", &fiction::synthesis::network_balancing_params::unify_outputs,
                DOC(fiction_network_balancing_params_unify_outputs))

        ;

    detail::network_balancing_impl<py_logic_network>(m);
}

}  // namespace pyfiction
