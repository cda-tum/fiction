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

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/types.hpp>
#include <fiction/verification/equivalence_checking.hpp>

#include <cstdint>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Bind equivalence checking for a pair of network or layout types.
 * @tparam Spec Specification type.
 * @tparam Impl Implementation type.
 * @param m Python module.
 */
template <typename Spec, typename Impl>
void register_equivalence_checking_impl(nb::module_& m)
{
    m.def(
        "equivalence_checking",
        [](const Spec& spec, const Impl& impl, fiction::verification::equivalence_checking_stats* pst,
           const std::vector<uint32_t>& input_mapping) -> fiction::verification::eq_type
        {
            const fiction::verification::equivalence_checking_params parameters{.input_mapping = input_mapping};
            fiction::verification::equivalence_checking_stats        stats{};
            fiction::verification::equivalence_checking(spec, impl, parameters, &stats);

            if (pst != nullptr)
            {
                *pst = stats;
            }

            return stats.eq;
        },
        nb::arg("specification"), nb::arg("implementation"), nb::arg("statistics") = nullptr, nb::kw_only(),
        nb::arg("input_mapping") = std::vector<uint32_t>{},
        R"doc(Check a specification against an implementation by SAT, taking layout timing into account.
Each operand is a logic network or a gate-level layout; layout logic is extracted first. The inputs match by names
unique on both sides, then by remaining declared positions, unless `input_mapping` names the specification input of
every implementation input. The outputs match by names and positions either way. The approach was first proposed in
"Verification for Field-coupled Nanocomputing Circuits" by M. Walter, R. Wille, F. Sill Torres, D. Große, and
R. Drechsler in DAC 2020.

Args:
    specification: The specification.
    implementation: The implementation.
    statistics: Statistics object filled by the check; None discards statistics.
    input_mapping: For every implementation input in declared order, the index of the specification input it reads.
        Empty matches the inputs by names and positions, which requires equal input counts. With a mapping, several
        implementation inputs may read the same specification input, as the duplicated inputs of a planarized layout
        do.

Returns:
    `eq_type.NO` when the functions differ, the interfaces do not match, or a layout has design rule violations;
    `eq_type.STRONG` when the functions agree at equal throughput; `eq_type.WEAK` when they agree with different
    throughput.

Raises:
    ValueError: If `input_mapping` is non-empty and does not name one specification input for every implementation
        input.)doc",
        nb::call_guard<nb::gil_scoped_release>());
}

/**
 * @brief Bind one specification type against every supported implementation type.
 * @tparam Spec Specification network or layout type.
 * @param m Python module.
 */
template <typename Spec>
void register_equivalence_checking_for(nb::module_& m)
{
    register_equivalence_checking_impl<Spec, fiction::networks::named_technology_network>(m);
    register_equivalence_checking_impl<Spec, fiction::layouts::cartesian_gate_layout>(m);
    register_equivalence_checking_impl<Spec, fiction::layouts::shifted_cartesian_gate_layout>(m);
    register_equivalence_checking_impl<Spec, fiction::layouts::hexagonal_gate_layout>(m);
}

}  // namespace detail

/**
 * @brief Register equivalence checking, its result enum, and statistics.
 * @param m Python module.
 */
void register_equivalence_checking(nb::module_& m)
{
    /**
     * Result type for equivalence checking.
     */
    nb::enum_<fiction::verification::eq_type>(m, "eq_type", DOC(fiction_verification_eq_type))
        .value("NO", fiction::verification::eq_type::NO, DOC(fiction_verification_eq_type_NO))
        .value("WEAK", fiction::verification::eq_type::WEAK, DOC(fiction_verification_eq_type_WEAK))
        .value("STRONG", fiction::verification::eq_type::STRONG, DOC(fiction_verification_eq_type_STRONG))

        ;

    // Keep the GIL for statistics construction and field access; conversions can create Python objects.
    nb::class_<fiction::verification::equivalence_checking_stats>(m, "equivalence_checking_stats",
                                                                  DOC(fiction_verification_equivalence_checking_stats))
        .def(nb::init<>(), "Default constructor.")
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

    detail::register_equivalence_checking_for<fiction::networks::named_technology_network>(m);
    detail::register_equivalence_checking_for<fiction::layouts::cartesian_gate_layout>(m);
    detail::register_equivalence_checking_for<fiction::layouts::shifted_cartesian_gate_layout>(m);
    detail::register_equivalence_checking_for<fiction::layouts::hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
