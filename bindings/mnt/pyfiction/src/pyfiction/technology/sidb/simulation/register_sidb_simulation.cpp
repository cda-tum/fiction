#include <nanobind/nanobind.h>

namespace pyfiction
{

void engine(nanobind::module_& m);
void result(nanobind::module_& m);
void is_ground_state(nanobind::module_& m);
void check_simulation_results_for_equivalence(nanobind::module_& m);

void register_sidb_simulation(nanobind::module_& m)
{
    engine(m);
    result(m);
    is_ground_state(m);
    check_simulation_results_for_equivalence(m);
}

}  // namespace pyfiction
