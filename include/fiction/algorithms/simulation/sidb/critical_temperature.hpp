//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_CRITICAL_TEMPERATURE_HPP
#define FICTION_CRITICAL_TEMPERATURE_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/algorithms/simulation/sidb/occupation_function_erroneous.hpp"
#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/sort_function.hpp"
#include "fiction/utils/gate_logic_map.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/hash.hpp"
#include <cmath>

namespace fiction {

    template<typename Lyt>
    struct critical_temperature_stats {
        double critical_temperature{};
        std::map<double, std::pair<uint64_t, bool>> valid_lyts{};
        uint64_t num_valid_lyt{};
        double emingrounderror = std::numeric_limits<double>::max();

        void report(std::ostream &out = std::cout) const {
            out << fmt::format("critical temperature  = {:.2f} K\n", critical_temperature);
            if (!valid_lyts.empty()) {
                for (const auto &[energy, count]: valid_lyts) {
                    out << fmt::format("energy: {} | occurance: {} | transparent: {} | E_min_g,err: {}\n", energy,
                                       count.first,
                                       count.second, emingrounderror);
                }
            } else {
                std::cout << "no state found | if two state simulation is used, continue with three state" << std::endl;
            }

            out << fmt::format("{} phyiscally valid charge states were found \n", num_valid_lyt);
            std::cout << "_____________________________________________________ \n";
        }
    };

    /**
    * The *critical temperature* function computes the critical temperature of a given layout. It defines the temperature
    * that results in a population of erroneous excited states with a probability greater than 1 − η,
    * where η is the confidence level for the presence of a working gate, is called the Critical Temperature (CT) of the gate.
    *
    * @tparam Lyt Cell-level layout type.
    * @param lyt The layout to simulate.
    * @param ps Physical parameters. They are material-specific and may vary from experiment to experiment.
    * @param pst Statistics. They store the simulation results.
    * @param confidence_level Confidence level for the presence of a working gate.
    * @param max_temperature The maximal critical temperature is set at 400 K.
    * @return The criticaltemperature is returned. -10 as return says that either no charge distribution satisfies logic, or at least not the ground state as it should be.
    * Changing the physical parameter µ_ might help.
    */
    template<typename Lyt>
    void critical_temperature(const Lyt &lyt, const std::string &gate, const std::string &input_bits,
                              const sidb_simulation_parameters &params = sidb_simulation_parameters{},
                              critical_temperature_stats<Lyt> *pst = nullptr,
                              const double confidence_level = 0.99, const uint64_t max_temperature = 400) {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
        static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
        critical_temperature_stats<Lyt> cs{};


        if (!lyt.is_empty()) {


            // all physically valid charge configurations are determined for the given layout (exhaustive ground state simulation is used to provide 100 % accuracy for CT).
            exgs_stats<Lyt> stats_exhaustive{};
            exhaustive_ground_state_simulation(lyt, params, &stats_exhaustive);

            cs.num_valid_lyt = stats_exhaustive.valid_lyts.size();

            if (lyt.num_cells() == 1) {
                cs.critical_temperature = max_temperature;
            }

            if (lyt.num_cells() > 1) {
                // vector with temperature values from 0.01 to 400 K in 0.01 K steps is generated.
                std::vector<double> temp_values{};
                for (uint64_t i = 1; i <= max_temperature * 100; i++) {
                    temp_values.push_back(static_cast<double>(i) / 100.0);
                }

                // all cells of the given layout are collected.
                std::vector<typename Lyt::cell> all_cells{};
                all_cells.reserve(lyt.num_cells());
                lyt.foreach_cell([&all_cells](const auto &c) { all_cells.push_back(c); });

                // the cells are sorted by local_sort_sidb_cells. Goal is to have the cells sorted from left to right, and from top to bottom.
                // Meaning the output SiDB can be found in the second last place.
                std::sort(all_cells.begin(), all_cells.end(), local_sort_sidb_cells<Lyt>);


                // the indices of the output cells are collected as a vector. Depending on the specific gate, there is one or two outputs.
                std::vector<int64_t> output_bits_index{};

                if (gate_logic::GATES_IN_OUT_NUMBER.at(gate).at("out") == "2") {
                    output_bits_index = {-4, -3}; // for example: double wire, cx, etc.
                } else {
                    output_bits_index = {-2};
                }

                // we collect the cell coordinates of the output cell(s).
                std::vector<typename Lyt::cell> output_cells;
                output_cells.reserve(output_bits_index.size());
                std::transform(output_bits_index.begin(), output_bits_index.end(), std::back_inserter(output_cells),
                               [&all_cells](const auto &index) { return *(all_cells.end() + index); });


                // we determine the energy distribution of the physically valid charge configurations for the given layout.
                std::map<double, uint64_t> distribution = energy_distribution(stats_exhaustive.valid_lyts);

                // we add a label that indicates whether the state still calculates the correct result.
                std::map<double, std::pair<uint64_t, bool>> degeneracy_transparent_erroneous{};

                for (const auto &[energy, occurance]: distribution) {
                    bool transparent = false;
                    for (const auto &layout: stats_exhaustive.valid_lyts) {
                        if ((std::round(layout.get_system_energy() * 1'000'000)) / 1'000'000 ==
                            (std::round(energy * 1'000'000)) / 1'000'000) {
                            std::string charge; // we collect the output as string. For example: "10", "1", etc. (depending on the number of outputs).
                            for (const auto &cell: output_cells) {
                                charge = charge + std::to_string(-charge_state_to_sign(layout.get_charge_state(cell)));
                            }

                            // if the output is correct, the state still computes the correct output. Hence, state is called transparent.
                            if (charge == gate_logic::GATE_TRUTH_TABLE.at(gate).at(input_bits)) {
                                transparent = true;
                            }
                        }
                    }
                    degeneracy_transparent_erroneous[energy] = std::make_pair(distribution[energy], transparent);
                }

                cs.valid_lyts = degeneracy_transparent_erroneous;

                // we determine the energy difference between the ground state and the first erroneous state.
                for (const auto &[energy, count]: degeneracy_transparent_erroneous) {
                    if (!count.second) {
                        cs.emingrounderror = (energy - degeneracy_transparent_erroneous.begin()->first) * 1000;
                        break;
                    }
                }

                // this function determines the critical temperature (CT) for a confidence level of 99 %.
                for (const auto &temp: temp_values) {
                    if (occupation_propability_erroneous(degeneracy_transparent_erroneous, temp) >
                        (1 - confidence_level) &&
                        occupation_propability_erroneous(degeneracy_transparent_erroneous, temp) != -1.0) {
                        cs.critical_temperature = temp;
                        break;
                    }
                    if (occupation_propability_erroneous(degeneracy_transparent_erroneous, temp) == -1.0) {
                        cs.critical_temperature = -10;
                        break;
                    }
                    if (std::abs(temp - static_cast<double>(max_temperature)) < 0.001) {
                        cs.critical_temperature = static_cast<double>(max_temperature);
                    }
                }
            }
                if (pst) {
                    *pst = cs;
                }

        }
    }

}  // namespace fiction

#endif  //FICTION_CRITICAL_TEMPERATURE_HPP
