//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_OCCUPATION_FUNCTION_HPP
#define FICTION_OCCUPATION_FUNCTION_HPP

#include <map>
#include <cmath>
#include <iterator>
#include <iostream>
#include <vector>
#include <numeric>

namespace fiction {
    double occupation_propability_erroneous(const std::map<double, std::pair<uint64_t, bool>> &energy_deg, const double &temp) {

        assert(!energy_deg.empty() && "map is empty");
        assert((temp > 0) && "temperature should be slightly above 0 K");

        double min_energy = energy_deg.begin()->first;

        double part_func = std::accumulate(energy_deg.begin(), energy_deg.end(), 0.0,
                                           [&](double sum, const auto &it) {
                                               return sum + static_cast<double>(it.second.first) *
                                                            std::exp(-(it.first - min_energy) * 12000 / temp);
                                           });

        if (energy_deg.size() == 1) {
            if (!energy_deg.begin()->second.second) {
                return 1.0;
            }
            return 0.0;
        }

        double p = 0;
        for (const auto &[energies, count]: energy_deg) {
            if (!count.second) {
                p += static_cast<double>(count.first) *
                     std::exp(-(energies - min_energy) * 12000 / temp) / part_func;
            }
        }
        return p / part_func;


    }

} // namespace fiction

#endif  // FICTION_OCCUPATION_FUNCTION_HPP
