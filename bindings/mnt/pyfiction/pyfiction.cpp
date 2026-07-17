//
// Created by marcel on 02.06.22.
//

#define PYBIND11_DETAILED_ERROR_MESSAGES

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include <pybind11/pybind11.h>

namespace pyfiction
{
void register_inout(pybind11::module& m);
void register_algorithms(pybind11::module& m);
void register_layouts(pybind11::module& m);
void register_networks(pybind11::module& m);
void register_sidb_support(pybind11::module& m);
void register_technology(pybind11::module& m);
void register_utils(pybind11::module& m);
}  // namespace pyfiction

PYBIND11_MODULE(pyfiction, m, pybind11::mod_gil_not_used())
{
    // docstring
    m.doc() = "Python bindings for fiction, a framework for Design Automation for Field-coupled Nanotechnologies";

    pyfiction::register_layouts(m);
    pyfiction::register_networks(m);
    pyfiction::register_sidb_support(m);
    pyfiction::register_technology(m);
    pyfiction::register_inout(m);
    pyfiction::register_utils(m);
    pyfiction::register_algorithms(m);
}

#pragma GCC diagnostic pop
