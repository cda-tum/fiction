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
 * @brief Python bindings for `fiction/verification/equivalence_checking.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/verification/equivalence_checking.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/**
 * Bind equivalence checking for a pair of network or layout types.
 * @tparam Spec Specification type.
 * @tparam Impl Implementation type.
 * @param m Python module.
 */
template <typename Spec, typename Impl>
void equivalence_checking_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "equivalence_checking",
        [](const Spec& spec, const Impl& impl,
           fiction::verification::equivalence_checking_stats* pst = nullptr) -> fiction::verification::eq_type
        {
            fiction::verification::equivalence_checking_stats stats{};
            fiction::verification::equivalence_checking(spec, impl, &stats);

            if (pst != nullptr)
            {
                *pst = stats;
            }

            return stats.eq;
        },
        py::arg("specification"), py::arg("implementation"), py::arg("statistics") = nullptr,
        DOC(fiction_verification_equivalence_checking));
}

/**
 * Bind one specification type against every supported implementation type.
 * @tparam Spec Specification network or layout type.
 * @param m Python module.
 */
template <typename Spec>
void equivalence_checking_for(nanobind::module_& m)
{
    equivalence_checking_impl<Spec, py_logic_network>(m);
    equivalence_checking_impl<Spec, py_cartesian_gate_layout>(m);
    equivalence_checking_impl<Spec, py_shifted_cartesian_gate_layout>(m);
    equivalence_checking_impl<Spec, py_hexagonal_gate_layout>(m);
    equivalence_checking_impl<Spec, py_odd_row_cartesian_gate_layout>(m);
    equivalence_checking_impl<Spec, py_even_row_cartesian_gate_layout>(m);
    equivalence_checking_impl<Spec, py_even_column_cartesian_gate_layout>(m);
    equivalence_checking_impl<Spec, py_odd_row_hex_gate_layout>(m);
    equivalence_checking_impl<Spec, py_odd_column_hex_gate_layout>(m);
    equivalence_checking_impl<Spec, py_even_column_hex_gate_layout>(m);
}

}  // namespace detail

/** Register equivalence checking, its result enum, and statistics.
 * @param m Python module.
 */
void equivalence_checking(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    /**
     * Result type for equivalence checking.
     */
    py::enum_<fiction::verification::eq_type>(m, "eq_type", DOC(fiction_verification_eq_type))
        .value("NO", fiction::verification::eq_type::NO, DOC(fiction_verification_eq_type_NO))
        .value("WEAK", fiction::verification::eq_type::WEAK, DOC(fiction_verification_eq_type_WEAK))
        .value("STRONG", fiction::verification::eq_type::STRONG, DOC(fiction_verification_eq_type_STRONG))

        ;

    py::class_<fiction::verification::equivalence_checking_stats>(m, "equivalence_checking_stats",
                                                                  DOC(fiction_verification_equivalence_checking_stats))
        .def(py::init<>(), "Default constructor.")
        .def_ro("eq", &fiction::verification::equivalence_checking_stats::eq,
                DOC(fiction_verification_equivalence_checking_stats_eq))
        .def_ro("tp_spec", &fiction::verification::equivalence_checking_stats::tp_spec,
                DOC(fiction_verification_equivalence_checking_stats_tp_spec))
        .def_ro("tp_impl", &fiction::verification::equivalence_checking_stats::tp_impl,
                DOC(fiction_verification_equivalence_checking_stats_tp_impl))
        .def_ro("tp_diff", &fiction::verification::equivalence_checking_stats::tp_diff,
                DOC(fiction_verification_equivalence_checking_stats_tp_diff))
        .def_ro("counter_example", &fiction::verification::equivalence_checking_stats::counter_example,
                DOC(fiction_verification_equivalence_checking_stats_counter_example))
        .def_ro("runtime", &fiction::verification::equivalence_checking_stats::runtime,
                DOC(fiction_verification_equivalence_checking_stats_runtime))
        .def_ro("spec_drv_stats", &fiction::verification::equivalence_checking_stats::spec_drv_stats,
                DOC(fiction_verification_equivalence_checking_stats_spec_drv_stats))
        .def_ro("impl_drv_stats", &fiction::verification::equivalence_checking_stats::impl_drv_stats,
                DOC(fiction_verification_equivalence_checking_stats_impl_drv_stats))

        ;

    detail::equivalence_checking_for<py_logic_network>(m);
    detail::equivalence_checking_for<py_cartesian_gate_layout>(m);
    detail::equivalence_checking_for<py_shifted_cartesian_gate_layout>(m);
    detail::equivalence_checking_for<py_hexagonal_gate_layout>(m);
    detail::equivalence_checking_for<py_odd_row_cartesian_gate_layout>(m);
    detail::equivalence_checking_for<py_even_row_cartesian_gate_layout>(m);
    detail::equivalence_checking_for<py_even_column_cartesian_gate_layout>(m);
    detail::equivalence_checking_for<py_odd_row_hex_gate_layout>(m);
    detail::equivalence_checking_for<py_odd_column_hex_gate_layout>(m);
    detail::equivalence_checking_for<py_even_column_hex_gate_layout>(m);
}

}  // namespace pyfiction
