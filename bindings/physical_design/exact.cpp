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
 * @brief Python bindings for `fiction/physical_design/exact.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <nanobind/nanobind.h>

#if (FICTION_Z3_SOLVER)

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/exact.hpp>

#include <sstream>
#include <type_traits>

#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using exact_physical_design_options = fiction::physical_design::exact_physical_design_params;

namespace detail
{
/**
 * @brief Bind exact physical design for a registered output layout class.
 * @tparam Lyt Layout representation.
 * @param m Physical design module.
 * @param signature Python signature including the selected return type.
 */
template <typename Lyt>
void register_exact(nb::module_& m, const char* signature)
{
    /** @brief Compute exact physical design for the selected layout representation. */
    const auto design =
        [](const py_tec_network& network, const nb::handle layout_type,
           fiction::physical_design::exact_physical_design_stats*                   statistics,
           const decltype(exact_physical_design_options{}.scheme)                   scheme,
           const decltype(exact_physical_design_options{}.layout_arrangement)       layout_arrangement,
           const decltype(exact_physical_design_options{}.upper_bound_area)         upper_bound_area,
           const decltype(exact_physical_design_options{}.upper_bound_x)            upper_bound_x,
           const decltype(exact_physical_design_options{}.upper_bound_y)            upper_bound_y,
           const decltype(exact_physical_design_options{}.fixed_size)               fixed_size,
           const decltype(exact_physical_design_options{}.num_threads)              num_threads,
           const decltype(exact_physical_design_options{}.synchronization_elements) synchronization_elements,
           const decltype(exact_physical_design_options{}.crossings)                crossings,
           const decltype(exact_physical_design_options{}.border_io)                border_io,
           const decltype(exact_physical_design_options{}.straight_inverters)       straight_inverters,
           const decltype(exact_physical_design_options{}.desynchronize)            desynchronize,
           const decltype(exact_physical_design_options{}.minimize_wires)           minimize_wires,
           const decltype(exact_physical_design_options{}.minimize_crossings)       minimize_crossings,
           const decltype(exact_physical_design_options{}.timeout)                  timeout,
           const decltype(exact_physical_design_options{}.technology_specifics)     technology_specifics,
           const decltype(exact_physical_design_options{}.on_progress)              on_progress,
           const decltype(exact_physical_design_options{}.on_worker_progress)       on_worker_progress)
    {
        if (!layout_type.is(nb::type<Lyt>()))
        {
            throw nb::next_overload();
        }
        exact_physical_design_options parameters{};
        parameters.scheme                   = scheme;
        parameters.layout_arrangement       = layout_arrangement;
        parameters.upper_bound_area         = upper_bound_area;
        parameters.upper_bound_x            = upper_bound_x;
        parameters.upper_bound_y            = upper_bound_y;
        parameters.fixed_size               = fixed_size;
        parameters.num_threads              = num_threads;
        parameters.synchronization_elements = synchronization_elements;
        parameters.crossings                = crossings;
        parameters.border_io                = border_io;
        parameters.straight_inverters       = straight_inverters;
        parameters.desynchronize            = desynchronize;
        parameters.minimize_wires           = minimize_wires;
        parameters.minimize_crossings       = minimize_crossings;
        parameters.timeout                  = timeout;
        parameters.technology_specifics     = technology_specifics;
        parameters.on_progress              = on_progress;
        parameters.on_worker_progress       = on_worker_progress;
        const nb::gil_scoped_release release{};
        return fiction::physical_design::exact<Lyt, py_tec_network>(network, parameters, statistics);
    };
    if constexpr (std::is_same_v<Lyt, py_cartesian_gate_layout>)
    {
        m.def("exact", design, nb::arg("network"), nb::kw_only(), nb::arg("layout_type") = nb::type<Lyt>(),
              nb::arg("statistics") = nullptr, nb::arg("scheme") = exact_physical_design_options{}.scheme,
              nb::arg("layout_arrangement")       = exact_physical_design_options{}.layout_arrangement,
              nb::arg("upper_bound_area")         = exact_physical_design_options{}.upper_bound_area,
              nb::arg("upper_bound_x")            = exact_physical_design_options{}.upper_bound_x,
              nb::arg("upper_bound_y")            = exact_physical_design_options{}.upper_bound_y,
              nb::arg("fixed_size")               = exact_physical_design_options{}.fixed_size,
              nb::arg("num_threads")              = exact_physical_design_options{}.num_threads,
              nb::arg("synchronization_elements") = exact_physical_design_options{}.synchronization_elements,
              nb::arg("crossings")                = exact_physical_design_options{}.crossings,
              nb::arg("border_io")                = exact_physical_design_options{}.border_io,
              nb::arg("straight_inverters")       = exact_physical_design_options{}.straight_inverters,
              nb::arg("desynchronize")            = exact_physical_design_options{}.desynchronize,
              nb::arg("minimize_wires")           = exact_physical_design_options{}.minimize_wires,
              nb::arg("minimize_crossings")       = exact_physical_design_options{}.minimize_crossings,
              nb::arg("timeout")                  = exact_physical_design_options{}.timeout,
              nb::arg("technology_specifics")     = exact_physical_design_options{}.technology_specifics,
              nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
              nb::sig(signature), R"doc(An exact placement & routing approach using SMT solving as originally
proposed in \"An Exact Method for Design Exploration of Quantum-dot
Cellular Automata\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
and R. Drechsler in DATE 2018. A more extensive description can be
found in \"Design Automation for Field-coupled Nanotechnologies\" by
M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
Springer Nature in 2022.

Via incremental SMT calls, an optimal gate-level layout for a given
logic network will be found under constraints. Starting with :math:`n`
tiles, where :math:`n` is the number of logic network nodes, each
possible layout aspect ratio will be examined by factorization and
tested for routability with the SMT solver Z3. When no upper bound is
given, this approach will run until it finds a solution to the
placement & routing problem instance.

Note that there a combinations of constraints for which no valid
solution under the given parameters exist for the given logic network.
Such combinations cannot be detected automatically. It is, thus,
recommended to always set a timeout. Recommended settings include the
use of I/O pins located at the layout borders for better integration.
Most networks are not realizable without crossings enabled. Specifying
a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
allows for the strictest constraints and, thereby, finds a solution
the quickest. However, for high input degree networks, no valid
solution exists when border I/Os are to be used unless global
synchronization is disabled. Generally, solutions are found the
fastest with the following settings: Crossings enabled, de-
synchronization enabled, and 2DDWave clocking given. Multi-threading
can sometimes speed up the process, especially for large networks.
Note that the more threads are being used, the less information can be
shared across the individual solver runs which destroys the benefits
of incremental solving and thereby, comparatively, slows down each
run.

The SMT instance works with a single layer of variables even though it
is possible to allow crossings in the solution. The reduced number of
variables saves a considerable amount of runtime. That's why
`layout.foreach_ground_tile()` is used even though the model will be
mapped to a 3-dimensional layout afterwards. Generally, the algorithm
incorporates quite a few encoding optimizations to be as performant as
possible on various layout topologies and clocking schemes.

The approach applies to any data structures that implement the
necessary functions to comply with `is_network_type` and concrete
gate-level layouts, respectively. It is, thereby, mostly technology-
independent but can make certain assumptions if needed, for instance
for ToPoliNano-compliant circuits.

This approach requires the Z3 SMT solver to be installed on the
system. Due to this circumstance, it is excluded from (CLI)
compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
cmake call.

May throw a high_degree_fanin_exception if `ntk` contains any node
with a fan-in too large to be handled by the specified clocking
scheme.

Args:
    ntk: The network that is to place and route.
    pst: Statistics.

Returns:
    A gate-level layout of type `Lyt` that implements `ntk` as an FCN
    circuit if one is found under the given parameters;
    `std::nullopt`, otherwise.

Raises:
    std::invalid_argument: If `Lyt` is a shifted Cartesian or
                           hexagonal layout and
                           `layout_arrangement` is empty.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    }
    else
    {
        m.def("exact", design, nb::arg("network"), nb::kw_only(), nb::arg("layout_type"),
              nb::arg("statistics") = nullptr, nb::arg("scheme") = exact_physical_design_options{}.scheme,
              nb::arg("layout_arrangement")       = exact_physical_design_options{}.layout_arrangement,
              nb::arg("upper_bound_area")         = exact_physical_design_options{}.upper_bound_area,
              nb::arg("upper_bound_x")            = exact_physical_design_options{}.upper_bound_x,
              nb::arg("upper_bound_y")            = exact_physical_design_options{}.upper_bound_y,
              nb::arg("fixed_size")               = exact_physical_design_options{}.fixed_size,
              nb::arg("num_threads")              = exact_physical_design_options{}.num_threads,
              nb::arg("synchronization_elements") = exact_physical_design_options{}.synchronization_elements,
              nb::arg("crossings")                = exact_physical_design_options{}.crossings,
              nb::arg("border_io")                = exact_physical_design_options{}.border_io,
              nb::arg("straight_inverters")       = exact_physical_design_options{}.straight_inverters,
              nb::arg("desynchronize")            = exact_physical_design_options{}.desynchronize,
              nb::arg("minimize_wires")           = exact_physical_design_options{}.minimize_wires,
              nb::arg("minimize_crossings")       = exact_physical_design_options{}.minimize_crossings,
              nb::arg("timeout")                  = exact_physical_design_options{}.timeout,
              nb::arg("technology_specifics")     = exact_physical_design_options{}.technology_specifics,
              nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
              nb::sig(signature));
    }
}
}  // namespace detail

/**
 * @brief Binds exact helpers.
 * @param m Python module.
 */
void register_exact(nb::module_& m)
{
    nb::enum_<fiction::physical_design::technology_constraints>(m, "technology_constraints",
                                                                DOC(fiction_physical_design_technology_constraints))
        .value("NONE", fiction::physical_design::technology_constraints::NONE,
               DOC(fiction_physical_design_technology_constraints_NONE))
        .value("TOPOLINANO", fiction::physical_design::technology_constraints::TOPOLINANO,
               DOC(fiction_physical_design_technology_constraints_TOPOLINANO));

    nb::class_<fiction::physical_design::exact_physical_design_stats>(
        m, "exact_stats", DOC(fiction_physical_design_exact_physical_design_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::exact_physical_design_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_total", &fiction::physical_design::exact_physical_design_stats::time_total,
                DOC(fiction_physical_design_exact_physical_design_stats_time_total))
        .def_ro("x_size", &fiction::physical_design::exact_physical_design_stats::x_size,
                DOC(fiction_physical_design_exact_physical_design_stats_x_size))
        .def_ro("y_size", &fiction::physical_design::exact_physical_design_stats::y_size,
                DOC(fiction_physical_design_exact_physical_design_stats_y_size))
        .def_ro("num_gates", &fiction::physical_design::exact_physical_design_stats::num_gates,
                DOC(fiction_physical_design_exact_physical_design_stats_num_gates))
        .def_ro("num_wires", &fiction::physical_design::exact_physical_design_stats::num_wires,
                DOC(fiction_physical_design_exact_physical_design_stats_num_wires))
        .def_ro("num_crossings", &fiction::physical_design::exact_physical_design_stats::num_crossings,
                DOC(fiction_physical_design_exact_physical_design_stats_num_crossings))
        .def_ro("num_aspect_ratios", &fiction::physical_design::exact_physical_design_stats::num_aspect_ratios,
                DOC(fiction_physical_design_exact_physical_design_stats_num_aspect_ratios));

    detail::register_exact<py_cartesian_gate_layout>(
        m,
        "def exact(network: mnt.pyfiction.networks.technology_network, *, layout_type: "
        "type[mnt.pyfiction.layouts.cartesian_gate_layout] = mnt.pyfiction.layouts.cartesian_gate_layout, statistics: "
        "exact_stats | None = None, scheme: str = ..., layout_arrangement: mnt.pyfiction.layouts.arrangement | None = "
        "..., upper_bound_area: int = ..., upper_bound_x: int = ..., upper_bound_y: int = ..., fixed_size: bool = ..., "
        "num_threads: int = ..., synchronization_elements: bool = ..., crossings: bool = ..., border_io: bool = ..., "
        "straight_inverters: bool = ..., desynchronize: bool = ..., minimize_wires: bool = ..., minimize_crossings: "
        "bool = ..., timeout: int = ..., technology_specifics: technology_constraints = ..., on_progress: "
        "Callable[[str, int, int], None] | None = ..., on_worker_progress: Callable[[int, int, str, int, int, bool], "
        "None] | None = ...) -> mnt.pyfiction.layouts.cartesian_gate_layout | None");
    detail::register_exact<py_shifted_cartesian_gate_layout>(
        m, "def exact(network: mnt.pyfiction.networks.technology_network, *, layout_type: "
           "type[mnt.pyfiction.layouts.shifted_cartesian_gate_layout], statistics: exact_stats | None = None, scheme: "
           "str = ..., layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ..., upper_bound_area: int = "
           "..., upper_bound_x: int = ..., upper_bound_y: int = ..., fixed_size: bool = ..., num_threads: int = ..., "
           "synchronization_elements: bool = ..., crossings: bool = ..., border_io: bool = ..., straight_inverters: "
           "bool = ..., desynchronize: bool = ..., minimize_wires: bool = ..., minimize_crossings: bool = ..., "
           "timeout: int = ..., technology_specifics: technology_constraints = ..., on_progress: Callable[[str, int, "
           "int], None] | None = ..., on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = "
           "...) -> mnt.pyfiction.layouts.shifted_cartesian_gate_layout | None");
    detail::register_exact<py_hexagonal_gate_layout>(
        m, "def exact(network: mnt.pyfiction.networks.technology_network, *, layout_type: "
           "type[mnt.pyfiction.layouts.hexagonal_gate_layout], statistics: exact_stats | None = None, scheme: str = "
           "..., layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ..., upper_bound_area: int = ..., "
           "upper_bound_x: int = ..., upper_bound_y: int = ..., fixed_size: bool = ..., num_threads: int = ..., "
           "synchronization_elements: bool = ..., crossings: bool = ..., border_io: bool = ..., straight_inverters: "
           "bool = ..., desynchronize: bool = ..., minimize_wires: bool = ..., minimize_crossings: bool = ..., "
           "timeout: int = ..., technology_specifics: technology_constraints = ..., on_progress: Callable[[str, int, "
           "int], None] | None = ..., on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = "
           "...) -> mnt.pyfiction.layouts.hexagonal_gate_layout | None");
    m.def(
        "exact",
        [](const py_tec_network& network, const nb::handle layout_type,
           fiction::physical_design::exact_physical_design_stats*                   statistics,
           const decltype(exact_physical_design_options{}.scheme)                   scheme,
           const decltype(exact_physical_design_options{}.layout_arrangement)       layout_arrangement,
           const decltype(exact_physical_design_options{}.upper_bound_area)         upper_bound_area,
           const decltype(exact_physical_design_options{}.upper_bound_x)            upper_bound_x,
           const decltype(exact_physical_design_options{}.upper_bound_y)            upper_bound_y,
           const decltype(exact_physical_design_options{}.fixed_size)               fixed_size,
           const decltype(exact_physical_design_options{}.num_threads)              num_threads,
           const decltype(exact_physical_design_options{}.synchronization_elements) synchronization_elements,
           const decltype(exact_physical_design_options{}.crossings)                crossings,
           const decltype(exact_physical_design_options{}.border_io)                border_io,
           const decltype(exact_physical_design_options{}.straight_inverters)       straight_inverters,
           const decltype(exact_physical_design_options{}.desynchronize)            desynchronize,
           const decltype(exact_physical_design_options{}.minimize_wires)           minimize_wires,
           const decltype(exact_physical_design_options{}.minimize_crossings)       minimize_crossings,
           const decltype(exact_physical_design_options{}.timeout)                  timeout,
           const decltype(exact_physical_design_options{}.technology_specifics)     technology_specifics,
           const decltype(exact_physical_design_options{}.on_progress)              on_progress,
           const decltype(exact_physical_design_options{}.on_worker_progress)       on_worker_progress) -> nb::object
        { throw nb::type_error("layout_type must be a supported registered gate layout class"); },
        nb::arg("network"), nb::kw_only(), nb::arg("layout_type"), nb::arg("statistics") = nullptr,
        nb::arg("scheme")                   = exact_physical_design_options{}.scheme,
        nb::arg("layout_arrangement")       = exact_physical_design_options{}.layout_arrangement,
        nb::arg("upper_bound_area")         = exact_physical_design_options{}.upper_bound_area,
        nb::arg("upper_bound_x")            = exact_physical_design_options{}.upper_bound_x,
        nb::arg("upper_bound_y")            = exact_physical_design_options{}.upper_bound_y,
        nb::arg("fixed_size")               = exact_physical_design_options{}.fixed_size,
        nb::arg("num_threads")              = exact_physical_design_options{}.num_threads,
        nb::arg("synchronization_elements") = exact_physical_design_options{}.synchronization_elements,
        nb::arg("crossings")                = exact_physical_design_options{}.crossings,
        nb::arg("border_io")                = exact_physical_design_options{}.border_io,
        nb::arg("straight_inverters")       = exact_physical_design_options{}.straight_inverters,
        nb::arg("desynchronize")            = exact_physical_design_options{}.desynchronize,
        nb::arg("minimize_wires")           = exact_physical_design_options{}.minimize_wires,
        nb::arg("minimize_crossings")       = exact_physical_design_options{}.minimize_crossings,
        nb::arg("timeout")                  = exact_physical_design_options{}.timeout,
        nb::arg("technology_specifics")     = exact_physical_design_options{}.technology_specifics,
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none());
}

}  // namespace pyfiction

#else  // FICTION_Z3_SOLVER

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Disable SMT-based exact physical design.
 * @param m Python module.
 */
void register_exact([[maybe_unused]] nb::module_& m) {}

}  // namespace pyfiction

#endif  // FICTION_Z3_SOLVER
