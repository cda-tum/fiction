#include <nanobind/nanobind.h>

namespace pyfiction
{

void write_qcc_layout(nanobind::module_& m);

void register_inml_io(nanobind::module_& m)
{
    write_qcc_layout(m);
}

}  // namespace pyfiction
