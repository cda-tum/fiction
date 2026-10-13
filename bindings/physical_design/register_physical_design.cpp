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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds `exact`.
 * @param m Python module.
 */
void bind_exact(nb::module_& m);
/**
 * @brief Binds `orthogonal`.
 * @param m Python module.
 */
void bind_orthogonal(nb::module_& m);
/**
 * @brief Binds `graph_oriented_layout_design`.
 * @param m Python module.
 */
void bind_graph_oriented_layout_design(nb::module_& m);
/**
 * @brief Binds `apply_gate_library`.
 * @param m Python module.
 */
void bind_apply_gate_library(nb::module_& m);
/**
 * @brief Binds `color_routing`.
 * @param m Python module.
 */
void bind_color_routing(nb::module_& m);
/**
 * @brief Binds `hexagonalization`.
 * @param m Python module.
 */
void bind_hexagonalization(nb::module_& m);
/**
 * @brief Binds `post_layout_optimization`.
 * @param m Python module.
 */
void bind_post_layout_optimization(nb::module_& m);
/**
 * @brief Binds `wiring_reduction`.
 * @param m Python module.
 */
void bind_wiring_reduction(nb::module_& m);
/**
 * @brief Binds `routing_utils`.
 * @param m Python module.
 */
void bind_routing_utils(nb::module_& m);
/**
 * @brief Binds `placement_utils`.
 * @param m Python module.
 */
void bind_placement_utils(nb::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.physical_design.path_finding` submodule.
 *
 * @param m Python submodule.
 */
void register_path_finding(nb::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(physical_design, m)
{
    m.doc() = "Placement and routing of logic networks into gate-level layouts.";

    // Registers the types this module names in signatures and default arguments.
    nb::module_::import_("mnt.pyfiction.layouts");
    nb::module_::import_("mnt.pyfiction.networks");
    nb::module_::import_("mnt.pyfiction.qca");
    nb::module_::import_("mnt.pyfiction.mol_qca");
    nb::module_::import_("mnt.pyfiction.inml");
    nb::module_::import_("mnt.pyfiction.sidb");

    // `networks` owns the Python class of `high_degree_fanin_exception`, but the placement algorithms of
    // this module throw it. A translator catches the exception only in the module whose code threw it,
    // because macOS does not match exception types across modules.
    nb::register_exception_translator(
        [](const std::exception_ptr& exception, void* /* unused */)
        {
            try
            {
                std::rethrow_exception(exception);
            }
            catch (const fiction::networks::high_degree_fanin_exception& error)
            {
                const auto python_class =
                    nb::module_::import_("mnt.pyfiction.networks").attr("high_degree_fanin_exception");
                PyErr_SetString(python_class.ptr(), error.what());
            }
        });

    pyfiction::bind_exact(m);
    pyfiction::bind_orthogonal(m);
    pyfiction::bind_graph_oriented_layout_design(m);
    pyfiction::bind_apply_gate_library(m);
    pyfiction::bind_color_routing(m);
    pyfiction::bind_hexagonalization(m);
    pyfiction::bind_post_layout_optimization(m);
    pyfiction::bind_wiring_reduction(m);
    pyfiction::bind_routing_utils(m);
    pyfiction::bind_placement_utils(m);
    auto path_finding = pyfiction::def_submodule(m, "path_finding", "Path finding and distance functions on layouts.");
    pyfiction::register_path_finding(path_finding);
}

#pragma GCC diagnostic pop
