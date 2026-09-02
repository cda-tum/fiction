#include <nanobind/nanobind.h>

namespace pyfiction
{

void read_fgl_layout(nanobind::module_& m);
void write_dot_layout(nanobind::module_& m);
void write_fgl_layout(nanobind::module_& m);

void register_layouts_io(nanobind::module_& m)
{
    write_dot_layout(m);
    write_fgl_layout(m);

    read_fgl_layout(m);
}

}  // namespace pyfiction
