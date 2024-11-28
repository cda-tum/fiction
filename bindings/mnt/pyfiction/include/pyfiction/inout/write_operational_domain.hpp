//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_WRITE_OPERATIONAL_DOMAIN_HPP
#define PYFICTION_WRITE_OPERATIONAL_DOMAIN_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_operational_domain.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

inline void write_operational_domain(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::write_operational_domain_params::sample_writing_mode>(
        m, "sample_writing_mode", DOC(fiction_write_operational_domain_params_sample_writing_mode))
        .value("ALL_SAMPLES", fiction::write_operational_domain_params::sample_writing_mode::ALL_SAMPLES,
               DOC(fiction_write_operational_domain_params_sample_writing_mode_ALL_SAMPLES))
        .value("OPERATIONAL_ONLY", fiction::write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY,
               DOC(fiction_write_operational_domain_params_sample_writing_mode_OPERATIONAL_ONLY));

    py::class_<fiction::write_operational_domain_params>(m, "write_operational_domain_params",
                                                         DOC(fiction_write_operational_domain_params))
        .def(py::init<>())
        .def_readwrite("operational_tag", &fiction::write_operational_domain_params::operational_tag,
                       DOC(fiction_write_operational_domain_params_operational_tag))
        .def_readwrite("non_operational_tag", &fiction::write_operational_domain_params::non_operational_tag,
                       DOC(fiction_write_operational_domain_params_non_operational_tag))
        .def_readwrite("writing_mode", &fiction::write_operational_domain_params::writing_mode,
                       DOC(fiction_write_operational_domain_params_writing_mode));

    void (*write_operational_domain_pointer)(
        const fiction::operational_domain<fiction::parameter_point, fiction::operational_status>&,
        const std::string_view&, const fiction::write_operational_domain_params&) = &fiction::write_operational_domain;

    m.def("write_operational_domain", write_operational_domain_pointer, py::arg("opdom"), py::arg("filename"),
          py::arg("params") = fiction::write_operational_domain_params{}, DOC(fiction_write_operational_domain));
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_OPERATIONAL_DOMAIN_HPP
