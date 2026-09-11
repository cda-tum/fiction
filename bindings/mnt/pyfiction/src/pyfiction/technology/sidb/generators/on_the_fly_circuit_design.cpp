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
#include "pyfiction/types.hpp"

#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp>
#include <fiction/technology/sidb/on_the_fly_gate_library.hpp>

#include <fmt/format.h>

#include <stdexcept>

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers SiDB circuit design and its gate-library parameters.
 *
 * @param m Python module.
 */
void on_the_fly_circuit_design(nanobind::module_& m)
{
    namespace py = nanobind;

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
    using circuit_params = fiction::sidb::generators::on_the_fly_circuit_design_params;

    py::enum_<complex_policy>(m, "complex_gate_design_policy",
                              DOC(fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy))
        .value("USING_PREDEFINED", complex_policy::USING_PREDEFINED,
               DOC(fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy_USING_PREDEFINED))
        .value("DESIGN_ON_THE_FLY", complex_policy::DESIGN_ON_THE_FLY,
               DOC(fiction_sidb_on_the_fly_gate_library_params_complex_gate_design_policy_DESIGN_ON_THE_FLY));

    py::class_<library_params>(m, "sidb_on_the_fly_gate_library_params",
                               DOC(fiction_sidb_on_the_fly_gate_library_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("design_gate_params", &library_params::design_gate_params,
                DOC(fiction_sidb_on_the_fly_gate_library_params_design_gate_params))
        .def_rw("canvas_sidb_complex_gates", &library_params::canvas_sidb_complex_gates,
                DOC(fiction_sidb_on_the_fly_gate_library_params_canvas_sidb_complex_gates))
        .def_rw("using_predefined_crossing_and_double_wire_if_possible",
                &library_params::using_predefined_crossing_and_double_wire_if_possible,
                DOC(fiction_sidb_on_the_fly_gate_library_params_using_predefined_crossing_and_double_wire_if_possible))
        .def_rw("influence_radius_charged_defects", &library_params::influence_radius_charged_defects,
                DOC(fiction_sidb_on_the_fly_gate_library_params_influence_radius_charged_defects));

    py::class_<circuit_params>(m, "on_the_fly_sidb_circuit_design_params",
                               DOC(fiction_sidb_generators_on_the_fly_circuit_design_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("sidb_on_the_fly_gate_library_parameters", &circuit_params::sidb_on_the_fly_gate_library_parameters,
                DOC(fiction_sidb_generators_on_the_fly_circuit_design_params_sidb_on_the_fly_gate_library_parameters));

    m.def(
        "on_the_fly_sidb_circuit_design",
        [](const py_hexagonal_gate_layout& layout, const circuit_params params)
        {
            // Gate layouts share storage on copy; clone before releasing the GIL so Python edits cannot race the
            // design.
            const auto                   snapshot = layout.clone();
            const py::gil_scoped_release release{};

            try
            {
                return fiction::sidb::generators::on_the_fly_circuit_design(snapshot, params);
            }
            catch (const fiction::fcn::unsupported_gate_type_exception<py_offset_coordinate>& error)
            {
                throw std::invalid_argument{fmt::format("Unsupported gate type at tile {}", error.where())};
            }
            catch (const fiction::fcn::unsupported_gate_orientation_exception<py_offset_coordinate,
                                                                              fiction::fcn::port_direction>& error)
            {
                throw std::invalid_argument{fmt::format("Unsupported gate orientation at tile {}", error.where())};
            }
        },
        py::arg("layout"), py::arg("params") = circuit_params{},
        R"doc(Design an SiDB circuit from a placed and routed hexagonal gate-level layout.

The layout and parameters are copied before releasing the GIL. The input layout is not modified.
This function does not perform placement and routing or accept a defective surface.
The search has no timeout; use a separate process when cancellation is required.

Args:
    layout: A hexagonal gate-level layout with supported Bestagon port orientations.
    params: Circuit and individual-gate design parameters.

Returns:
    An SiDB layout that can be exported with write_sqd_layout or write_sidb_layout_svg.

Raises:
    RuntimeError: A gate cannot be designed with the supplied parameters.
    ValueError: A gate type or orientation is unsupported, or gate parameters are invalid.
)doc");
}

}  // namespace pyfiction
