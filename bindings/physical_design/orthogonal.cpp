/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Python bindings for `fiction/physical_design/orthogonal.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/physical_design/orthogonal.hpp>

#include <sstream>
#include <type_traits>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using orthogonal_physical_design_options = fiction::physical_design::orthogonal_physical_design_params;

namespace detail
{
/**
 * @brief Bind orthogonal physical design for a registered output layout class.
 * @tparam Lyt Layout representation.
 * @param m Physical design module.
 * @param signature Python signature including the selected return type.
 */
template <typename Lyt>
void register_orthogonal(nb::module_& m, const char* signature)
{
    /** @brief Compute orthogonal physical design for the selected layout representation. */
    const auto design =
        [](const py_tec_network& network, const nb::handle layout_type,
           fiction::physical_design::orthogonal_physical_design_stats*                 statistics,
           const decltype(orthogonal_physical_design_options{}.number_of_clock_phases) number_of_clock_phases,
           const decltype(orthogonal_physical_design_options{}.layout_arrangement)     layout_arrangement,
           const decltype(orthogonal_physical_design_options{}.on_progress)            on_progress)
    {
        if (!layout_type.is(nb::type<Lyt>()))
        {
            throw nb::next_overload();
        }
        orthogonal_physical_design_options parameters{};
        parameters.number_of_clock_phases = number_of_clock_phases;
        parameters.layout_arrangement     = layout_arrangement;
        parameters.on_progress            = on_progress;
        const nb::gil_scoped_release release{};
        return fiction::physical_design::orthogonal<Lyt, py_tec_network>(network, parameters, statistics);
    };
    if constexpr (std::is_same_v<Lyt, py_cartesian_gate_layout>)
    {
        m.def("orthogonal", design, nb::arg("network"), nb::kw_only(), nb::arg("layout_type") = nb::type<Lyt>(),
              nb::arg("statistics")             = nullptr,
              nb::arg("number_of_clock_phases") = orthogonal_physical_design_options{}.number_of_clock_phases,
              nb::arg("layout_arrangement")     = orthogonal_physical_design_options{}.layout_arrangement,
              nb::arg("on_progress").none()     = nb::none(), nb::sig(signature),
              R"doc(A scalable placement & routing approach based on orthogonal graph
drawing as originally proposed in \"Scalable Design for Field-coupled
Nanocomputing Circuits\" by M. Walter, R. Wille, F. Sill Torres, D.
Große, and R. Drechsler in ASP-DAC 2019. A more extensive description
can be found in \"Design Automation for Field-coupled
Nanotechnologies\" by M. Walter, R. Wille, F. Sill Torres, and R.
Drechsler published by Springer Nature in 2022.

Via certain restrictions to the degrees of freedom in FCN physical
design, this algorithm achieves a polynomial time complexity. However,
these restrictions lead to an overall approximation of optimal layout
quality within several factors. Therefore, this algorithm produces
valid layouts within a short amount of time, its results are far from
being optimal in terms of area.

The imposed restrictions are that the input logic network has to be a
3-graph, i.e., cannot have any node exceeding degree 3 (combined input
and output), and that the resulting layout is always 2DDWave-clocked.

This algorithm is based on a modification of \"Improved orthogonal
drawings of 3-graphs\" by Therese C. Biedl in Canadian Conference on
Computational Geometry 1996. Biedl's original algorithm works for
undirected graphs only while this modification respects information
flow of directed logic networks. To this end, the edge directions of
the logic network directly used instead of relabeling the edges
according to its DFS tree, ordering the vertices using topological
sorting instead of DFS, and adding an extra placement rule for nodes
without predecessors.

The algorithm works in polynomial time :math:`\mathcal{O}(3|N| + |L|)`
where :math:`|N|` is the number of nodes the given network and
:math:`|L|` is the resulting layout size given by :math:`x \cdot y`,
which approaches :math:`(\frac{|N|}{2})^2` asymptotically.

May throw a high_degree_fanin_exception if `ntk` contains any node
with a fan-in larger than 2.

Args:
    ntk: The network that is to place and route.
    pst: Statistics.

Returns:
    A gate-level layout of type `Lyt` that implements `ntk` as an FCN
    circuit.

Raises:
    std::invalid_argument: If `Lyt` is a shifted Cartesian or
                           hexagonal layout and
                           `layout_arrangement` is empty.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    }
    else
    {
        m.def("orthogonal", design, nb::arg("network"), nb::kw_only(), nb::arg("layout_type"),
              nb::arg("statistics")             = nullptr,
              nb::arg("number_of_clock_phases") = orthogonal_physical_design_options{}.number_of_clock_phases,
              nb::arg("layout_arrangement")     = orthogonal_physical_design_options{}.layout_arrangement,
              nb::arg("on_progress").none()     = nb::none(), nb::sig(signature));
    }
}
}  // namespace detail

/**
 * @brief Binds orthogonal helpers.
 * @param m Python module.
 */
void register_orthogonal(nb::module_& m)
{
    nb::enum_<fiction::layouts::clocking::num_clks>(m, "num_clks", DOC(fiction_layouts_clocking_num_clks))
        .value("THREE", fiction::layouts::clocking::num_clks::THREE, DOC(fiction_layouts_clocking_num_clks_THREE))
        .value("FOUR", fiction::layouts::clocking::num_clks::FOUR, DOC(fiction_layouts_clocking_num_clks_FOUR));

    nb::class_<fiction::physical_design::orthogonal_physical_design_stats>(
        m, "orthogonal_stats", DOC(fiction_physical_design_orthogonal_physical_design_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::orthogonal_physical_design_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_total", &fiction::physical_design::orthogonal_physical_design_stats::time_total,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_time_total))
        .def_ro("x_size", &fiction::physical_design::orthogonal_physical_design_stats::x_size,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_x_size))
        .def_ro("y_size", &fiction::physical_design::orthogonal_physical_design_stats::y_size,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_y_size))
        .def_ro("num_gates", &fiction::physical_design::orthogonal_physical_design_stats::num_gates,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_num_gates))
        .def_ro("num_wires", &fiction::physical_design::orthogonal_physical_design_stats::num_wires,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_num_wires))
        .def_ro("num_crossings", &fiction::physical_design::orthogonal_physical_design_stats::num_crossings,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_num_crossings));

    detail::register_orthogonal<py_cartesian_gate_layout>(
        m, "def orthogonal(network: mnt.pyfiction.networks.technology_network, *, layout_type: "
           "type[mnt.pyfiction.layouts.cartesian_gate_layout] = mnt.pyfiction.layouts.cartesian_gate_layout, "
           "statistics: orthogonal_stats | None = None, number_of_clock_phases: num_clks = ..., layout_arrangement: "
           "mnt.pyfiction.layouts.arrangement | None = ..., on_progress: Callable[[str, int, int], None] | None = ...) "
           "-> mnt.pyfiction.layouts.cartesian_gate_layout");
    detail::register_orthogonal<py_hexagonal_gate_layout>(
        m,
        "def orthogonal(network: mnt.pyfiction.networks.technology_network, *, layout_type: "
        "type[mnt.pyfiction.layouts.hexagonal_gate_layout], statistics: orthogonal_stats | None = None, "
        "number_of_clock_phases: num_clks = ..., layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ..., "
        "on_progress: Callable[[str, int, int], None] | None = ...) -> mnt.pyfiction.layouts.hexagonal_gate_layout");
    m.def(
        "orthogonal",
        [](const py_tec_network& network, const nb::handle layout_type,
           fiction::physical_design::orthogonal_physical_design_stats*                 statistics,
           const decltype(orthogonal_physical_design_options{}.number_of_clock_phases) number_of_clock_phases,
           const decltype(orthogonal_physical_design_options{}.layout_arrangement)     layout_arrangement,
           const decltype(orthogonal_physical_design_options{}.on_progress)            on_progress) -> nb::object
        { throw nb::type_error("layout_type must be a supported registered gate layout class"); },
        nb::arg("network"), nb::kw_only(), nb::arg("layout_type"), nb::arg("statistics") = nullptr,
        nb::arg("number_of_clock_phases") = orthogonal_physical_design_options{}.number_of_clock_phases,
        nb::arg("layout_arrangement")     = orthogonal_physical_design_options{}.layout_arrangement,
        nb::arg("on_progress").none()     = nb::none());
}

}  // namespace pyfiction
