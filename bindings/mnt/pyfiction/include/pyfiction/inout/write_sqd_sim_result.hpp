//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_WRITE_SQD_SIM_RESULT_HPP
#define PYFICTION_WRITE_SQD_SIM_RESULT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/io/write_sqd_sim_result.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_sqd_sim_result(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "write_sqd_sim_result", [](const fiction::sidb_simulation_result<Lyt>& lyt, const std::string_view& filename)
        { fiction::write_sqd_sim_result(lyt, filename); }, py::arg("layout"), py::arg("filename"),
        DOC(fiction_write_sqd_sim_result));
}

}  // namespace detail

inline void write_sqd_sim_result(pybind11::module& m)
{
    detail::write_sqd_sim_result<py_sidb_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_SQD_SIM_RESULT_HPP
