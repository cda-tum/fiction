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
#include "pyfiction/progress.hpp"
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
        [](const py_tec_network& network, const fiction::physical_design::orthogonal_physical_design_params& parameters,
           fiction::physical_design::orthogonal_physical_design_stats* statistics, const nb::handle layout_type)
    {
        if (!layout_type.is(nb::type<Lyt>()))
        {
            throw nb::next_overload();
        }
        const nb::gil_scoped_release release{};
        return fiction::physical_design::orthogonal<Lyt, py_tec_network>(network, parameters, statistics);
    };
    if constexpr (std::is_same_v<Lyt, py_cartesian_gate_layout>)
    {
        m.def("orthogonal", design, nb::arg("network"),
              nb::arg("parameters") = fiction::physical_design::orthogonal_physical_design_params{},
              nb::arg("statistics") = nullptr, nb::kw_only(), nb::arg("layout_type") = nb::type<Lyt>(),
              nb::sig(signature), DOC(fiction_physical_design_orthogonal));
    }
    else
    {
        m.def("orthogonal", design, nb::arg("network"),
              nb::arg("parameters") = fiction::physical_design::orthogonal_physical_design_params{},
              nb::arg("statistics") = nullptr, nb::kw_only(), nb::arg("layout_type"), nb::sig(signature));
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

    nb::class_<fiction::physical_design::orthogonal_physical_design_params>(
        m, "orthogonal_params",
        pyfiction::progress_type_slots<fiction::physical_design::orthogonal_physical_design_params>(),
        DOC(fiction_physical_design_orthogonal_physical_design_params))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("number_of_clock_phases",
                &fiction::physical_design::orthogonal_physical_design_params::number_of_clock_phases,
                DOC(fiction_physical_design_orthogonal_physical_design_params_number_of_clock_phases))
        .def_rw("layout_arrangement", &fiction::physical_design::orthogonal_physical_design_params::layout_arrangement,
                DOC(fiction_physical_design_orthogonal_physical_design_params_layout_arrangement))
        .def_rw("on_progress", &fiction::physical_design::orthogonal_physical_design_params::on_progress,
                pyfiction::ON_PROGRESS_GETTER, pyfiction::CALLBACK_SETTER,
                DOC(fiction_physical_design_orthogonal_physical_design_params_on_progress));

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
        m,
        "def orthogonal(network: mnt.pyfiction.networks.technology_network, parameters: orthogonal_params = ..., "
        "statistics: orthogonal_stats | None = None, *, layout_type: type[mnt.pyfiction.layouts.cartesian_gate_layout] "
        "= mnt.pyfiction.layouts.cartesian_gate_layout) -> mnt.pyfiction.layouts.cartesian_gate_layout");
    detail::register_orthogonal<py_hexagonal_gate_layout>(
        m, "def orthogonal(network: mnt.pyfiction.networks.technology_network, parameters: orthogonal_params = ..., "
           "statistics: orthogonal_stats | None = None, *, layout_type: "
           "type[mnt.pyfiction.layouts.hexagonal_gate_layout]) -> mnt.pyfiction.layouts.hexagonal_gate_layout");
    m.def(
        "orthogonal",
        [](const py_tec_network&, const fiction::physical_design::orthogonal_physical_design_params&,
           fiction::physical_design::orthogonal_physical_design_stats*, const nb::handle) -> nb::object
        { throw nb::type_error("layout_type must be a supported registered gate layout class"); },
        nb::arg("network"), nb::arg("parameters") = fiction::physical_design::orthogonal_physical_design_params{},
        nb::arg("statistics") = nullptr, nb::kw_only(), nb::arg("layout_type"),
        nb::sig("def orthogonal(network: mnt.pyfiction.networks.technology_network, parameters: orthogonal_params = "
                "..., statistics: orthogonal_stats | None = None, *, layout_type: "
                "type[mnt.pyfiction.layouts.cartesian_gate_layout | mnt.pyfiction.layouts.hexagonal_gate_layout]) -> "
                "mnt.pyfiction.layouts.cartesian_gate_layout | mnt.pyfiction.layouts.hexagonal_gate_layout"));
}

}  // namespace pyfiction
