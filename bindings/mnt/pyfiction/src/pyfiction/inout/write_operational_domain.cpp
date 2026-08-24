//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"

#include <fiction/io/write_operational_domain.hpp>
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

namespace pyfiction
{

namespace detail
{
void write_operational_domain(nanobind::module_& m)
{
    namespace py = nanobind;

    // Function pointer for writing to a file
    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_operational_domain_pointer)(
        const fiction::sidb::simulation::logic::operational_domain&, const std::string_view&,
        const fiction::write_operational_domain_params&) = &fiction::write_operational_domain;

    // Define function using function pointer
    m.def("write_operational_domain", write_operational_domain_pointer, py::arg("opdom"), py::arg("filename"),
          py::arg("params"), DOC(fiction_write_operational_domain));

    m.def(
        "write_operational_domain_to_string",
        [](const fiction::sidb::simulation::logic::operational_domain& opdom,
           const fiction::write_operational_domain_params&             params = {}) -> std::string
        {
            std::ostringstream oss;
            fiction::write_operational_domain(opdom, oss, params);
            return oss.str();
        },
        py::arg("opdom"), py::arg("params") = fiction::write_operational_domain_params{});
}

void write_critical_temperature_domain(nanobind::module_& m)
{
    namespace py = nanobind;

    // Function pointer for writing to a file
    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_critical_temperature_domain_pointer)(
        const fiction::sidb::simulation::logic::critical_temperature_domain&, const std::string_view&,
        const fiction::write_operational_domain_params&) = &fiction::write_operational_domain;

    // Define function using function pointer
    m.def("write_critical_temperature_domain", write_critical_temperature_domain_pointer, py::arg("opdom"),
          py::arg("filename"), py::arg("params"), DOC(fiction_critical_temperature_domain));

    m.def(
        "write_critical_temperature_domain_to_string",
        [](const fiction::sidb::simulation::logic::critical_temperature_domain& opdom,
           const fiction::write_operational_domain_params&                      params = {}) -> std::string
        {
            std::ostringstream oss;
            fiction::write_operational_domain(opdom, oss, params);
            return oss.str();
        },
        py::arg("opdom"), py::arg("params") = fiction::write_operational_domain_params{});
}

}  // namespace detail

void write_operational_domain(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::write_operational_domain_params::sample_writing_mode>(
        m, "sample_writing_mode", DOC(fiction_write_operational_domain_params_sample_writing_mode))
        .value("ALL_SAMPLES", fiction::write_operational_domain_params::sample_writing_mode::ALL_SAMPLES,
               DOC(fiction_write_operational_domain_params_sample_writing_mode_ALL_SAMPLES))
        .value("OPERATIONAL_ONLY", fiction::write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY,
               DOC(fiction_write_operational_domain_params_sample_writing_mode_OPERATIONAL_ONLY))

        ;

    py::class_<fiction::write_operational_domain_params>(m, "write_operational_domain_params",
                                                         DOC(fiction_write_operational_domain_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("operational_tag", &fiction::write_operational_domain_params::operational_tag,
                DOC(fiction_write_operational_domain_params_operational_tag))
        .def_rw("non_operational_tag", &fiction::write_operational_domain_params::non_operational_tag,
                DOC(fiction_write_operational_domain_params_non_operational_tag))
        .def_rw("writing_mode", &fiction::write_operational_domain_params::writing_mode,
                DOC(fiction_write_operational_domain_params_writing_mode));

    detail::write_operational_domain(m);
    detail::write_critical_temperature_domain(m);
}

}  // namespace pyfiction
