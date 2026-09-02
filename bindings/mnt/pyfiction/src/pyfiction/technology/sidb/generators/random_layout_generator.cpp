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
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/sidb/generators/random_layout_generator.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void random_layout_generator_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("generate_random_sidb_layout", &fiction::sidb::generators::generate_random_layout<Lyt>, py::arg("params"),
          py::arg("lyt_skeleton") = std::nullopt, DOC(fiction_sidb_generators_generate_random_layout));

    m.def("generate_multiple_random_sidb_layouts", &fiction::sidb::generators::generate_multiple_random_layouts<Lyt>,
          py::arg("params"), py::arg("lyt_skeleton") = std::nullopt,
          DOC(fiction_sidb_generators_generate_multiple_random_layouts));
}

}  // namespace detail

void random_layout_generator(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<typename fiction::sidb::generators::generate_random_layout_params<
        fiction::layouts::coords::offset>::positive_charges>(
        m, "positive_charges", DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges))
        .value("ALLOWED",
               fiction::sidb::generators::generate_random_layout_params<
                   fiction::layouts::coords::offset>::positive_charges::ALLOWED,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_ALLOWED))
        .value("FORBIDDEN",
               fiction::sidb::generators::generate_random_layout_params<
                   fiction::layouts::coords::offset>::positive_charges::FORBIDDEN,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_FORBIDDEN))
        .value("MAY_OCCUR",
               fiction::sidb::generators::generate_random_layout_params<
                   fiction::layouts::coords::offset>::positive_charges::MAY_OCCUR,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_MAY_OCCUR));

    /**
     * Parameters.
     */
    py::class_<fiction::sidb::generators::generate_random_layout_params<fiction::layouts::coords::offset>>(
        m, "generate_random_sidb_layout_params", DOC(fiction_sidb_generators_generate_random_layout_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("coordinate_pair",
                &fiction::sidb::generators::generate_random_layout_params<
                    fiction::layouts::coords::offset>::coordinate_pair,
                DOC(fiction_sidb_generators_generate_random_layout_params_coordinate_pair))
        .def_rw("number_of_sidbs",
                &fiction::sidb::generators::generate_random_layout_params<
                    fiction::layouts::coords::offset>::number_of_sidbs,
                DOC(fiction_sidb_generators_generate_random_layout_params_number_of_sidbs))
        .def_rw(
            "positive_sidbs",
            &fiction::sidb::generators::generate_random_layout_params<fiction::layouts::coords::offset>::positive_sidbs,
            DOC(fiction_sidb_generators_generate_random_layout_params_positive_sidbs))
        .def_rw("simulation_parameters",
                &fiction::sidb::generators::generate_random_layout_params<fiction::layouts::coords::offset>::sim_params,
                DOC(fiction_sidb_generators_generate_random_layout_params_sim_params))
        .def_rw("maximal_attempts",
                &fiction::sidb::generators::generate_random_layout_params<
                    fiction::layouts::coords::offset>::maximal_attempts,
                DOC(fiction_sidb_generators_generate_random_layout_params_maximal_attempts))
        .def_rw("number_of_unique_generated_layouts",
                &fiction::sidb::generators::generate_random_layout_params<
                    fiction::layouts::coords::offset>::number_of_unique_generated_layouts,
                DOC(fiction_sidb_generators_generate_random_layout_params_number_of_unique_generated_layouts))
        .def_rw("maximal_attempts_for_multiple_layouts",
                &fiction::sidb::generators::generate_random_layout_params<
                    fiction::layouts::coords::offset>::maximal_attempts_for_multiple_layouts,
                DOC(fiction_sidb_generators_generate_random_layout_params_maximal_attempts_for_multiple_layouts));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::random_layout_generator_impl<py_sidb_100_lattice>(m);
    detail::random_layout_generator_impl<py_sidb_111_lattice>(m);
    detail::random_layout_generator_impl<py_sidb_layout>(m);
}

}  // namespace pyfiction
