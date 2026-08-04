#include <nanobind/nanobind.h>

namespace pyfiction
{

void calculate_energy_and_state_type(nanobind::module_& m);
void can_positive_charges_occur(nanobind::module_& m);
void check_simulation_results_for_equivalence(nanobind::module_& m);
void clustercomplete(nanobind::module_& m);
void critical_temperature(nanobind::module_& m);
void detect_bdl_pairs(nanobind::module_& m);
void detect_bdl_wires(nanobind::module_& m);
void determine_displacement_robustness_domain(nanobind::module_& m);
void energy_distribution(nanobind::module_& m);
void exhaustive_ground_state_simulation(nanobind::module_& m);
void is_ground_state(nanobind::module_& m);
void is_operational(nanobind::module_& m);
void minimum_energy(nanobind::module_& m);
void occupation_probability_of_excited_states(nanobind::module_& m);
void operational_domain(nanobind::module_& m);
void compute_operational_ratio(nanobind::module_& m);
void physical_population_stability(nanobind::module_& m);
void physically_valid_parameters(nanobind::module_& m);
void potential_to_distance_conversion(nanobind::module_& m);
void quickexact(nanobind::module_& m);
void quicksim(nanobind::module_& m);
void random_sidb_layout_generator(nanobind::module_& m);
void time_to_solution(nanobind::module_& m);

void register_sidb_simulation(nanobind::module_& m)
{
    calculate_energy_and_state_type(m);
    can_positive_charges_occur(m);
    check_simulation_results_for_equivalence(m);
    clustercomplete(m);
    critical_temperature(m);
    detect_bdl_pairs(m);
    detect_bdl_wires(m);
    determine_displacement_robustness_domain(m);
    energy_distribution(m);
    exhaustive_ground_state_simulation(m);
    is_ground_state(m);
    is_operational(m);
    minimum_energy(m);
    occupation_probability_of_excited_states(m);
    operational_domain(m);
    compute_operational_ratio(m);
    physical_population_stability(m);
    physically_valid_parameters(m);
    potential_to_distance_conversion(m);
    quickexact(m);
    quicksim(m);
    random_sidb_layout_generator(m);
    time_to_solution(m);
}

}  // namespace pyfiction
