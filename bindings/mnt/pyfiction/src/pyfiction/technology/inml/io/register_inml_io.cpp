#include <nanobind/nanobind.h>

namespace pyfiction
{

void write_qcc_layout(nanobind::module_& m);
void write_qll_layout(nanobind::module_& m);

void register_inml_io(nanobind::module_& m)
{
    write_qcc_layout(m);
    write_qll_layout(m);
}

}  // namespace pyfiction
