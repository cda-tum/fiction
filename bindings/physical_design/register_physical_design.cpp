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
 * @author Benjamin Hien (hibenj)
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
 * @brief Binds exact helpers.
 * @param m Python module.
 */
void register_exact(nb::module_& m);
/**
 * @brief Binds orthogonal helpers.
 * @param m Python module.
 */
void register_orthogonal(nb::module_& m);
/**
 * @brief Binds PLANE helpers.
 * @param m Python module.
 */
void register_plane(nb::module_& m);
/**
 * @brief Binds graph oriented layout design helpers.
 * @param m Python module.
 */
void register_graph_oriented_layout_design(nb::module_& m);
/**
 * @brief Binds apply gate library helpers.
 * @param m Python module.
 */
void register_apply_gate_library(nb::module_& m);
/**
 * @brief Binds color routing helpers.
 * @param m Python module.
 */
void register_color_routing(nb::module_& m);
/**
 * @brief Binds hexagonalization helpers.
 * @param m Python module.
 */
void register_hexagonalization(nb::module_& m);
/**
 * @brief Binds post layout optimization helpers.
 * @param m Python module.
 */
void register_post_layout_optimization(nb::module_& m);
/**
 * @brief Binds wiring reduction helpers.
 * @param m Python module.
 */
void register_wiring_reduction(nb::module_& m);
/**
 * @brief Binds routing utils helpers.
 * @param m Python module.
 */
void register_routing_utils(nb::module_& m);
/**
 * @brief Binds placement utils helpers.
 * @param m Python module.
 */
void register_placement_utils(nb::module_& m);

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

    pyfiction::register_exact(m);
    pyfiction::register_orthogonal(m);
    pyfiction::register_plane(m);
    pyfiction::register_graph_oriented_layout_design(m);
    pyfiction::register_apply_gate_library(m);
    pyfiction::register_color_routing(m);
    pyfiction::register_hexagonalization(m);
    pyfiction::register_post_layout_optimization(m);
    pyfiction::register_wiring_reduction(m);
    pyfiction::register_routing_utils(m);
    pyfiction::register_placement_utils(m);
    auto path_finding = pyfiction::def_submodule(m, "path_finding", "Path finding and distance functions on layouts.");
    pyfiction::register_path_finding(path_finding);
}

#pragma GCC diagnostic pop
