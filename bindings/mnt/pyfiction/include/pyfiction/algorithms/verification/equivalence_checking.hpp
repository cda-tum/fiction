//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_EQUIVALENCE_CHECKING_HPP
#define PYFICTION_EQUIVALENCE_CHECKING_HPP

#include "pyfiction/documentation.hpp"
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
    namespace py = pybind11;

    /**
     * Result type for equivalence checking.
     */
    py::enum_<fiction::eq_type>(m, "eq_type", DOC(fiction_eq_type))
        .value("NO", fiction::eq_type::NO, DOC(fiction_eq_type_NO))
        .value("WEAK", fiction::eq_type::WEAK, DOC(fiction_eq_type_WEAK))
        .value("STRONG", fiction::eq_type::STRONG, DOC(fiction_eq_type_STRONG))

        ;

    py::class_<fiction::equivalence_checking_stats>(m, "equivalence_checking_stats",
                                                    DOC(fiction_equivalence_checking_stats))
        .def(py::init<>())
        .def_readonly("eq", &fiction::equivalence_checking_stats::eq, DOC(fiction_equivalence_checking_stats_eq))
        .def_readonly("tp_spec", &fiction::equivalence_checking_stats::tp_spec,
                      DOC(fiction_equivalence_checking_stats_tp_spec))
        .def_readonly("tp_impl", &fiction::equivalence_checking_stats::tp_impl,
                      DOC(fiction_equivalence_checking_stats_tp_impl))
        .def_readonly("tp_diff", &fiction::equivalence_checking_stats::tp_diff,
                      DOC(fiction_equivalence_checking_stats_tp_diff))
        .def_readonly("counter_example", &fiction::equivalence_checking_stats::counter_example,
                      DOC(fiction_equivalence_checking_stats_counter_example))
        .def_readonly("runtime", &fiction::equivalence_checking_stats::runtime,
                      DOC(fiction_equivalence_checking_stats_duration))
        .def_readonly("spec_drv_stats", &fiction::equivalence_checking_stats::spec_drv_stats,
                      DOC(fiction_equivalence_checking_stats_spec_drv_stats))
        .def_readonly("impl_drv_stats", &fiction::equivalence_checking_stats::impl_drv_stats,
                      DOC(fiction_equivalence_checking_stats_impl_drv_stats))

        ;

    detail::equivalence_checking<py_logic_network, py_logic_network>(m);
    detail::equivalence_checking<py_logic_network, py_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_logic_network, py_shifted_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_logic_network, py_hexagonal_gate_layout>(m);

    detail::equivalence_checking<py_cartesian_gate_layout, py_logic_network>(m);
    detail::equivalence_checking<py_cartesian_gate_layout, py_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_cartesian_gate_layout, py_shifted_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_cartesian_gate_layout, py_hexagonal_gate_layout>(m);

    detail::equivalence_checking<py_shifted_cartesian_gate_layout, py_logic_network>(m);
    detail::equivalence_checking<py_shifted_cartesian_gate_layout, py_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_shifted_cartesian_gate_layout, py_shifted_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_shifted_cartesian_gate_layout, py_hexagonal_gate_layout>(m);

    detail::equivalence_checking<py_hexagonal_gate_layout, py_logic_network>(m);
    detail::equivalence_checking<py_hexagonal_gate_layout, py_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_hexagonal_gate_layout, py_shifted_cartesian_gate_layout>(m);
    detail::equivalence_checking<py_hexagonal_gate_layout, py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_EQUIVALENCE_CHECKING_HPP
