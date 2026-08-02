#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/graph/graph_coloring.hpp>
#include <fiction/algorithms/physical_design/color_routing.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <utility>
#include <vector>

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
void color_routing_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "color_routing",
        [](Lyt& lyt, const std::vector<std::pair<fiction::coordinate<Lyt>, fiction::coordinate<Lyt>>>& objectives,
           const fiction::color_routing_params& params)
        {
            std::vector<fiction::routing_objective<Lyt>> objs{};
            objs.reserve(objectives.size());

            for (const auto& obj : objectives)
            {
                objs.push_back({obj.first, obj.second});
            }

            return fiction::color_routing(lyt, objs, params);
        },
        py::arg("layout"), py::arg("objectives"), py::arg("params") = fiction::color_routing_params{},
        DOC(fiction_color_routing));
}

}  // namespace detail

void color_routing(nanobind::module_& m)
{
    namespace py = nanobind;

    /**
     * Graph coloring engine selector type.
     */
    nanobind::enum_<fiction::graph_coloring_engine>(m, "graph_coloring_engine", DOC(fiction_graph_coloring_engine))
        .value("MCS", fiction::graph_coloring_engine::MCS, DOC(fiction_graph_coloring_engine_MCS))
        .value("DSATUR", fiction::graph_coloring_engine::DSATUR, DOC(fiction_graph_coloring_engine_DSATUR))
        .value("LMXRLF", fiction::graph_coloring_engine::LMXRLF, DOC(fiction_graph_coloring_engine_LMXRLF))
        .value("TABUCOL", fiction::graph_coloring_engine::TABUCOL, DOC(fiction_graph_coloring_engine_TABUCOL))
        .value("SAT", fiction::graph_coloring_engine::SAT, DOC(fiction_graph_coloring_engine_SAT));

    py::class_<fiction::color_routing_params>(m, "color_routing_params", DOC(fiction_color_routing_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("conduct_partial_routing", &fiction::color_routing_params::conduct_partial_routing,
                DOC(fiction_color_routing_params_conduct_partial_routing))
        .def_rw("crossings", &fiction::color_routing_params::crossings, DOC(fiction_color_routing_params_crossings))
        .def_rw("path_limit", &fiction::color_routing_params::path_limit, DOC(fiction_color_routing_params_path_limit))
        .def_rw("engine", &fiction::color_routing_params::engine, DOC(fiction_color_routing_params_engine))
        .def_rw("partial_sat", &fiction::color_routing_params::partial_sat,
                DOC(fiction_color_routing_params_partial_sat));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::color_routing_impl<py_cartesian_obstruction_layout>(m);
    detail::color_routing_impl<py_cartesian_gate_layout>(m);
    detail::color_routing_impl<py_shifted_cartesian_obstruction_layout>(m);
    detail::color_routing_impl<py_shifted_cartesian_gate_layout>(m);
    detail::color_routing_impl<py_hexagonal_obstruction_layout>(m);
    detail::color_routing_impl<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
