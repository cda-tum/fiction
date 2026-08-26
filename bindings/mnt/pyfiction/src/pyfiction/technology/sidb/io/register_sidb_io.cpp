#include <nanobind/nanobind.h>

namespace pyfiction
{

void read_sqd_layout(nanobind::module_& m);
void write_operational_domain(nanobind::module_& m);
void write_sqd_layout(nanobind::module_& m);
void write_sqd_sim_result(nanobind::module_& m);

void register_sidb_io(nanobind::module_& m)
{
    write_sqd_layout(m);
    write_sqd_sim_result(m);
    write_operational_domain(m);

    read_sqd_layout(m);
}

}  // namespace pyfiction
