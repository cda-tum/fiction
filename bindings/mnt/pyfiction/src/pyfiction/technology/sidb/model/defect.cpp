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
// Created by marcel on 19.09.23.
//

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/model/defect.hpp>

#include <cstdint>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void defect(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::sidb::model::defect_type>(m, "sidb_defect_type", DOC(fiction_sidb_model_defect_type))
        .value("NONE", fiction::sidb::model::defect_type::NONE, DOC(fiction_sidb_model_defect_type_NONE))
        .value("DB", fiction::sidb::model::defect_type::DB, DOC(fiction_sidb_model_defect_type_DB))
        .value("SI_VACANCY", fiction::sidb::model::defect_type::SI_VACANCY,
               DOC(fiction_sidb_model_defect_type_SI_VACANCY))
        .value("SINGLE_DIHYDRIDE", fiction::sidb::model::defect_type::SINGLE_DIHYDRIDE,
               DOC(fiction_sidb_model_defect_type_SINGLE_DIHYDRIDE))
        .value("DIHYDRIDE_PAIR", fiction::sidb::model::defect_type::DIHYDRIDE_PAIR,
               DOC(fiction_sidb_model_defect_type_DIHYDRIDE_PAIR))
        .value("ONE_BY_ONE", fiction::sidb::model::defect_type::ONE_BY_ONE,
               DOC(fiction_sidb_model_defect_type_ONE_BY_ONE))
        .value("THREE_BY_ONE", fiction::sidb::model::defect_type::THREE_BY_ONE,
               DOC(fiction_sidb_model_defect_type_THREE_BY_ONE))
        .value("SILOXANE", fiction::sidb::model::defect_type::SILOXANE, DOC(fiction_sidb_model_defect_type_SILOXANE))
        .value("RAISED_SI", fiction::sidb::model::defect_type::RAISED_SI, DOC(fiction_sidb_model_defect_type_RAISED_SI))
        .value("MISSING_DIMER", fiction::sidb::model::defect_type::MISSING_DIMER,
               DOC(fiction_sidb_model_defect_type_MISSING_DIMER))
        .value("ETCH_PIT", fiction::sidb::model::defect_type::ETCH_PIT, DOC(fiction_sidb_model_defect_type_ETCH_PIT))
        .value("STEP_EDGE", fiction::sidb::model::defect_type::STEP_EDGE, DOC(fiction_sidb_model_defect_type_STEP_EDGE))
        .value("GUNK", fiction::sidb::model::defect_type::GUNK, DOC(fiction_sidb_model_defect_type_GUNK))
        .value("UNKNOWN", fiction::sidb::model::defect_type::UNKNOWN, DOC(fiction_sidb_model_defect_type_UNKNOWN))

        ;

    py::class_<fiction::sidb::model::defect>(m, "sidb_defect", DOC(fiction_sidb_model_defect))
        .def(py::init<const fiction::sidb::model::defect_type, const int64_t, const double, const double>(),
             py::arg("defect_type") = fiction::sidb::model::defect_type::UNKNOWN, py::arg("electric_charge") = 0.0,
             py::arg("relative_permittivity") = 0.0, py::arg("screening_distance") = 0.0)

        .def_ro("type", &fiction::sidb::model::defect::type, DOC(fiction_sidb_model_defect_type))
        .def_ro("charge", &fiction::sidb::model::defect::charge, DOC(fiction_sidb_model_defect_charge))
        .def_ro("epsilon_r", &fiction::sidb::model::defect::epsilon_r, DOC(fiction_sidb_model_defect_epsilon_r))
        .def_ro("lambda_tf", &fiction::sidb::model::defect::lambda_tf, DOC(fiction_sidb_model_defect_lambda_tf))

        // NOLINTNEXTLINE(misc-redundant-expression)
        .def(py::self == py::self, py::arg("rhs"), DOC(fiction_sidb_model_defect_operator_eq))
        // `operator!=` is compiler-synthesized from the defaulted `operator==` (C++20 rewritten candidates), so
        // `pybind11_mkdoc` no longer generates a docstring symbol for it; the docstring is inlined instead.
        // NOLINTNEXTLINE(misc-redundant-expression)
        .def(py::self != py::self, py::arg("rhs"),
             "This operator compares two `sidb_defect` instances for inequality. It uses the `operator==` to "
             "check if the two instances are equal and returns the negation of the result.\n\n"
             "Parameter ``rhs``:\n"
             "    `sidb_defect` instance to compare against.")

        ;

    m.def("is_charged_defect_type", &fiction::sidb::model::is_charged_defect_type, py::arg("defect"),
          DOC(fiction_sidb_model_is_charged_defect_type));
    m.def("is_neutral_defect_type", &fiction::sidb::model::is_neutral_defect_type, py::arg("defect"),
          DOC(fiction_sidb_model_is_neutral_defect_type));

    m.def("is_positively_charged_defect", &fiction::sidb::model::is_positively_charged_defect, py::arg("defect"),
          DOC(fiction_sidb_model_is_positively_charged_defect));
    m.def("is_negatively_charged_defect", &fiction::sidb::model::is_negatively_charged_defect, py::arg("defect"),
          DOC(fiction_sidb_model_is_negatively_charged_defect));
    m.def("is_neutrally_charged_defect", &fiction::sidb::model::is_neutrally_charged_defect, py::arg("defect"),
          DOC(fiction_sidb_model_is_neutrally_charged_defect));

    m.def("defect_extent", &fiction::sidb::model::defect_extent, py::arg("defect"),
          py::arg("charged_defect_spacing_overwrite"), py::arg("neutral_defect_spacing_overwrite"),
          DOC(fiction_sidb_model_defect_extent));
}

}  // namespace pyfiction
