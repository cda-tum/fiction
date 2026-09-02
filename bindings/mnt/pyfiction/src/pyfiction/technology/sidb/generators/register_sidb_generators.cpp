#include <nanobind/nanobind.h>

namespace pyfiction
{

void design_gates(nanobind::module_& m);
void random_layout_generator(nanobind::module_& m);

void register_sidb_generators(nanobind::module_& m)
{
    random_layout_generator(m);
    design_gates(m);
}

}  // namespace pyfiction
