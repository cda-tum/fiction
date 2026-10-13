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
 * @brief Python bindings for `fiction/synthesis/technology_mapping.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/synthesis/technology_mapping.hpp>

#include <mockturtle/algorithms/emap.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/chrono.h>      // NOLINT(misc-include-cleaner): Required by nanobind return-value conversion.
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>      // NOLINT(misc-include-cleaner): Required by nanobind return-value conversion.
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Binds `technology_mapping_impl`.
 * @param m Python module.
 * @tparam Ntk Network type.
 */
template <typename Ntk>
void bind_technology_mapping_impl(nb::module_& m)
{
    m.def("technology_mapping", &fiction::synthesis::technology_mapping<Ntk>, nb::arg("network"),
          nb::arg("params") = fiction::synthesis::technology_mapping_params{}, nb::arg("stats") = nullptr,
          DOC(fiction_synthesis_technology_mapping), nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace detail

/**
 * @brief Binds `technology_mapping`.
 * @param m Python module.
 */
void bind_technology_mapping(nb::module_& m)
{
    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    nb::exception<fiction::synthesis::missing_required_gates_exception>(
        m, "missing_required_gates_exception",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    nb::class_<fiction::synthesis::technology_mapping_params>(m, "technology_mapping_params",
                                                              DOC(fiction_synthesis_technology_mapping_params))
        .def(nb::init<>(), "Default constructor.")

        .def_rw("decay", &fiction::synthesis::technology_mapping_params::decay,
                DOC(fiction_synthesis_technology_mapping_params_decay))

        .def_rw("inv", &fiction::synthesis::technology_mapping_params::inv,
                DOC(fiction_synthesis_technology_mapping_params_inv))

        .def_rw("and2", &fiction::synthesis::technology_mapping_params::and2,
                DOC(fiction_synthesis_technology_mapping_params_and2))
        .def_rw("nand2", &fiction::synthesis::technology_mapping_params::nand2,
                DOC(fiction_synthesis_technology_mapping_params_nand2))
        .def_rw("or2", &fiction::synthesis::technology_mapping_params::or2,
                DOC(fiction_synthesis_technology_mapping_params_or2))
        .def_rw("nor2", &fiction::synthesis::technology_mapping_params::nor2,
                DOC(fiction_synthesis_technology_mapping_params_nor2))
        .def_rw("xor2", &fiction::synthesis::technology_mapping_params::xor2,
                DOC(fiction_synthesis_technology_mapping_params_xor2))
        .def_rw("xnor2", &fiction::synthesis::technology_mapping_params::xnor2,
                DOC(fiction_synthesis_technology_mapping_params_xnor2))

        .def_rw("lt2", &fiction::synthesis::technology_mapping_params::lt2,
                DOC(fiction_synthesis_technology_mapping_params_lt2))
        .def_rw("gt2", &fiction::synthesis::technology_mapping_params::gt2,
                DOC(fiction_synthesis_technology_mapping_params_gt2))
        .def_rw("le2", &fiction::synthesis::technology_mapping_params::le2,
                DOC(fiction_synthesis_technology_mapping_params_le2))
        .def_rw("ge2", &fiction::synthesis::technology_mapping_params::ge2,
                DOC(fiction_synthesis_technology_mapping_params_ge2))
        .def_rw("and3", &fiction::synthesis::technology_mapping_params::and3,
                DOC(fiction_synthesis_technology_mapping_params_and3))
        .def_rw("xor_and", &fiction::synthesis::technology_mapping_params::xor_and,
                DOC(fiction_synthesis_technology_mapping_params_xor_and))
        .def_rw("or_and", &fiction::synthesis::technology_mapping_params::or_and,
                DOC(fiction_synthesis_technology_mapping_params_or_and))
        .def_rw("onehot", &fiction::synthesis::technology_mapping_params::onehot,
                DOC(fiction_synthesis_technology_mapping_params_onehot))
        .def_rw("maj3", &fiction::synthesis::technology_mapping_params::maj3,
                DOC(fiction_synthesis_technology_mapping_params_maj3))
        .def_rw("gamble", &fiction::synthesis::technology_mapping_params::gamble,
                DOC(fiction_synthesis_technology_mapping_params_gamble))
        .def_rw("dot", &fiction::synthesis::technology_mapping_params::dot,
                DOC(fiction_synthesis_technology_mapping_params_dot))
        .def_rw("mux", &fiction::synthesis::technology_mapping_params::mux,
                DOC(fiction_synthesis_technology_mapping_params_mux))
        .def_rw("and_xor", &fiction::synthesis::technology_mapping_params::and_xor,
                DOC(fiction_synthesis_technology_mapping_params_and_xor))

        ;

    nb::class_<mockturtle::emap_stats>(m, "mapper_stats", "Technology mapper results, including failure status.")
        .def_ro("mapping_error", &mockturtle::emap_stats::mapping_error)
        .def_ro("area", &mockturtle::emap_stats::area)
        .def_ro("delay", &mockturtle::emap_stats::delay)
        .def_ro("power", &mockturtle::emap_stats::power)
        .def_ro("inverters", &mockturtle::emap_stats::inverters)
        .def_ro("multioutput_gates", &mockturtle::emap_stats::multioutput_gates)
        .def_ro("time_multioutput", &mockturtle::emap_stats::time_multioutput)
        .def_ro("time_total", &mockturtle::emap_stats::time_total)
        .def_ro("round_stats", &mockturtle::emap_stats::round_stats);

    nb::class_<fiction::synthesis::technology_mapping_stats>(m, "technology_mapping_stats",
                                                             DOC(fiction_synthesis_technology_mapping_stats))
        .def(nb::init<>(), "Default constructor.")
        .def("report", &fiction::synthesis::technology_mapping_stats::report,
             DOC(fiction_synthesis_technology_mapping_stats_report))
        .def_ro("mapper_stats", &fiction::synthesis::technology_mapping_stats::mapper_stats,
                DOC(fiction_synthesis_technology_mapping_stats_mapper_stats));

    m.def("and_or_not", &fiction::synthesis::and_or_not, DOC(fiction_synthesis_and_or_not),
          nb::call_guard<nb::gil_scoped_release>());

    m.def("and_or_not_maj", &fiction::synthesis::and_or_not_maj, DOC(fiction_synthesis_and_or_not_maj),
          nb::call_guard<nb::gil_scoped_release>());

    m.def("all_standard_2_input_functions", &fiction::synthesis::all_standard_2_input_functions,
          DOC(fiction_synthesis_all_standard_2_input_functions), nb::call_guard<nb::gil_scoped_release>());

    m.def("all_standard_3_input_functions", &fiction::synthesis::all_standard_3_input_functions,
          DOC(fiction_synthesis_all_standard_3_input_functions), nb::call_guard<nb::gil_scoped_release>());

    m.def("all_supported_standard_functions", &fiction::synthesis::all_supported_standard_functions,
          DOC(fiction_synthesis_all_supported_standard_functions), nb::call_guard<nb::gil_scoped_release>());

    detail::bind_technology_mapping_impl<py_tec_network>(m);
    detail::bind_technology_mapping_impl<py_aig_network>(m);
    detail::bind_technology_mapping_impl<py_xag_network>(m);
    detail::bind_technology_mapping_impl<py_mig_network>(m);
}

}  // namespace pyfiction
