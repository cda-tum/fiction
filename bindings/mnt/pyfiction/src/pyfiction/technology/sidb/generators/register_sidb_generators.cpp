#include <nanobind/nanobind.h>

namespace pyfiction
{

void design_sidb_gates(nanobind::module_& m);
void random_sidb_layout_generator(nanobind::module_& m);

void register_sidb_generators(nanobind::module_& m)
{
    random_sidb_layout_generator(m);
    design_sidb_gates(m);
}

}  // namespace pyfiction
