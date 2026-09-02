/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 02.06.22.
//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include <nanobind/nanobind.h>

namespace pyfiction
{

void register_layouts(nanobind::module_& m);
void register_networks(nanobind::module_& m);
void register_sidb_model(nanobind::module_& m);
void register_sidb_surfaces(nanobind::module_& m);
void register_sidb_simulation(nanobind::module_& m);
void register_fcn(nanobind::module_& m);
void register_layouts_io(nanobind::module_& m);
void register_fcn_io(nanobind::module_& m);
void register_qca_io(nanobind::module_& m);
void register_inml_io(nanobind::module_& m);
void register_sidb_io(nanobind::module_& m);
void register_sidb_simulation_io(nanobind::module_& m);
void register_utils(nanobind::module_& m);
void register_synthesis(nanobind::module_& m);
void register_verification(nanobind::module_& m);
void register_path_finding(nanobind::module_& m);
void register_physical_design(nanobind::module_& m);
void register_sidb_simulation_engines(nanobind::module_& m);
void register_sidb_simulation_logic(nanobind::module_& m);
void register_sidb_simulation_analysis(nanobind::module_& m);
void register_sidb_simulation_defects(nanobind::module_& m);
void register_sidb_generators(nanobind::module_& m);

}  // namespace pyfiction

NB_MODULE(pyfiction, m)
{
    // docstring
    m.doc() = "Python bindings for fiction, a framework for Design Automation for Field-coupled Nanotechnologies";

    // The order is load-bearing: a type has to be registered before anything that names it in a
    // signature or a default argument. Types first, then the readers and writers, then the
    // utilities, then the algorithms that build on all of them.
    pyfiction::register_layouts(m);
    pyfiction::register_networks(m);
    pyfiction::register_sidb_model(m);
    pyfiction::register_sidb_surfaces(m);
    pyfiction::register_sidb_simulation(m);
    pyfiction::register_fcn(m);
    pyfiction::register_layouts_io(m);
    pyfiction::register_fcn_io(m);
    pyfiction::register_qca_io(m);
    pyfiction::register_inml_io(m);
    pyfiction::register_sidb_io(m);
    pyfiction::register_sidb_simulation_io(m);
    pyfiction::register_utils(m);
    pyfiction::register_synthesis(m);
    pyfiction::register_verification(m);
    pyfiction::register_path_finding(m);
    pyfiction::register_physical_design(m);
    pyfiction::register_sidb_simulation_engines(m);
    pyfiction::register_sidb_simulation_logic(m);
    pyfiction::register_sidb_simulation_analysis(m);
    pyfiction::register_sidb_simulation_defects(m);
    pyfiction::register_sidb_generators(m);
}

#pragma GCC diagnostic pop
