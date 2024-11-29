//
// Created by marcel on 19.09.23.
//

#ifndef PYFICTION_SIDB_DEFECTS_HPP
#define PYFICTION_SIDB_DEFECTS_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb_defects.hpp>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <cstdint>

namespace pyfiction
{

inline void sidb_defects(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::sidb_defect_type>(m, "sidb_defect_type", DOC(fiction_sidb_defect_type))
        .value("NONE", fiction::sidb_defect_type::NONE, DOC(fiction_sidb_defect_type_NONE))
        .value("DB", fiction::sidb_defect_type::DB, DOC(fiction_sidb_defect_type_DB))
        .value("SI_VACANCY", fiction::sidb_defect_type::SI_VACANCY, DOC(fiction_sidb_defect_type_SI_VACANCY))
        .value("SINGLE_DIHYDRIDE", fiction::sidb_defect_type::SINGLE_DIHYDRIDE,
               DOC(fiction_sidb_defect_type_SINGLE_DIHYDRIDE))
        .value("DIHYDRIDE_PAIR", fiction::sidb_defect_type::DIHYDRIDE_PAIR,
               DOC(fiction_sidb_defect_type_DIHYDRIDE_PAIR))
        .value("ONE_BY_ONE", fiction::sidb_defect_type::ONE_BY_ONE, DOC(fiction_sidb_defect_type_ONE_BY_ONE))
        .value("THREE_BY_ONE", fiction::sidb_defect_type::THREE_BY_ONE, DOC(fiction_sidb_defect_type_THREE_BY_ONE))
        .value("SILOXANE", fiction::sidb_defect_type::SILOXANE, DOC(fiction_sidb_defect_type_SILOXANE))
        .value("RAISED_SI", fiction::sidb_defect_type::RAISED_SI, DOC(fiction_sidb_defect_type_RAISED_SI))
        .value("MISSING_DIMER", fiction::sidb_defect_type::MISSING_DIMER, DOC(fiction_sidb_defect_type_MISSING_DIMER))
        .value("ETCH_PIT", fiction::sidb_defect_type::ETCH_PIT, DOC(fiction_sidb_defect_type_ETCH_PIT))
        .value("STEP_EDGE", fiction::sidb_defect_type::STEP_EDGE, DOC(fiction_sidb_defect_type_STEP_EDGE))
        .value("GUNK", fiction::sidb_defect_type::GUNK, DOC(fiction_sidb_defect_type_GUNK))
        .value("UNKNOWN", fiction::sidb_defect_type::UNKNOWN, DOC(fiction_sidb_defect_type_UNKNOWN))

        ;

    py::class_<fiction::sidb_defect>(m, "sidb_defect", DOC(fiction_sidb_defect))
        .def(py::init<const fiction::sidb_defect_type, const int64_t, const double, const double>(),
             py::arg("defect_type") = fiction::sidb_defect_type::UNKNOWN, py::arg("electric_charge") = 0.0,
             py::arg("relative_permittivity") = 0.0, py::arg("screening_distance") = 0.0)

        .def_readonly("type", &fiction::sidb_defect::type, DOC(fiction_sidb_defect_type))
        .def_readonly("charge", &fiction::sidb_defect::charge, DOC(fiction_sidb_defect_charge))
        .def_readonly("epsilon_r", &fiction::sidb_defect::epsilon_r, DOC(fiction_sidb_defect_epsilon_r))
        .def_readonly("lambda_tf", &fiction::sidb_defect::lambda_tf, DOC(fiction_sidb_defect_lambda_tf))

        .def(py::self == py::self, py::arg("rhs"), DOC(fiction_sidb_defect_operator_eq))
        .def(py::self != py::self, py::arg("rhs"), DOC(fiction_sidb_defect_operator_ne))

        ;

    m.def("is_charged_defect_type", &fiction::is_charged_defect_type, py::arg("defect"),
          DOC(fiction_is_charged_defect_type));
    m.def("is_neutral_defect_type", &fiction::is_neutral_defect_type, py::arg("defect"),
          DOC(fiction_is_neutral_defect_type));

    m.def("is_positively_charged_defect", &fiction::is_positively_charged_defect, py::arg("defect"),
          DOC(fiction_is_positively_charged_defect));
    m.def("is_negatively_charged_defect", &fiction::is_negatively_charged_defect, py::arg("defect"),
          DOC(fiction_is_negatively_charged_defect));
    m.def("is_neutrally_charged_defect", &fiction::is_neutrally_charged_defect, py::arg("defect"),
          DOC(fiction_is_neutrally_charged_defect));

    m.def("defect_extent", &fiction::defect_extent, py::arg("defect"), py::arg("charged_defect_spacing_overwrite"),
          py::arg("neutral_defect_spacing_overwrite"), DOC(fiction_defect_extent));
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_DEFECTS_HPP
