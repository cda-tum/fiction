//
// Created by marcel on 19.05.24.
//

#ifndef PYFICTION_BDL_INPUT_ITERATOR_HPP
#define PYFICTION_BDL_INPUT_ITERATOR_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void bdl_input_iterator(pybind11::module& m, const std::string& lattice)
{
    namespace py = pybind11;
    using namespace py::literals;

    py::class_<fiction::bdl_input_iterator<Lyt>>(m, fmt::format("bdl_input_iterator_{}", lattice).c_str(),
                                                 DOC(fiction_bdl_input_iterator))
        .def(py::init<const Lyt&, const fiction::bdl_input_iterator_params&>(), "lyt"_a,
             "params"_a = fiction::bdl_input_iterator_params{}, DOC(fiction_bdl_input_iterator_bdl_input_iterator))
        .def(
            "__next__",
            [](fiction::bdl_input_iterator<Lyt>& self) -> Lyt&
            {
                if (self >= ((1ull << self.num_input_pairs()) - 1))
                {
                    throw py::stop_iteration();
                }

                auto result = *self;
                ++self;
                return result;
            },
            DOC(fiction_bdl_input_iterator_operator_mul))
        .def(
            "__eq__", [](const fiction::bdl_input_iterator<Lyt>& self, const uint64_t m) -> bool { return self == m; },
            "m"_a, DOC(fiction_bdl_input_iterator_operator_eq))
        .def(
            "__ne__", [](const fiction::bdl_input_iterator<Lyt>& self, const uint64_t m) -> bool { return self != m; },
            "m"_a, DOC(fiction_bdl_input_iterator_operator_ne))
        .def(
            "__lt__", [](const fiction::bdl_input_iterator<Lyt>& self, const uint64_t m) -> bool { return self < m; },
            "m"_a, DOC(fiction_bdl_input_iterator_operator_lt))
        .def(
            "__le__", [](const fiction::bdl_input_iterator<Lyt>& self, const uint64_t m) -> bool { return self <= m; },
            "m"_a, DOC(fiction_bdl_input_iterator_operator_le))
        .def(
            "__gt__", [](const fiction::bdl_input_iterator<Lyt>& self, const uint64_t m) -> bool { return self > m; },
            "m"_a, DOC(fiction_bdl_input_iterator_operator_gt))
        .def(
            "__ge__", [](const fiction::bdl_input_iterator<Lyt>& self, const uint64_t m) -> bool { return self >= m; },
            "m"_a, DOC(fiction_bdl_input_iterator_operator_ge))
        .def(
            "__add__", [](const fiction::bdl_input_iterator<Lyt>& self, const int m) -> fiction::bdl_input_iterator<Lyt>
            { return self + m; }, "m"_a, DOC(fiction_bdl_input_iterator_operator_add))
        .def(
            "__iadd__",
            [](fiction::bdl_input_iterator<Lyt>& self, const int m) -> fiction::bdl_input_iterator<Lyt>&
            {
                self += m;
                return self;
            },
            "m"_a, DOC(fiction_bdl_input_iterator_operator_iadd))
        .def(
            "__sub__", [](const fiction::bdl_input_iterator<Lyt>& self, const int m) { return self - m; }, "m"_a,
            DOC(fiction_bdl_input_iterator_operator_sub))
        .def(
            "__isub__",
            [](fiction::bdl_input_iterator<Lyt>& self, const int m) -> fiction::bdl_input_iterator<Lyt>&
            {
                self -= m;
                return self;
            },
            "m"_a, DOC(fiction_bdl_input_iterator_operator_isub))
        .def(
            "__getitem__", [](const fiction::bdl_input_iterator<Lyt>& self, int m) -> fiction::bdl_input_iterator<Lyt>
            { return self[m]; }, "m"_a, DOC(fiction_bdl_input_iterator_operator_array))

        .def("num_input_pairs", &fiction::bdl_input_iterator<Lyt>::num_input_pairs)
        .def("get_layout", [](const fiction::bdl_input_iterator<Lyt>& self) -> const Lyt& { return *self; })

        ;
}

}  // namespace detail

inline void bdl_input_iterator(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace py::literals;

    /**
     * Input BDL configuration
     */
    py::enum_<typename fiction::bdl_input_iterator_params::input_bdl_configuration>(m, "input_bdl_configuration")
        .value("PERTURBER_ABSENCE_ENCODED",
               fiction::bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED,
               DOC(fiction_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_ABSENCE_ENCODED))
        .value("PERTURBER_DISTANCE_ENCODED",
               fiction::bdl_input_iterator_params::input_bdl_configuration::PERTURBER_DISTANCE_ENCODED,
               DOC(fiction_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_DISTANCE_ENCODED));

    /**
     * BDL input iterator parameters.
     */
    py::class_<fiction::bdl_input_iterator_params>(m, "bdl_input_iterator_params")
        .def(py::init<>())
        .def_readwrite("bdl_wires_params", &fiction::bdl_input_iterator_params::bdl_wire_params,
                       DOC(fiction_bdl_input_iterator_params_bdl_pairs_params))
        .def_readwrite("input_bdl_config", &fiction::bdl_input_iterator_params::input_bdl_config,
                       DOC(fiction_bdl_input_iterator_params_input_bdl_config));


    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::bdl_input_iterator<py_sidb_100_lattice>(m, "100");
    detail::bdl_input_iterator<py_sidb_111_lattice>(m, "111");
}

}  // namespace pyfiction

#endif  // PYFICTION_BDL_INPUT_ITERATOR_HPP
