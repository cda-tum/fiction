#include <nanobind/nanobind.h>

namespace pyfiction
{

void layout_utils(nanobind::module_& m);

void register_layouts_utils(nanobind::module_& m)
{
    layout_utils(m);
}

}  // namespace pyfiction
