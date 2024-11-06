//
// Created by Jan Drewniok on 06.11.2024
//

#ifndef PYFICTION_WRITE_DEFECT_OPERATIONAL_DOMAIN_HPP
#define PYFICTION_WRITE_DEFECT_OPERATIONAL_DOMAIN_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_defect_operational_domain.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
inline void write_defect_operational_domain_impl(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace py::literals;

    void (*write_defect_operational_domain_pointer)(
        const fiction::defect_operational_domain<Lyt>&, const std::string_view&,
        const fiction::write_defect_operational_domain_params&) = &fiction::write_defect_operational_domain;

    m.def("write_defect_operational_domain", write_defect_operational_domain_pointer, "opdom"_a, "filename"_a,
          "params"_a = fiction::write_defect_operational_domain_params{});
}

}

inline void write_defect_operational_domain(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace py::literals;

    py::class_<fiction::write_defect_operational_domain_params>(m, "write_defect_operational_domain_params")
        .def(py::init<>())
        .def_readwrite("operational_tag", &fiction::write_defect_operational_domain_params::operational_tag)
        .def_readwrite("non_operational_tag", &fiction::write_defect_operational_domain_params::non_operational_tag);

    detail::write_defect_operational_domain_impl<py_sidb_100_lattice_cube>(m);
    detail::write_defect_operational_domain_impl<py_sidb_111_lattice_cube>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_DEFECT_OPERATIONAL_DOMAIN_HPP
