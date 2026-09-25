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
 * @brief Python bindings for `fiction/technology/inml/layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/cell_grid.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/inml/layout.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers iNML magnet types and layouts.
 *
 * @param m Python module.
 */
void inml_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::inml::layout;
    using fiction::inml::magnet_type;

    py::enum_<magnet_type>(m, "inml_magnet_type", DOC(fiction_inml_magnet_type))
        .value("EMPTY", magnet_type::EMPTY, DOC(fiction_inml_magnet_type_EMPTY))
        .value("NORMAL", magnet_type::NORMAL, DOC(fiction_inml_magnet_type_NORMAL))
        .value("INPUT", magnet_type::INPUT, DOC(fiction_inml_magnet_type_INPUT))
        .value("OUTPUT", magnet_type::OUTPUT, DOC(fiction_inml_magnet_type_OUTPUT))
        .value("SLANTED_EDGE_UP_MAGNET", magnet_type::SLANTED_EDGE_UP_MAGNET,
               DOC(fiction_inml_magnet_type_SLANTED_EDGE_UP_MAGNET))
        .value("SLANTED_EDGE_DOWN_MAGNET", magnet_type::SLANTED_EDGE_DOWN_MAGNET,
               DOC(fiction_inml_magnet_type_SLANTED_EDGE_DOWN_MAGNET))
        .value("INVERTER_MAGNET", magnet_type::INVERTER_MAGNET, DOC(fiction_inml_magnet_type_INVERTER_MAGNET))
        .value("CROSSWIRE_MAGNET", magnet_type::CROSSWIRE_MAGNET, DOC(fiction_inml_magnet_type_CROSSWIRE_MAGNET))
        .value("FANOUT_COUPLER_MAGNET", magnet_type::FANOUT_COUPLER_MAGNET,
               DOC(fiction_inml_magnet_type_FANOUT_COUPLER_MAGNET));

    py::class_<layout, py_cartesian_layout> cls(m, "inml_layout", DOC(fiction_inml_layout));

    detail::bind_clocked_constructors(cls);
    detail::bind_cell_grid(cls);
    detail::bind_tile_clocking(cls);
}

}  // namespace pyfiction
