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
 * @brief Entry point of the `mnt.pyfiction.sidb` extension module.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/submodule.hpp"

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers SiDB lattice sites, lattice geometry, and row conversions.
 *
 * @param m Python module.
 */
void lattice(nanobind::module_& m);
/**
 * @brief Registers SiDB layouts and their cell and defect operations.
 *
 * @param m Python module.
 */
void layout(nanobind::module_& m);
/**
 * @brief Registers SiDB charge distributions.
 *
 * @param m Python module.
 */
void charge_distribution(nanobind::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.model` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_model(nanobind::module_& m);
/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_simulation(nanobind::module_& m);
/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.io` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_io(nanobind::module_& m);
/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.io` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_simulation_io(nanobind::module_& m);
/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.engines` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_simulation_engines(nanobind::module_& m);
/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.logic` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_simulation_logic(nanobind::module_& m);
/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.analysis` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_simulation_analysis(nanobind::module_& m);
/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.defects` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_simulation_defects(nanobind::module_& m);
/**
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.generators` submodule.
 *
 * @param m Python submodule.
 */
void register_sidb_generators(nanobind::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(sidb, m)
{
    m.doc() = "Silicon Dangling Bond (SiDB) lattices, layouts, charge distributions, and simulation.";

    // Registers the types this module names in signatures and default arguments. `utils` registers
    // the translator that raises `TimeoutError` for `fiction::utils::timeout_error`.
    nanobind::module_::import_("mnt.pyfiction.utils");
    nanobind::module_::import_("mnt.pyfiction.layouts");
    nanobind::module_::import_("mnt.pyfiction.synthesis");

    auto model = pyfiction::def_submodule(m, "model", "Physical models and parameters of SiDB systems.");
    pyfiction::register_sidb_model(model);
    pyfiction::lattice(m);
    pyfiction::layout(m);
    pyfiction::charge_distribution(m);
    auto simulation =
        pyfiction::def_submodule(m, "simulation", "SiDB simulation engines, results, and their analysis.");
    pyfiction::register_sidb_simulation(simulation);
    auto io = pyfiction::def_submodule(m, "io", "Readers and writers of SiDB layouts.");
    pyfiction::register_sidb_io(io);
    auto simulation_io = pyfiction::def_submodule(simulation, "io", "Readers and writers of SiDB simulation results.");
    pyfiction::register_sidb_simulation_io(simulation_io);
    auto engines = pyfiction::def_submodule(simulation, "engines", "Physical simulation engines for SiDB layouts.");
    pyfiction::register_sidb_simulation_engines(engines);
    auto logic = pyfiction::def_submodule(simulation, "logic", "Operational analysis of SiDB logic.");
    pyfiction::register_sidb_simulation_logic(logic);
    auto analysis =
        pyfiction::def_submodule(simulation, "analysis", "Physical analyses of SiDB layouts built on simulation.");
    pyfiction::register_sidb_simulation_analysis(analysis);
    auto defects = pyfiction::def_submodule(simulation, "defects", "Influence of atomic defects on SiDB layouts.");
    pyfiction::register_sidb_simulation_defects(defects);
    auto generators = pyfiction::def_submodule(m, "generators", "Generators of SiDB layouts and circuits.");
    pyfiction::register_sidb_generators(generators);
}

#pragma GCC diagnostic pop
