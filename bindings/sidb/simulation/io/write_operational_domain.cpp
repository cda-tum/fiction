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
 * @author OpenAI Codex
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
 * @brief Registers file and string writers for a domain type.
 * @tparam Domain Operational domain type.
 * @param m Python module.
 * @param file_name Exported file writer name.
 * @param string_name Exported string writer name.
 * @param file_doc File writer documentation.
 */
template <typename Domain>
void register_domain_writer(nb::module_& m, const char* file_name, const char* string_name, const char* file_doc)
{
    m.def(
        file_name,
        [](const Domain& opdom, const std::string_view& filename,
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
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode, file_doc);

    m.def(
        string_name,
        [](const Domain& opdom, const decltype(write_operational_domain_options{}.operational_tag) operational_tag,
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

Args:
    operational_tag: The tag used to represent the operational value of a parameter set.
    non_operational_tag: The tag used to represent the non-operational value of a parameter set.
    writing_mode: Write all samples or only operational samples.
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

    detail::register_domain_writer<fiction::sidb::simulation::logic::operational_domain>(
        m, "write_operational_domain", "write_operational_domain_to_string",
        R"doc(Write the operational domain to the supplied filename as CSV.
CSV columns hold the sweep-parameter values followed by the operational-status tag.
The keyword options select status tags and which samples to write.
Raises ValueError unless the domain has one to three sweep dimensions.

Args:
    operational_tag: The tag used to represent the operational value of a parameter set.
    non_operational_tag: The tag used to represent the non-operational value of a parameter set.
    writing_mode: Write all samples or only operational samples.
)doc");
    detail::register_domain_writer<fiction::sidb::simulation::logic::critical_temperature_domain>(
        m, "write_critical_temperature_domain", "write_critical_temperature_domain_to_string",
        R"doc(Write critical-temperature samples to a CSV file.
CSV columns hold sweep-parameter values, the operational-status tag, and critical temperature.
The keyword options select status tags and which samples to write.
Raises ValueError unless the domain has one to three sweep dimensions.

Args:
    operational_tag: The tag used to represent the operational value of a parameter set.
    non_operational_tag: The tag used to represent the non-operational value of a parameter set.
    writing_mode: Write all samples or only operational samples.
)doc");
}

}  // namespace pyfiction
