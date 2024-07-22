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
    using namespace py::literals;

    py::class_<fiction::write_operational_domain_params>(m, "write_operational_domain_params",
                                                         DOC(fiction_write_operational_domain_params))
        .def(py::init<>())
        .def_readwrite("operational_tag", &fiction::write_operational_domain_params::operational_tag,
                       DOC(fiction_write_operational_domain_params_operational_tag))
        .def_readwrite("non_operational_tag", &fiction::write_operational_domain_params::non_operational_tag,
                       DOC(fiction_write_operational_domain_params_non_operational_tag))

        ;

    void (*write_operational_domain_pointer)(
        const fiction::operational_domain<fiction::parameter_point, fiction::operational_status>&,
        const std::string_view&, const fiction::write_operational_domain_params&) = &fiction::write_operational_domain;

    m.def("write_operational_domain", write_operational_domain_pointer, "opdom"_a, "filename"_a,
          "params"_a = fiction::write_operational_domain_params{}, DOC(fiction_write_operational_domain));
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_OPERATIONAL_DOMAIN_HPP
