//
// Created by marcel on 02.06.22.
//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#include <nanobind/nanobind.h>

namespace pyfiction
{
void register_layouts(nanobind::module_& m);
void register_networks(nanobind::module_& m);
void register_sidb_support(nanobind::module_& m);
void register_technology(nanobind::module_& m);

void register_layouts_io(nanobind::module_& m);
void register_qca_io(nanobind::module_& m);
void register_inml_io(nanobind::module_& m);
void register_sidb_io(nanobind::module_& m);

void register_layouts_utils(nanobind::module_& m);
void register_physical_design_utils(nanobind::module_& m);
void register_networks_utils(nanobind::module_& m);
void register_utils(nanobind::module_& m);

void register_path_finding(nanobind::module_& m);
void register_synthesis(nanobind::module_& m);
void register_verification(nanobind::module_& m);
void register_sidb_simulation(nanobind::module_& m);
void register_sidb_generators(nanobind::module_& m);
void register_physical_design(nanobind::module_& m);
}  // namespace pyfiction

NB_MODULE(pyfiction, m)
{
    // docstring
    m.doc() = "Python bindings for fiction, a framework for Design Automation for Field-coupled Nanotechnologies";

    // The order is load-bearing: a type has to be registered before anything that names it in a
    // signature or a default argument. Types first, then the readers and writers, then the
    // utilities, then the algorithms that build on all of them.
    pyfiction::register_layouts(m);
    pyfiction::register_networks(m);
    pyfiction::register_sidb_support(m);
    pyfiction::register_technology(m);

    pyfiction::register_layouts_io(m);
    pyfiction::register_qca_io(m);
    pyfiction::register_inml_io(m);
    pyfiction::register_sidb_io(m);

    pyfiction::register_layouts_utils(m);
    pyfiction::register_physical_design_utils(m);
    pyfiction::register_networks_utils(m);
    pyfiction::register_utils(m);

    pyfiction::register_path_finding(m);
    pyfiction::register_synthesis(m);
    pyfiction::register_verification(m);
    pyfiction::register_sidb_simulation(m);
    pyfiction::register_sidb_generators(m);
    pyfiction::register_physical_design(m);
}

#pragma GCC diagnostic pop
