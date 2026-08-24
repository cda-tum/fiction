#include <nanobind/nanobind.h>

namespace pyfiction
{

void calculate_energy_and_state_type(nanobind::module_& m);
void sidb::simulation::generic::can_positive_charges_occur(nanobind::module_& m);
void sidb::simulation::utils::check_simulation_results_for_equivalence(nanobind::module_& m);
void sidb::simulation::engines::clustercomplete(nanobind::module_& m);
void critical_temperature(nanobind::module_& m);
void sidb::simulation::logic::detect_bdl_pairs(nanobind::module_& m);
void sidb::simulation::logic::detect_bdl_wires(nanobind::module_& m);
void sidb::simulation::defects::determine_displacement_robustness_domain(nanobind::module_& m);
void sidb::simulation::analysis::energy_distribution(nanobind::module_& m);
void sidb::simulation::engines::exhaustive_ground_state_simulation(nanobind::module_& m);
void sidb::simulation::generic::is_ground_state(nanobind::module_& m);
void sidb::simulation::logic::is_operational(nanobind::module_& m);
void sidb::simulation::generic::minimum_energy(nanobind::module_& m);
void occupation_probability_of_excited_states(nanobind::module_& m);
void sidb::simulation::logic::operational_domain(nanobind::module_& m);
void compute_operational_ratio(nanobind::module_& m);
void sidb::simulation::analysis::physical_population_stability(nanobind::module_& m);
void sidb::simulation::generic::physically_valid_parameters(nanobind::module_& m);
void sidb::simulation::generic::potential_to_distance_conversion(nanobind::module_& m);
void sidb::simulation::engines::quickexact(nanobind::module_& m);
void sidb::simulation::engines::quicksim(nanobind::module_& m);
void random_sidb_layout_generator(nanobind::module_& m);
void sidb::simulation::utils::time_to_solution(nanobind::module_& m);

void register_sidb_simulation(nanobind::module_& m)
{
    calculate_energy_and_state_type(m);
    sidb::simulation::generic::can_positive_charges_occur(m);
    sidb::simulation::utils::check_simulation_results_for_equivalence(m);
    sidb::simulation::engines::clustercomplete(m);
    critical_temperature(m);
    sidb::simulation::logic::detect_bdl_pairs(m);
    sidb::simulation::logic::detect_bdl_wires(m);
    sidb::simulation::defects::determine_displacement_robustness_domain(m);
    sidb::simulation::analysis::energy_distribution(m);
    sidb::simulation::engines::exhaustive_ground_state_simulation(m);
    sidb::simulation::generic::is_ground_state(m);
    sidb::simulation::logic::is_operational(m);
    sidb::simulation::generic::minimum_energy(m);
    occupation_probability_of_excited_states(m);
    sidb::simulation::logic::operational_domain(m);
    compute_operational_ratio(m);
    sidb::simulation::analysis::physical_population_stability(m);
    sidb::simulation::generic::physically_valid_parameters(m);
    sidb::simulation::generic::potential_to_distance_conversion(m);
    sidb::simulation::engines::quickexact(m);
    sidb::simulation::engines::quicksim(m);
    random_sidb_layout_generator(m);
    sidb::simulation::utils::time_to_solution(m);
}

}  // namespace pyfiction
