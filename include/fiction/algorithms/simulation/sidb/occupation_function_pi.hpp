//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_OCCUPATION_FUNCTION_HPP
#define FICTION_OCCUPATION_FUNCTION_HPP

#include <map>
#include <cmath>

double occupation_propability_pi(std::map<double, uint64_t>& energy_deg, const double &temp, uint64_t &state_number)
{
    static_assert((energy_deg < state_number), "Lyt is not a cell-level layout");

    double part_func = 0;
    for (const auto& it : energy_deg)
    {
        part_func += static_cast<double>(it.second) * std::exp(-it.first * 12000 / temp);
    }

    auto it = std::advance(energy_deg.begin(), state_number);

    return static_cast<double>(it->second) * std::exp(-it->first * 12000 / temp) / part_func;

};


#endif  // FICTION_OCCUPATION_FUNCTION_HPP
