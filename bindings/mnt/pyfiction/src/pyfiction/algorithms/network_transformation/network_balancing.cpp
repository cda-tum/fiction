#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/network_transformation/network_balancing.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void network_balancing_impl(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    m.def("network_balancing", &fiction::network_balancing<py_logic_network, Ntk>, py::arg("network"),
          py::arg("params") = fiction::network_balancing_params{}, DOC(fiction_network_balancing));

    m.def("is_balanced", &fiction::is_balanced<Ntk>, py::arg("network"),
          py::arg("params") = fiction::network_balancing_params{}, DOC(fiction_is_balanced));
}

}  // namespace detail

void network_balancing(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;

    py::class_<fiction::network_balancing_params>(m, "network_balancing_params", DOC(fiction_network_balancing_params))
        .def(py::init<>())
        .def_readwrite("unify_outputs", &fiction::network_balancing_params::unify_outputs,
                       DOC(fiction_network_balancing_params_unify_outputs))

        ;

    detail::network_balancing_impl<py_logic_network>(m);
}

}  // namespace pyfiction
