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
 * @brief Python bindings for `fiction/technology/sidb/simulation/io/write_operational_domain.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/simulation/io/write_operational_domain.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>

#include <sstream>
#include <string>
#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using write_operational_domain_options = fiction::sidb::simulation::io::write_operational_domain_params;

namespace detail
{
/**
 * @brief Binds write operational domain helpers.
 * @param m Python module.
 */
void register_write_operational_domain(nb::module_& m)
{
    // Function pointer for writing to a file
    // Define function using function pointer
    m.def(
        "write_operational_domain",
        [](const fiction::sidb::simulation::logic::operational_domain& opdom, const std::string_view& filename,
           const decltype(write_operational_domain_options{}.operational_tag)     operational_tag,
           const decltype(write_operational_domain_options{}.non_operational_tag) non_operational_tag,
           const decltype(write_operational_domain_options{}.writing_mode)        writing_mode)
        {
            write_operational_domain_options params{};
            params.operational_tag     = operational_tag;
            params.non_operational_tag = non_operational_tag;
            params.writing_mode        = writing_mode;
            fiction::sidb::simulation::io::write_operational_domain(opdom, filename, params);
        },
        nb::arg("opdom"), nb::arg("filename"), nb::kw_only(),
        nb::arg("operational_tag")     = write_operational_domain_options{}.operational_tag,
        nb::arg("non_operational_tag") = write_operational_domain_options{}.non_operational_tag,
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode,
        R"doc(Write the operational domain to the supplied filename as CSV.
CSV columns hold the sweep-parameter values followed by the operational-status tag.
The keyword options select status tags and which samples to write.
Raises ValueError unless the domain has one to three sweep dimensions.
)doc");

    m.def(
        "write_operational_domain_to_string",
        [](const fiction::sidb::simulation::logic::operational_domain&            opdom,
           const decltype(write_operational_domain_options{}.operational_tag)     operational_tag,
           const decltype(write_operational_domain_options{}.non_operational_tag) non_operational_tag,
           const decltype(write_operational_domain_options{}.writing_mode)        writing_mode) -> std::string
        {
            write_operational_domain_options params{};
            params.operational_tag     = operational_tag;
            params.non_operational_tag = non_operational_tag;
            params.writing_mode        = writing_mode;

            std::ostringstream oss;
            fiction::sidb::simulation::io::write_operational_domain(opdom, oss, params);
            return oss.str();
        },
        nb::arg("opdom"), nb::kw_only(),
        nb::arg("operational_tag")     = write_operational_domain_options{}.operational_tag,
        nb::arg("non_operational_tag") = write_operational_domain_options{}.non_operational_tag,
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode,
        R"doc(Return the domain CSV with a header and one row per selected sample.
Columns hold the sweep-parameter values and operational-status tag; temperature domains add critical temperature.
The keyword options select status tags and which samples to write.
Raises ValueError unless the domain has one to three sweep dimensions.
)doc");
}

/**
 * @brief Binds write critical temperature domain helpers.
 * @param m Python module.
 */
void register_write_critical_temperature_domain(nb::module_& m)
{

    // Define function using function pointer
    m.def(
        "write_critical_temperature_domain",
        [](const fiction::sidb::simulation::logic::critical_temperature_domain& opdom, const std::string_view& filename,
           const decltype(write_operational_domain_options{}.operational_tag)     operational_tag,
           const decltype(write_operational_domain_options{}.non_operational_tag) non_operational_tag,
           const decltype(write_operational_domain_options{}.writing_mode)        writing_mode)
        {
            write_operational_domain_options params{};
            params.operational_tag     = operational_tag;
            params.non_operational_tag = non_operational_tag;
            params.writing_mode        = writing_mode;
            fiction::sidb::simulation::io::write_operational_domain(opdom, filename, params);
        },
        nb::arg("opdom"), nb::arg("filename"), nb::kw_only(),
        nb::arg("operational_tag")     = write_operational_domain_options{}.operational_tag,
        nb::arg("non_operational_tag") = write_operational_domain_options{}.non_operational_tag,
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode,
        R"doc(Write critical-temperature samples to a CSV file.
CSV columns hold sweep-parameter values, the operational-status tag, and critical temperature.
The keyword options select status tags and which samples to write.
Raises ValueError unless the domain has one to three sweep dimensions.
)doc");

    m.def(
        "write_critical_temperature_domain_to_string",
        [](const fiction::sidb::simulation::logic::critical_temperature_domain&   opdom,
           const decltype(write_operational_domain_options{}.operational_tag)     operational_tag,
           const decltype(write_operational_domain_options{}.non_operational_tag) non_operational_tag,
           const decltype(write_operational_domain_options{}.writing_mode)        writing_mode) -> std::string
        {
            write_operational_domain_options params{};
            params.operational_tag     = operational_tag;
            params.non_operational_tag = non_operational_tag;
            params.writing_mode        = writing_mode;

            std::ostringstream oss;
            fiction::sidb::simulation::io::write_operational_domain(opdom, oss, params);
            return oss.str();
        },
        nb::arg("opdom"), nb::kw_only(),
        nb::arg("operational_tag")     = write_operational_domain_options{}.operational_tag,
        nb::arg("non_operational_tag") = write_operational_domain_options{}.non_operational_tag,
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode,
        R"doc(Return the domain CSV with a header and one row per selected sample.
Columns hold the sweep-parameter values and operational-status tag; temperature domains add critical temperature.
The keyword options select status tags and which samples to write.
Raises ValueError unless the domain has one to three sweep dimensions.
)doc");
}

}  // namespace detail

/**
 * @brief Binds write operational domain helpers.
 * @param m Python module.
 */
void register_write_operational_domain(nb::module_& m)
{
    nb::enum_<write_operational_domain_options::sample_writing_mode>(
        m, "sample_writing_mode", DOC(fiction_sidb_simulation_io_write_operational_domain_params_sample_writing_mode))
        .value("ALL_SAMPLES", write_operational_domain_options::sample_writing_mode::ALL_SAMPLES,
               DOC(fiction_sidb_simulation_io_write_operational_domain_params_sample_writing_mode_ALL_SAMPLES))
        .value("OPERATIONAL_ONLY", write_operational_domain_options::sample_writing_mode::OPERATIONAL_ONLY,
               DOC(fiction_sidb_simulation_io_write_operational_domain_params_sample_writing_mode_OPERATIONAL_ONLY))

        ;

    detail::register_write_operational_domain(m);
    detail::register_write_critical_temperature_domain(m);
}

}  // namespace pyfiction
