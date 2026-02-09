#include <pybind11/pybind11.h>

namespace pyfiction
{

void calculate_energy_and_state_type(pybind11::module& m);
void can_positive_charges_occur(pybind11::module& m);
void check_simulation_results_for_equivalence(pybind11::module& m);
void clustercomplete(pybind11::module& m);
void critical_temperature(pybind11::module& m);
void detect_bdl_pairs(pybind11::module& m);
void detect_bdl_wires(pybind11::module& m);
void determine_displacement_robustness_domain(pybind11::module& m);
void energy_distribution(pybind11::module& m);
void exhaustive_ground_state_simulation(pybind11::module& m);
void is_ground_state(pybind11::module& m);
void is_operational(pybind11::module& m);
void minimum_energy(pybind11::module& m);
void occupation_probability_of_excited_states(pybind11::module& m);
void operational_domain(pybind11::module& m);
void compute_operational_ratio(pybind11::module& m);
void physical_population_stability(pybind11::module& m);
void physically_valid_parameters(pybind11::module& m);
void potential_to_distance_conversion(pybind11::module& m);
void quickexact(pybind11::module& m);
void quicksim(pybind11::module& m);
void sidb_simulation_engine(pybind11::module& m);
void sidb_simulation_result(pybind11::module& m);

void register_sidb_simulation(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    sidb_simulation_engine(m);
    sidb_simulation_result(m);
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
}

}  // namespace pyfiction
