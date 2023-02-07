//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_CRITICAL_TEMPERATURE_HPP
#define FICTION_CRITICAL_TEMPERATURE_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/algorithms/simulation/sidb/occupation_function_pi.hpp"
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

        void report(std::ostream &out = std::cout) const {
            out << fmt::format("critical temperature  = {:.2f} K\n", critical_temperature);
            if (!valid_lyts.empty()) {
                for (const auto &[energy, count]: valid_lyts) {
                    out << fmt::format("energy: {} | occurance: {} | transparent: {} \n", energy, count.first,
                                       count.second);
                }
            } else {
                std::cout << "no state found | if two state simulation is used, continue with three state" << std::endl;
            }

            out << fmt::format("{} phyiscally valid charge states were found \n", valid_lyts.size());
            std::cout << "_____________________________________________________ \n";
        }
    };

    template<typename Lyt>
    void critical_temperature(const Lyt &lyt, const std::string &gate, const std::string &input_bits,
                              const sidb_simulation_parameters &params = sidb_simulation_parameters{},
                              critical_temperature_stats<Lyt> *pst = nullptr,
                              const double convlevel = 0.997, const uint64_t temp_limit = 400) {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
        static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

        critical_temperature_stats<Lyt> cs{};
        // all physically valid charge cofigurations are determined for the given layout.
        exgs_stats<Lyt> stats_exhaustive{};
        exhaustive_ground_state_simulation(lyt, params, &stats_exhaustive);

        // vector with temperature values from 0 to 400 K in 0.2 K steps is generated.
        std::vector<double> temp_values{};
        for (uint64_t i = 0; i <= temp_limit * 5; i++) {
            temp_values.push_back(static_cast<double>(i) / 5.0);
        }

        // all cells of the given layout are collected.
        std::vector<typename Lyt::cell> all_cells{};
        all_cells.reserve(lyt.num_cells());
        lyt.foreach_cell([&all_cells](const auto &c) { all_cells.push_back(c); });

        std::sort(all_cells.begin(), all_cells.end(), local_sort_sidb_cells<Lyt>);


        std::vector<int64_t> output_bits_index{};
        // the indices of the output cells are collected as a vector. Depending on the specific gate, there is one or two outputs.

        if (gate_log::GATES_IN_OUT_NUMBER.at(gate).at("out") == "2") {
            output_bits_index = {-4, -3};
        } else {
            output_bits_index = {-2};
        }

        // we collect the cell coordinates of the output cell(s).
        std::vector<typename Lyt::cell> output_cells_simulated{};
        for (const auto &index: output_bits_index) {
            output_cells_simulated.emplace_back(*(all_cells.end() + index));
        }

        // we determine the energy distribution of the physically valid charge configurations for the given layout.
        std::map<double, uint64_t> distribution = energy_distribution(stats_exhaustive.valid_lyts);

        std::map<double, std::pair<uint64_t, bool>> degeneracy_transparent_errenous{};

        // we add to the energy distribution for eacxh energy a label that marks if the energy state still computes the correct result.
        for (const auto &[energy, pair]: distribution) {
            bool transparent = false;
            for (const auto &layout: stats_exhaustive.valid_lyts) {
                if ((std::round(layout.get_system_energy() * 1'000'000)) / 1'000'000 ==
                    (std::round(energy * 1'000'000)) / 1'000'000) {
                    std::string charge;
                    for (const auto &cell: output_cells_simulated) {
                        charge = charge + std::to_string(-charge_state_to_sign(layout.get_charge_state(cell)));
                    }

                    if (charge == gate_log::GATE_LOGIC.at(gate).at(input_bits)) {
                        transparent = true;
                    }
                }
            }
            degeneracy_transparent_errenous[energy] = std::make_pair(distribution[energy], transparent);
        }

        cs.valid_lyts = degeneracy_transparent_errenous;

        // this function determines the critical temperature (CT) for a confidence level of 99 %.
        for (const auto &temp: temp_values) {
            if (occupation_propability_pi(degeneracy_transparent_errenous, temp, 1) < convlevel) {
                cs.critical_temperature = temp;
                break;
            }
            if (std::abs(temp - static_cast<double>(temp_limit)) < 0.001) {
                cs.critical_temperature = static_cast<double>(temp_limit);
                break;
            }
        }

        if (pst) {
            *pst = cs;
        }
    }

}  // namespace fiction

#endif  //FICTION_CRITICAL_TEMPERATURE_HPP
