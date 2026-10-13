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

#include <optional>
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
            return fiction::sidb::simulation::io::write_operational_domain(opdom, filename, params);
        },
        nb::arg("opdom"), nb::arg("filename"), nb::kw_only(),
        nb::arg("operational_tag")     = write_operational_domain_options{}.operational_tag,
        nb::arg("non_operational_tag") = write_operational_domain_options{}.non_operational_tag,
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode,
        R"doc( Writes a CSV representation of an operational domain to the specified
 output stream. The data are written as rows, each corresponding to
 one set of simulation parameters and their corresponding operational
 status.

 The output CSV format is e.g. as follows:
```
embed:rst
.. code-block:: RST

epsilon_r, lambda_tf, operational status
0.0, 0.0, 0
0.1, 0.0, 1
... subsequent rows for each set of simulation parameters
```




 The operational status is a binary value represented by specified
 tags in the keyword options indicating whether the simulation parameters are
 within the operational domain or not.

Args:
    opdom: The operational domain to be written. It represents a
           mapping between sets of simulation parameters (one to three
           sweep parameters, written as the X, Y, and Z columns) and a
           tuple containing detailed information about the SiDB layout
           associated with those simulation parameters.
    os: The output stream where the CSV representation of the
        operational domain is written to.

Raises:
    std::invalid_argument: if the number of dimensions in the
                           operational domain is 0 or greater than 3.
                           Three is the number of enumerators of
                           `sweep_parameter`, so a fourth dimension
                           could only repeat one of them.

Algorithm options are keyword-only; the signature lists their defaults.
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
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode);
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
            return fiction::sidb::simulation::io::write_operational_domain(opdom, filename, params);
        },
        nb::arg("opdom"), nb::arg("filename"), nb::kw_only(),
        nb::arg("operational_tag")     = write_operational_domain_options{}.operational_tag,
        nb::arg("non_operational_tag") = write_operational_domain_options{}.non_operational_tag,
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode,
        DOC(fiction_sidb_simulation_logic_critical_temperature_domain));

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
        nb::arg("writing_mode")        = write_operational_domain_options{}.writing_mode);
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
