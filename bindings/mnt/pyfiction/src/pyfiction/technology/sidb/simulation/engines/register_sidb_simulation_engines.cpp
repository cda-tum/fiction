#include <nanobind/nanobind.h>

namespace pyfiction
{

void quickexact(nanobind::module_& m);
void quicksim(nanobind::module_& m);
void exhaustive_ground_state_simulation(nanobind::module_& m);
void clustercomplete(nanobind::module_& m);

void register_sidb_simulation_engines(nanobind::module_& m)
{
    quickexact(m);
    quicksim(m);
    exhaustive_ground_state_simulation(m);
    clustercomplete(m);
}

}  // namespace pyfiction
