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
 * @brief Entry point of the `mnt.pyfiction.physical_design` extension module.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/submodule.hpp"

#include <fiction/networks/network_utils.hpp>

#include <exception>

#include <nanobind/nanobind.h>
#include <pyerrors.h>

namespace pyfiction
{

void exact(nanobind::module_& m);
void orthogonal(nanobind::module_& m);
void graph_oriented_layout_design(nanobind::module_& m);
void apply_gate_library(nanobind::module_& m);
void color_routing(nanobind::module_& m);
void hexagonalization(nanobind::module_& m);
void post_layout_optimization(nanobind::module_& m);
void wiring_reduction(nanobind::module_& m);
void routing_utils(nanobind::module_& m);
void placement_utils(nanobind::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.physical_design.path_finding` submodule.
 *
 * @param m Python submodule.
 */
void register_path_finding(nanobind::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(physical_design, m)
{
    m.doc() = "Placement and routing of logic networks into gate-level layouts.";

    // Registers the types this module names in signatures and default arguments.
    nanobind::module_::import_("mnt.pyfiction.layouts");
    nanobind::module_::import_("mnt.pyfiction.networks");
    nanobind::module_::import_("mnt.pyfiction.qca");
    nanobind::module_::import_("mnt.pyfiction.mol_qca");
    nanobind::module_::import_("mnt.pyfiction.inml");
    nanobind::module_::import_("mnt.pyfiction.sidb");

    // `networks` owns the Python class of `high_degree_fanin_exception`, but the placement algorithms of
    // this module throw it. A translator catches the exception only in the module whose code threw it,
    // because macOS does not match exception types across modules.
    nanobind::register_exception_translator(
        [](const std::exception_ptr& exception, void* /* unused */)
        {
            try
            {
                std::rethrow_exception(exception);
            }
            catch (const fiction::networks::high_degree_fanin_exception& error)
            {
                const auto python_class =
                    nanobind::module_::import_("mnt.pyfiction.networks").attr("high_degree_fanin_exception");
                PyErr_SetString(python_class.ptr(), error.what());
            }
        });

    pyfiction::exact(m);
    pyfiction::orthogonal(m);
    pyfiction::graph_oriented_layout_design(m);
    pyfiction::apply_gate_library(m);
    pyfiction::color_routing(m);
    pyfiction::hexagonalization(m);
    pyfiction::post_layout_optimization(m);
    pyfiction::wiring_reduction(m);
    pyfiction::routing_utils(m);
    pyfiction::placement_utils(m);
    auto path_finding = pyfiction::def_submodule(m, "path_finding", "Path finding and distance functions on layouts.");
    pyfiction::register_path_finding(path_finding);
}

#pragma GCC diagnostic pop
