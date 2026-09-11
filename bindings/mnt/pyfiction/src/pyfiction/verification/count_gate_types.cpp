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
 * @brief Python bindings for counting the gate types of logic networks and gate-level layouts.
 * @author Marcel Walter (marcelwa)
 * @author OpenAI (Codex)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/verification/count_gate_types.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename NtkOrLyt>
void count_gate_types(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "count_gate_types",
        [](const NtkOrLyt& ntk_or_lyt)
        {
            fiction::verification::count_gate_types_stats stats{};
            fiction::verification::count_gate_types(ntk_or_lyt, &stats);
            return stats;
        },
        py::arg("ntk_or_lyt"), DOC(fiction_verification_count_gate_types));
}

}  // namespace detail

void count_gate_types(nanobind::module_& m)
{
    namespace py = nanobind;

    using stats = fiction::verification::count_gate_types_stats;

    py::class_<stats>(m, "count_gate_types_stats", "The number of nodes of each gate type in a network or layout.")
        .def(py::init<>(), "Default constructor.")
        .def(
            "report",
            [](const stats& st, const bool detailed)
            {
                std::ostringstream os{};
                st.report(os, detailed);
                return os.str();
            },
            py::arg("detailed") = false, DOC(fiction_verification_count_gate_types_stats_report))
        .def_ro("num_fanout", &stats::num_fanout)
        .def_ro("num_buf", &stats::num_buf)
        .def_ro("num_inv", &stats::num_inv)
        .def_ro("num_and2", &stats::num_and2)
        .def_ro("num_or2", &stats::num_or2)
        .def_ro("num_nand2", &stats::num_nand2)
        .def_ro("num_nor2", &stats::num_nor2)
        .def_ro("num_xor2", &stats::num_xor2)
        .def_ro("num_xnor2", &stats::num_xnor2)
        .def_ro("num_lt2", &stats::num_lt2)
        .def_ro("num_gt2", &stats::num_gt2)
        .def_ro("num_le2", &stats::num_le2)
        .def_ro("num_ge2", &stats::num_ge2)
        .def_ro("num_and3", &stats::num_and3)
        .def_ro("num_xor_and", &stats::num_xor_and)
        .def_ro("num_or_and", &stats::num_or_and)
        .def_ro("num_onehot", &stats::num_onehot)
        .def_ro("num_maj3", &stats::num_maj3)
        .def_ro("num_gamble", &stats::num_gamble)
        .def_ro("num_dot", &stats::num_dot)
        .def_ro("num_mux", &stats::num_mux)
        .def_ro("num_and_xor", &stats::num_and_xor)
        .def_ro("num_other", &stats::num_other);

    detail::count_gate_types<py_logic_network>(m);
    detail::count_gate_types<py_aig_network>(m);
    detail::count_gate_types<py_xag_network>(m);
    detail::count_gate_types<py_mig_network>(m);
    detail::count_gate_types<py_cartesian_gate_layout>(m);
    detail::count_gate_types<py_shifted_cartesian_gate_layout>(m);
    detail::count_gate_types<py_hexagonal_gate_layout>(m);
    detail::count_gate_types<py_odd_row_cartesian_gate_layout>(m);
    detail::count_gate_types<py_even_row_cartesian_gate_layout>(m);
    detail::count_gate_types<py_even_column_cartesian_gate_layout>(m);
    detail::count_gate_types<py_odd_row_hex_gate_layout>(m);
    detail::count_gate_types<py_odd_column_hex_gate_layout>(m);
    detail::count_gate_types<py_even_column_hex_gate_layout>(m);
}

}  // namespace pyfiction
