//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_DESIGN_RULE_VIOLATIONS_HPP
#define PYFICTION_DESIGN_RULE_VIOLATIONS_HPP

#include "pybind11/pybind11.h"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/verification/design_rule_violations.hpp>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void gate_level_drvs(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "gate_level_drvs",
        [](const Lyt& lyt) -> std::pair<std::size_t, std::size_t>
        {
            std::ostringstream null_stream{};

            fiction::gate_level_drv_params params{};
            params.out = &null_stream;
            fiction::gate_level_drv_stats stats{};

            fiction::gate_level_drvs(lyt, params, &stats);

            return {stats.warnings, stats.drvs};
        },
        "layout"_a);
}

}  // namespace detail

inline void design_rule_violations(pybind11::module& m)
{
    detail::gate_level_drvs<py_cartesian_gate_layout>(m);
    detail::gate_level_drvs<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DESIGN_RULE_VIOLATIONS_HPP
