#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/path_finding/a_star.hpp>
#include <fiction/algorithms/path_finding/cost.hpp>
#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/routing_utils.hpp>

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
void a_star_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "a_star",
        [](const Lyt& lyt, const fiction::coordinate<Lyt>& source, const fiction::coordinate<Lyt>& target,
           const fiction::a_star_params& params)
        {
            return static_cast<std::vector<fiction::coordinate<Lyt>>>(
                fiction::a_star<fiction::layout_coordinate_path<Lyt>, Lyt>(lyt, {source, target},
                                                                           fiction::manhattan_distance_functor<Lyt>(),
                                                                           fiction::unit_cost_functor<Lyt>(), params));
        },
        py::arg("layout"), py::arg("source"), py::arg("target"), py::arg("params") = fiction::a_star_params{},
        DOC(fiction_a_star));

    m.def("a_star_distance", &fiction::a_star_distance<Lyt, double>, py::arg("layout"), py::arg("source"),
          py::arg("target"), DOC(fiction_a_star_distance));
}

}  // namespace detail

void a_star(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::a_star_params>(m, "a_star_params", DOC(fiction_a_star_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("crossings", &fiction::a_star_params::crossings, DOC(fiction_a_star_params_crossings));

    detail::a_star_impl<py_cartesian_obstruction_layout>(m);
    detail::a_star_impl<py_cartesian_gate_layout>(m);
    detail::a_star_impl<py_cartesian_clocked_layout>(m);
    detail::a_star_impl<py_cartesian_layout>(m);
    detail::a_star_impl<py_shifted_cartesian_obstruction_layout>(m);
    detail::a_star_impl<py_shifted_cartesian_gate_layout>(m);
    detail::a_star_impl<py_shifted_cartesian_clocked_layout>(m);
    detail::a_star_impl<py_shifted_cartesian_layout>(m);
    detail::a_star_impl<py_hexagonal_obstruction_layout>(m);
    detail::a_star_impl<py_hexagonal_gate_layout>(m);
    detail::a_star_impl<py_hexagonal_clocked_layout>(m);
    detail::a_star_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction
