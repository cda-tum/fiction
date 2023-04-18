//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_EQUIVALENCE_CHECKING_HPP
#define PYFICTION_EQUIVALENCE_CHECKING_HPP

#include "pyfiction/docs.h"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/verification/equivalence_checking.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Spec, typename Impl>
void equivalence_checking(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "equivalence_checking",
        [](const Spec& spec, const Impl& impl, fiction::equivalence_checking_stats* pst = nullptr) -> fiction::eq_type
        {
            fiction::equivalence_checking_stats stats{};
            fiction::equivalence_checking(spec, impl, &stats);

            if (pst != nullptr)
            {
                *pst = stats;
            }

            return stats.eq;
        },
        "specification"_a, "implementation"_a, "statistics"_a = nullptr, DOC(fiction_equivalence_checking));
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

    pybind11::class_<fiction::equivalence_checking_stats>(m, "equivalence_checking_stats")
        .def(pybind11::init<>(), DOC(fiction_equivalence_checking_stats))
        .def_readwrite("counter_example", &fiction::equivalence_checking_stats::counter_example,
                       DOC(fiction_equivalence_checking_stats_counter_example))

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
