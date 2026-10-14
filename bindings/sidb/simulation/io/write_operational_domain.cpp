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

namespace detail
{
/**
 * @brief Registers file and string writers for an operational domain type.
 * @tparam Domain Operational domain type.
 * @param m Python module.
 * @param file_name Exported file writer name.
 * @param string_name Exported string writer name.
 * @param doc File writer documentation.
 */
template <typename Domain>
void register_domain_writer(nb::module_& m, const char* file_name, const char* string_name, const char* doc)
{
    using fiction::sidb::simulation::io::write_operational_domain_params;

    m.def(file_name,
          nb::overload_cast<const Domain&, const std::string_view&, const write_operational_domain_params&>(
              &fiction::sidb::simulation::io::write_operational_domain<Domain>),
          nb::arg("opdom"), nb::arg("filename"), nb::arg("params"), doc);

    m.def(
        string_name,
        [](const Domain& opdom, const write_operational_domain_params& params)
        {
            std::ostringstream oss;
            fiction::sidb::simulation::io::write_operational_domain(opdom, oss, params);
            return oss.str();
        },
        nb::arg("opdom"), nb::arg("params") = write_operational_domain_params{});
}

}  // namespace detail

/**
 * @brief Binds write operational domain helpers.
 * @param m Python module.
 */
void register_write_operational_domain(nb::module_& m)
{
    nb::enum_<fiction::sidb::simulation::io::write_operational_domain_params::sample_writing_mode>(
        m, "sample_writing_mode", DOC(fiction_sidb_simulation_io_write_operational_domain_params_sample_writing_mode))
        .value("ALL_SAMPLES",
               fiction::sidb::simulation::io::write_operational_domain_params::sample_writing_mode::ALL_SAMPLES,
               DOC(fiction_sidb_simulation_io_write_operational_domain_params_sample_writing_mode_ALL_SAMPLES))
        .value("OPERATIONAL_ONLY",
               fiction::sidb::simulation::io::write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY,
               DOC(fiction_sidb_simulation_io_write_operational_domain_params_sample_writing_mode_OPERATIONAL_ONLY))

        ;

    nb::class_<fiction::sidb::simulation::io::write_operational_domain_params>(
        m, "write_operational_domain_params", DOC(fiction_sidb_simulation_io_write_operational_domain_params))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("operational_tag", &fiction::sidb::simulation::io::write_operational_domain_params::operational_tag,
                DOC(fiction_sidb_simulation_io_write_operational_domain_params_operational_tag))
        .def_rw("non_operational_tag",
                &fiction::sidb::simulation::io::write_operational_domain_params::non_operational_tag,
                DOC(fiction_sidb_simulation_io_write_operational_domain_params_non_operational_tag))
        .def_rw("writing_mode", &fiction::sidb::simulation::io::write_operational_domain_params::writing_mode,
                DOC(fiction_sidb_simulation_io_write_operational_domain_params_writing_mode));

    detail::register_domain_writer<fiction::sidb::simulation::logic::operational_domain>(
        m, "write_operational_domain", "write_operational_domain_to_string",
        DOC(fiction_sidb_simulation_io_write_operational_domain));
    detail::register_domain_writer<fiction::sidb::simulation::logic::critical_temperature_domain>(
        m, "write_critical_temperature_domain", "write_critical_temperature_domain_to_string",
        DOC(fiction_sidb_simulation_logic_critical_temperature_domain));
}

}  // namespace pyfiction
