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
 * @brief Python bindings for SiDB circuit design from hexagonal gate-level layouts.
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp>
#include <fiction/technology/sidb/on_the_fly_gate_library.hpp>

#include <fmt/format.h>

#include <optional>
#include <stdexcept>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): converts progress callbacks
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner): enables keyword options
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner): converts gate-design canvas endpoints
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using on_the_fly_circuit_design_options = fiction::sidb::generators::on_the_fly_circuit_design_params;

/**
 * @brief Registers SiDB circuit design and its gate-library parameters.
 *
 * @param m Python module.
 */
void register_on_the_fly_circuit_design(nb::module_& m)
{
    /**
     * Parameters for the individual SiDB gate designs.
     */
    using library_params = fiction::sidb::on_the_fly_gate_library_params;
    /**
     * Choice between predefined and synthesized complex gates.
     */
    using complex_policy = library_params::complex_gate_design_policy;
    /**
     * Parameters for circuit design on a placed and routed layout.
     */
    using circuit_params = on_the_fly_circuit_design_options;

    nb::enum_<complex_policy>(m, "sidb_complex_gate_design_policy",
                              DOC(fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy))
        .value("USING_PREDEFINED", complex_policy::USING_PREDEFINED,
               DOC(fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy_USING_PREDEFINED))
        .value("DESIGN_ON_THE_FLY", complex_policy::DESIGN_ON_THE_FLY,
               DOC(fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy_DESIGN_ON_THE_FLY));

    m.def(
        "on_the_fly_sidb_circuit_design",  // NOLINTNEXTLINE(performance-unnecessary-value-param): Own parameters while
                                           // Python can mutate the original.
        [](const py_hexagonal_gate_layout& layout, const decltype(on_the_fly_circuit_design_options{}.timeout) timeout,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                              .timeout)                                    design_gate_timeout,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params
                              .base)                                       base,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params
                              .mu_minus)                                   mu_minus,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params
                              .epsilon_r)                                  epsilon_r,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params
                              .lambda_tf)                                  lambda_tf,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                              .sim_engine)                                 sim_engine,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)                threshold_bdl_interdistance,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                           minimum_distance,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                           maximum_distance,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                              .input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                              .op_condition)                               op_condition,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                              .strategy_to_analyze_operational_status)     strategy_to_analyze_operational_status,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.design_mode) design_mode,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.canvas)      canvas,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params
                              .number_of_canvas_sidbs)         number_of_canvas_sidbs,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params
                              .maximal_random_design_attempts) maximal_random_design_attempts,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params
                              .termination_cond)               termination_cond,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters.design_gate_params.on_progress) on_progress,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters
                              .canvas_sidb_complex_gates) canvas_sidb_complex_gates,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters
                              .using_predefined_crossing_and_double_wire_if_possible)
                                                                 using_predefined_crossing_and_double_wire_if_possible,
           const decltype(on_the_fly_circuit_design_options{}
                              .sidb_on_the_fly_gate_library_parameters
                              .influence_radius_charged_defects) influence_radius_charged_defects)
        {
            on_the_fly_circuit_design_options params{};
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params =
                make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            params.timeout = timeout;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.timeout =
                design_gate_timeout;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_engine =
                sim_engine;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                .input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                .input_bdl_iterator_params.input_bdl_config = input_bdl_config;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.op_condition =
                op_condition;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                .strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.design_mode = design_mode;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.canvas      = canvas;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.number_of_canvas_sidbs =
                number_of_canvas_sidbs;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.maximal_random_design_attempts =
                maximal_random_design_attempts;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.termination_cond = termination_cond;
            params.sidb_on_the_fly_gate_library_parameters.design_gate_params.on_progress      = on_progress;
            params.sidb_on_the_fly_gate_library_parameters.canvas_sidb_complex_gates = canvas_sidb_complex_gates;
            params.sidb_on_the_fly_gate_library_parameters.using_predefined_crossing_and_double_wire_if_possible =
                using_predefined_crossing_and_double_wire_if_possible;
            params.sidb_on_the_fly_gate_library_parameters.influence_radius_charged_defects =
                influence_radius_charged_defects;

            // Own the layout before releasing the GIL so Python edits cannot race the design.
            const auto                   snapshot = layout.clone();
            const nb::gil_scoped_release release{};

            try
            {
                return fiction::sidb::generators::on_the_fly_circuit_design(snapshot, params);
            }
            catch (const fiction::fcn::unsupported_gate_type_exception<py_coordinate>& error)
            {
                throw std::invalid_argument{fmt::format("Unsupported gate type at tile {}", error.where())};
            }
            catch (const fiction::fcn::unsupported_gate_orientation_exception<py_coordinate,
                                                                              fiction::fcn::port_direction>& error)
            {
                throw std::invalid_argument{fmt::format("Unsupported gate orientation at tile {}", error.where())};
            }
        },
        nb::arg("layout"), nb::kw_only(), nb::arg("timeout") = on_the_fly_circuit_design_options{}.timeout,
        nb::arg("design_gate_timeout") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.timeout,
        nb::arg("base") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params.base,
        nb::arg("mu_minus") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.input_bdl_iterator_params
                .bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") = on_the_fly_circuit_design_options{}
                                          .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                                          .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") = on_the_fly_circuit_design_options{}
                                          .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                                          .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = on_the_fly_circuit_design_options{}
                                          .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                                          .input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.operational_params
                .strategy_to_analyze_operational_status,
        nb::arg("design_mode") =
            on_the_fly_circuit_design_options{}.sidb_on_the_fly_gate_library_parameters.design_gate_params.design_mode,
        nb::arg("canvas") =
            on_the_fly_circuit_design_options{}.sidb_on_the_fly_gate_library_parameters.design_gate_params.canvas,
        nb::arg("number_of_canvas_sidbs") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.number_of_canvas_sidbs,
        nb::arg("maximal_random_design_attempts") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.design_gate_params.maximal_random_design_attempts,
        nb::arg("termination_cond") = on_the_fly_circuit_design_options{}
                                          .sidb_on_the_fly_gate_library_parameters.design_gate_params.termination_cond,
        nb::arg("on_progress").none() = nb::none(),
        nb::arg("canvas_sidb_complex_gates") =
            on_the_fly_circuit_design_options{}.sidb_on_the_fly_gate_library_parameters.canvas_sidb_complex_gates,
        nb::arg("using_predefined_crossing_and_double_wire_if_possible") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.using_predefined_crossing_and_double_wire_if_possible,
        nb::arg("influence_radius_charged_defects") =
            on_the_fly_circuit_design_options{}
                .sidb_on_the_fly_gate_library_parameters.influence_radius_charged_defects,
        R"doc(Design an SiDB circuit from a placed and routed pointy-top hexagonal gate-level layout.

The function copies the layout and keyword options before releasing the GIL. The input layout is not modified.
This function does not perform placement and routing or accept a defective surface.
The optional timeout bounds the whole circuit search in milliseconds. Deadline checks are cooperative;
use a separate process when an exact cutoff is required. A timeout never returns a partial circuit.

Args:
    layout: A pointy-top hexagonal gate-level layout with a row arrangement and supported Bestagon port orientations.
    timeout: Whole-circuit deadline in milliseconds.
    design_gate_timeout: Deadline for each individual gate in milliseconds.
    base, mu_minus, epsilon_r, lambda_tf: Physical simulation values.
    on_progress: Optional gate-design progress callback.

Returns:
    An SiDB layout that can be exported with write_sqd_layout or write_sidb_layout_svg.

Raises:
    TimeoutError: The circuit or an individual gate exceeds its timeout.
    RuntimeError: A gate cannot be designed with the supplied parameters.
    ValueError: Gate mapping uses a column arrangement, a gate type or orientation is unsupported, or gate parameters are invalid.
)doc");
}

}  // namespace pyfiction
