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
 * @brief Binds technology mapping helpers.
 * @param m Python module.
 * @tparam Ntk Network type.
 */
template <typename Ntk>
void register_technology_mapping_impl(nb::module_& m)
{
    m.def(
        "technology_mapping",
        [](const Ntk& network, const decltype(fiction::synthesis::technology_mapping_params{}.decay) decay,
           const decltype(fiction::synthesis::technology_mapping_params{}.inv)     inv,
           const decltype(fiction::synthesis::technology_mapping_params{}.and2)    and2,
           const decltype(fiction::synthesis::technology_mapping_params{}.nand2)   nand2,
           const decltype(fiction::synthesis::technology_mapping_params{}.or2)     or2,
           const decltype(fiction::synthesis::technology_mapping_params{}.nor2)    nor2,
           const decltype(fiction::synthesis::technology_mapping_params{}.xor2)    xor2,
           const decltype(fiction::synthesis::technology_mapping_params{}.xnor2)   xnor2,
           const decltype(fiction::synthesis::technology_mapping_params{}.lt2)     lt2,
           const decltype(fiction::synthesis::technology_mapping_params{}.gt2)     gt2,
           const decltype(fiction::synthesis::technology_mapping_params{}.le2)     le2,
           const decltype(fiction::synthesis::technology_mapping_params{}.ge2)     ge2,
           const decltype(fiction::synthesis::technology_mapping_params{}.and3)    and3,
           const decltype(fiction::synthesis::technology_mapping_params{}.xor_and) xor_and,
           const decltype(fiction::synthesis::technology_mapping_params{}.or_and)  or_and,
           const decltype(fiction::synthesis::technology_mapping_params{}.onehot)  onehot,
           const decltype(fiction::synthesis::technology_mapping_params{}.maj3)    maj3,
           const decltype(fiction::synthesis::technology_mapping_params{}.gamble)  gamble,
           const decltype(fiction::synthesis::technology_mapping_params{}.dot)     dot,
           const decltype(fiction::synthesis::technology_mapping_params{}.mux)     mux,
           const decltype(fiction::synthesis::technology_mapping_params{}.and_xor) and_xor,
           fiction::synthesis::technology_mapping_stats*                           stats)
        {
            fiction::synthesis::technology_mapping_params params{};
            params.decay   = decay;
            params.inv     = inv;
            params.and2    = and2;
            params.nand2   = nand2;
            params.or2     = or2;
            params.nor2    = nor2;
            params.xor2    = xor2;
            params.xnor2   = xnor2;
            params.lt2     = lt2;
            params.gt2     = gt2;
            params.le2     = le2;
            params.ge2     = ge2;
            params.and3    = and3;
            params.xor_and = xor_and;
            params.or_and  = or_and;
            params.onehot  = onehot;
            params.maj3    = maj3;
            params.gamble  = gamble;
            params.dot     = dot;
            params.mux     = mux;
            params.and_xor = and_xor;
            const nb::gil_scoped_release release{};
            return fiction::synthesis::technology_mapping<Ntk>(network, params, stats);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("decay") = fiction::synthesis::technology_mapping_params{}.decay,
        nb::arg("inv")     = fiction::synthesis::technology_mapping_params{}.inv,
        nb::arg("and2")    = fiction::synthesis::technology_mapping_params{}.and2,
        nb::arg("nand2")   = fiction::synthesis::technology_mapping_params{}.nand2,
        nb::arg("or2")     = fiction::synthesis::technology_mapping_params{}.or2,
        nb::arg("nor2")    = fiction::synthesis::technology_mapping_params{}.nor2,
        nb::arg("xor2")    = fiction::synthesis::technology_mapping_params{}.xor2,
        nb::arg("xnor2")   = fiction::synthesis::technology_mapping_params{}.xnor2,
        nb::arg("lt2")     = fiction::synthesis::technology_mapping_params{}.lt2,
        nb::arg("gt2")     = fiction::synthesis::technology_mapping_params{}.gt2,
        nb::arg("le2")     = fiction::synthesis::technology_mapping_params{}.le2,
        nb::arg("ge2")     = fiction::synthesis::technology_mapping_params{}.ge2,
        nb::arg("and3")    = fiction::synthesis::technology_mapping_params{}.and3,
        nb::arg("xor_and") = fiction::synthesis::technology_mapping_params{}.xor_and,
        nb::arg("or_and")  = fiction::synthesis::technology_mapping_params{}.or_and,
        nb::arg("onehot")  = fiction::synthesis::technology_mapping_params{}.onehot,
        nb::arg("maj3")    = fiction::synthesis::technology_mapping_params{}.maj3,
        nb::arg("gamble")  = fiction::synthesis::technology_mapping_params{}.gamble,
        nb::arg("dot")     = fiction::synthesis::technology_mapping_params{}.dot,
        nb::arg("mux")     = fiction::synthesis::technology_mapping_params{}.mux,
        nb::arg("and_xor") = fiction::synthesis::technology_mapping_params{}.and_xor, nb::arg("stats") = nullptr,
        R"doc(Performs technology mapping on the given network. Technology mapping
is the process of replacing the gates in a network with gates from a
given technology library. This function utilizes `mockturtle::emap` to
perform the technology mapping. This function is a wrapper around that
interface to provide a more convenient usage.

Args:
    ntk: Input logic network.
    pst: Technology mapping statistics.

Returns:
    Mapped network exclusively using gates from the provided library.

Raises:
    missing_required_gates_exception: if the technology library does
                                      not contain required gates for
                                      the base network type (e.g., AIG
                                      requires INV and AND; XAG
                                      requires INV, AND, and XOR; MIG
                                      requires INV and MAJ).

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace detail

/**
 * @brief Binds technology mapping helpers.
 * @param m Python module.
 */
void register_technology_mapping(nb::module_& m)
{
    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    nb::exception<fiction::synthesis::missing_required_gates_exception>(
        m, "missing_required_gates_exception",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

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

    m.def(
        "and_or_not",
        []()
        {
            const auto params = fiction::synthesis::and_or_not();
            nb::dict   options{};
            options["decay"]   = params.decay;
            options["inv"]     = params.inv;
            options["and2"]    = params.and2;
            options["nand2"]   = params.nand2;
            options["or2"]     = params.or2;
            options["nor2"]    = params.nor2;
            options["xor2"]    = params.xor2;
            options["xnor2"]   = params.xnor2;
            options["lt2"]     = params.lt2;
            options["gt2"]     = params.gt2;
            options["le2"]     = params.le2;
            options["ge2"]     = params.ge2;
            options["and3"]    = params.and3;
            options["xor_and"] = params.xor_and;
            options["or_and"]  = params.or_and;
            options["onehot"]  = params.onehot;
            options["maj3"]    = params.maj3;
            options["gamble"]  = params.gamble;
            options["dot"]     = params.dot;
            options["mux"]     = params.mux;
            options["and_xor"] = params.and_xor;
            return options;
        },
        DOC(fiction_synthesis_and_or_not));

    m.def(
        "and_or_not_maj",
        []()
        {
            const auto params = fiction::synthesis::and_or_not_maj();
            nb::dict   options{};
            options["decay"]   = params.decay;
            options["inv"]     = params.inv;
            options["and2"]    = params.and2;
            options["nand2"]   = params.nand2;
            options["or2"]     = params.or2;
            options["nor2"]    = params.nor2;
            options["xor2"]    = params.xor2;
            options["xnor2"]   = params.xnor2;
            options["lt2"]     = params.lt2;
            options["gt2"]     = params.gt2;
            options["le2"]     = params.le2;
            options["ge2"]     = params.ge2;
            options["and3"]    = params.and3;
            options["xor_and"] = params.xor_and;
            options["or_and"]  = params.or_and;
            options["onehot"]  = params.onehot;
            options["maj3"]    = params.maj3;
            options["gamble"]  = params.gamble;
            options["dot"]     = params.dot;
            options["mux"]     = params.mux;
            options["and_xor"] = params.and_xor;
            return options;
        },
        DOC(fiction_synthesis_and_or_not_maj));

    m.def(
        "all_standard_2_input_functions",
        []()
        {
            const auto params = fiction::synthesis::all_standard_2_input_functions();
            nb::dict   options{};
            options["decay"]   = params.decay;
            options["inv"]     = params.inv;
            options["and2"]    = params.and2;
            options["nand2"]   = params.nand2;
            options["or2"]     = params.or2;
            options["nor2"]    = params.nor2;
            options["xor2"]    = params.xor2;
            options["xnor2"]   = params.xnor2;
            options["lt2"]     = params.lt2;
            options["gt2"]     = params.gt2;
            options["le2"]     = params.le2;
            options["ge2"]     = params.ge2;
            options["and3"]    = params.and3;
            options["xor_and"] = params.xor_and;
            options["or_and"]  = params.or_and;
            options["onehot"]  = params.onehot;
            options["maj3"]    = params.maj3;
            options["gamble"]  = params.gamble;
            options["dot"]     = params.dot;
            options["mux"]     = params.mux;
            options["and_xor"] = params.and_xor;
            return options;
        },
        DOC(fiction_synthesis_all_standard_2_input_functions));

    m.def(
        "all_standard_3_input_functions",
        []()
        {
            const auto params = fiction::synthesis::all_standard_3_input_functions();
            nb::dict   options{};
            options["decay"]   = params.decay;
            options["inv"]     = params.inv;
            options["and2"]    = params.and2;
            options["nand2"]   = params.nand2;
            options["or2"]     = params.or2;
            options["nor2"]    = params.nor2;
            options["xor2"]    = params.xor2;
            options["xnor2"]   = params.xnor2;
            options["lt2"]     = params.lt2;
            options["gt2"]     = params.gt2;
            options["le2"]     = params.le2;
            options["ge2"]     = params.ge2;
            options["and3"]    = params.and3;
            options["xor_and"] = params.xor_and;
            options["or_and"]  = params.or_and;
            options["onehot"]  = params.onehot;
            options["maj3"]    = params.maj3;
            options["gamble"]  = params.gamble;
            options["dot"]     = params.dot;
            options["mux"]     = params.mux;
            options["and_xor"] = params.and_xor;
            return options;
        },
        DOC(fiction_synthesis_all_standard_3_input_functions));

    m.def(
        "all_supported_standard_functions",
        []()
        {
            const auto params = fiction::synthesis::all_supported_standard_functions();
            nb::dict   options{};
            options["decay"]   = params.decay;
            options["inv"]     = params.inv;
            options["and2"]    = params.and2;
            options["nand2"]   = params.nand2;
            options["or2"]     = params.or2;
            options["nor2"]    = params.nor2;
            options["xor2"]    = params.xor2;
            options["xnor2"]   = params.xnor2;
            options["lt2"]     = params.lt2;
            options["gt2"]     = params.gt2;
            options["le2"]     = params.le2;
            options["ge2"]     = params.ge2;
            options["and3"]    = params.and3;
            options["xor_and"] = params.xor_and;
            options["or_and"]  = params.or_and;
            options["onehot"]  = params.onehot;
            options["maj3"]    = params.maj3;
            options["gamble"]  = params.gamble;
            options["dot"]     = params.dot;
            options["mux"]     = params.mux;
            options["and_xor"] = params.and_xor;
            return options;
        },
        DOC(fiction_synthesis_all_supported_standard_functions));

    detail::register_technology_mapping_impl<py_tec_network>(m);
    detail::register_technology_mapping_impl<py_aig_network>(m);
    detail::register_technology_mapping_impl<py_xag_network>(m);
    detail::register_technology_mapping_impl<py_mig_network>(m);
}

}  // namespace pyfiction
