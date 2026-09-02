#include <nanobind/nanobind.h>

namespace pyfiction
{

void write_sqd_layout(nanobind::module_& m);
void read_sqd_layout(nanobind::module_& m);
void write_sidb_layout_svg(nanobind::module_& m);

void register_sidb_io(nanobind::module_& m)
{
    write_sqd_layout(m);
    read_sqd_layout(m);
    write_sidb_layout_svg(m);
}

}  // namespace pyfiction
