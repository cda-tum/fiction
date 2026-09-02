#include <nanobind/nanobind.h>

namespace pyfiction
{

void write_qll_layout(nanobind::module_& m);

void register_fcn_io(nanobind::module_& m)
{
    write_qll_layout(m);
}

}  // namespace pyfiction
