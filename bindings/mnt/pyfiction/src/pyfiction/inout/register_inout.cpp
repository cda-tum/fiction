#include <nanobind/nanobind.h>

namespace pyfiction
{

void read_fgl_layout(nanobind::module_& m);
void read_fqca_layout(nanobind::module_& m);
void read_sqd_layout(nanobind::module_& m);
void write_dot_layout(nanobind::module_& m);
void write_fgl_layout(nanobind::module_& m);
void write_fqca_layout(nanobind::module_& m);
void write_operational_domain(nanobind::module_& m);
void write_qca_layout(nanobind::module_& m);
void write_qcc_layout(nanobind::module_& m);
void write_qll_layout(nanobind::module_& m);
void write_sqd_layout(nanobind::module_& m);
void write_sqd_sim_result(nanobind::module_& m);
void write_svg_layout(nanobind::module_& m);

void register_inout(nanobind::module_& m)
{
    write_dot_layout(m);
    write_fgl_layout(m);
    write_qca_layout(m);
    write_svg_layout(m);
    write_sqd_layout(m);
    write_qcc_layout(m);
    write_qll_layout(m);
    write_fqca_layout(m);
    write_sqd_sim_result(m);
    write_operational_domain(m);

    read_fgl_layout(m);
    read_fqca_layout(m);
    read_sqd_layout(m);
}

}  // namespace pyfiction
