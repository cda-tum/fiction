//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_EQUIVALENCE_CHECKING_HPP
#define PYFICTION_EQUIVALENCE_CHECKING_HPP

#include "pybind11/pybind11.h"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/verification/equivalence_checking.hpp>

namespace pyfiction
{

namespace detail
{

template <typename Spec, typename Impl>
void equivalence_checking(pybind11::module& m)
{
    m.def("equivalence_checking",
          [](const Spec& spec, const Impl& impl) -> fiction::eq_type
          {
              fiction::equivalence_checking_stats stats{};
              fiction::equivalence_checking(spec, impl, &stats);

              return stats.eq;
          });
}

}  // namespace detail

inline void equivalence_checking(pybind11::module& m)
{
    /**
     * Result type for equivalence checking.
     */
    pybind11::enum_<fiction::eq_type>(m, "eq_type")
        .value("NO", fiction::eq_type::NO)
        .value("WEAK", fiction::eq_type::WEAK)
        .value("STRONG", fiction::eq_type::STRONG)

        ;

    detail::equivalence_checking<py_logic_network, py_logic_network>(m);
    detail::equivalence_checking<py_logic_network, py_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_logic_network, py_hexagonal_gate_layout>(m);
    detail::equivalence_checking<py_cartesian_gate_layout, py_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_cartesian_gate_layout, py_hexagonal_gate_layout>(m);
    detail::equivalence_checking<py_hexagonal_gate_layout, py_hexagonal_gate_layout>(m);
    detail::equivalence_checking<py_hexagonal_gate_layout, py_cartesian_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_EQUIVALENCE_CHECKING_HPP
