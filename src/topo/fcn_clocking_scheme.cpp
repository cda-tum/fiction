//
// Created by marcel on 26.04.18.
//

#include "fcn_clocking_scheme.h"


fcn_clocking_scheme::fcn_clocking_scheme(const fcn_clock::cutout& c, const fcn_clock::number n, const bool r)
        :
        scheme(c),
        num_clocks(n),
        regular(r)
{}

fcn_clocking_scheme::fcn_clocking_scheme(fcn_clock::cutout&& c, fcn_clock::number&& n, bool&& r)
        :
        scheme(std::move(c)),
        num_clocks(std::move(n)),
        regular(std::move(r))
{}
