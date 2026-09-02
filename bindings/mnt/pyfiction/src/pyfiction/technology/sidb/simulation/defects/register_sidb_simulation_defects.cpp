#include <nanobind/nanobind.h>

namespace pyfiction
{

void displacement_robustness_domain(nanobind::module_& m);

void register_sidb_simulation_defects(nanobind::module_& m)
{
    displacement_robustness_domain(m);
}

}  // namespace pyfiction
