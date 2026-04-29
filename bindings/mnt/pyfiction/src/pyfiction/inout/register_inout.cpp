#include <pybind11/pybind11.h>

namespace pyfiction
{

void read_fgl_layout(pybind11::module& m);
void read_fqca_layout(pybind11::module& m);
void read_sqd_layout(pybind11::module& m);
void write_dot_layout(pybind11::module& m);
void write_fgl_layout(pybind11::module& m);
void write_fqca_layout(pybind11::module& m);
void write_operational_domain(pybind11::module& m);
void write_qca_layout(pybind11::module& m);
void write_qcc_layout(pybind11::module& m);
void write_qll_layout(pybind11::module& m);
void write_sqd_layout(pybind11::module& m);
void write_sqd_sim_result(pybind11::module& m);
void write_svg_layout(pybind11::module& m);

void register_inout(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
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
