#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>

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
void fanout_substitution_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("fanout_substitution", &fiction::fanout_substitution<py_logic_network, Ntk>, py::arg("network"),
          py::arg("params") = fiction::fanout_substitution_params{}, DOC(fiction_fanout_substitution));

    m.def("is_fanout_substituted", &fiction::is_fanout_substituted<Ntk>, py::arg("network"),
          py::arg("params") = fiction::fanout_substitution_params{}, DOC(fiction_is_fanout_substituted));
}

}  // namespace detail

void fanout_substitution(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::fanout_substitution_params::substitution_strategy>(
        m, "substitution_strategy", DOC(fiction_fanout_substitution_params_substitution_strategy))
        .value("BREADTH", fiction::fanout_substitution_params::substitution_strategy::BREADTH,
               DOC(fiction_fanout_substitution_params_substitution_strategy_BREADTH))
        .value("DEPTH", fiction::fanout_substitution_params::substitution_strategy::DEPTH,
               DOC(fiction_fanout_substitution_params_substitution_strategy_DEPTH))
        .value("RANDOM", fiction::fanout_substitution_params::substitution_strategy::RANDOM,
               DOC(fiction_fanout_substitution_params_substitution_strategy_RANDOM))

        ;

    py::class_<fiction::fanout_substitution_params>(m, "fanout_substitution_params",
                                                    DOC(fiction_fanout_substitution_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("strategy", &fiction::fanout_substitution_params::strategy,
                DOC(fiction_fanout_substitution_params_strategy))
        .def_rw("degree", &fiction::fanout_substitution_params::degree, DOC(fiction_fanout_substitution_params_degree))
        .def_rw("threshold", &fiction::fanout_substitution_params::threshold,
                DOC(fiction_fanout_substitution_params_threshold))
        .def_rw("seed", &fiction::fanout_substitution_params::seed, DOC(fiction_fanout_substitution_params_seed))

        ;

    detail::fanout_substitution_impl<py_logic_network>(m);
}

}  // namespace pyfiction
