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
    double occupation_propability_pi(const std::map<double, uint64_t> &energy_deg, const double &temp,
                                     const uint64_t &state_number) {

        assert(!energy_deg.empty() && "map is empty");
        assert(!(energy_deg.size() < state_number) && "state_number is too large");

        if (temp == 0) {
            auto it = energy_deg.begin();
            for (uint64_t i = 0; i < state_number; i++) {
                it++;
            }

            if (it != energy_deg.begin()) {
                return 0;
            }
            return 1;

        }

        double part_func = std::accumulate(energy_deg.begin(), energy_deg.end(), 0.0,
                                           [&](double sum, const auto &it) {
                                               return sum + static_cast<double>(it.second) *
                                                            std::exp(-it.first * 12000 / temp);
                                           });


        auto it = energy_deg.begin();
        for (uint64_t i = 0; i < state_number; i++) {
            it++;
        }

        return static_cast<double>(it->second) * std::exp(-it->first * 12000 / temp) / part_func;

    }

} // namespace fiction

#endif  // FICTION_OCCUPATION_FUNCTION_HPP
