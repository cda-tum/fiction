#include <nanobind/nanobind.h>

namespace pyfiction
{

void layout_drawers(nanobind::module_& m);
void write_fgl_layout(nanobind::module_& m);
void read_fgl_layout(nanobind::module_& m);

void register_layouts_io(nanobind::module_& m)
{
    layout_drawers(m);
    write_fgl_layout(m);
    read_fgl_layout(m);
}

}  // namespace pyfiction
