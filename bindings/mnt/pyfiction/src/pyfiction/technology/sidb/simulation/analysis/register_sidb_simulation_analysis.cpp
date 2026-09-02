#include <nanobind/nanobind.h>

namespace pyfiction
{

void calculate_energy_and_state_type(nanobind::module_& m);
void can_positive_charges_occur(nanobind::module_& m);
void critical_temperature(nanobind::module_& m);
void energy_distribution(nanobind::module_& m);
void minimum_energy(nanobind::module_& m);
void occupation_probability_of_excited_states(nanobind::module_& m);
void physical_population_stability(nanobind::module_& m);
void physically_valid_parameters(nanobind::module_& m);
void time_to_solution(nanobind::module_& m);

void register_sidb_simulation_analysis(nanobind::module_& m)
{
    calculate_energy_and_state_type(m);
    can_positive_charges_occur(m);
    critical_temperature(m);
    energy_distribution(m);
    minimum_energy(m);
    occupation_probability_of_excited_states(m);
    physical_population_stability(m);
    physically_valid_parameters(m);
    time_to_solution(m);
}

}  // namespace pyfiction
