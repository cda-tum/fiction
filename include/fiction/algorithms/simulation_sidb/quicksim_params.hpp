//
// Created by Jan Drewniok on 27.01.23.
//

#ifndef FICTION_QUICKSIM_PARAMS_HPP
#define FICTION_QUICKSIM_PARAMS_HPP

#include "fiction/algorithms/simulation_sidb/sidb_simulation_parameters.hpp"

namespace fiction
{

struct quicksim_params
{
    const sidb_simulation_parameters phys_params{};
    const uint64_t interation_steps;
    const double alpha;

    explicit quicksim_params(const sidb_simulation_parameters &params = sidb_simulation_parameters{}, const uint64_t &it_steps = 80, const double al = 0.7) :
            phys_params{params},
            interation_steps{it_steps},
            alpha{al}
    {}
};

} // namespace fiction

#endif  // FICTION_QUICKSIM_PARAMS_HPP
