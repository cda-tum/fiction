//
// Created by marcel on 02.06.22.
//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include <nanobind/nanobind.h>

namespace pyfiction
{
void register_inout(nanobind::module_& m);
void register_algorithms(nanobind::module_& m);
void register_layouts(nanobind::module_& m);
void register_networks(nanobind::module_& m);
void register_sidb_support(nanobind::module_& m);
void register_technology(nanobind::module_& m);
void register_utils(nanobind::module_& m);
}  // namespace pyfiction

NB_MODULE(pyfiction, m)
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
