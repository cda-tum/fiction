//
// Created by Jan Drewniok on 22.11.22.
//

#ifndef FICTION_SIQAD_TO_CARTESIAN_HPP
#define FICTION_SIQAD_TO_CARTESIAN_HPP

#include <fiction/layouts/coordinates.hpp>
#include <cstdint>
#include <ostream>

using namespace fiction;

namespace Params
{
// lattice dimensions
static constexpr float a = 3.84f * 1E-10f;
static constexpr float b = 2.25f * 1E-10f;
static constexpr float c = 7.68f * 1E-10f;
};

using coordinate = offset::ucoord_t;

auto converter(coordinate &c_siqad)
{
        auto c_eucl = coordinate{};


};


};
#endif  // FICTION_SIQAD_TO_CARTESIAN_HPP
