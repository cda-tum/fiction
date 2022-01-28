//
// Created by marcel on 26.01.22.
//

#include <fiction/algorithms/verification/design_rule_violations.hpp>
#include <fiction/io/network_reader.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <kitty/print.hpp>
#include <mockturtle/algorithms/simulation.hpp>

#include <cstdint>
#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace pybind11::literals;

fiction::tec_nt create_logic_network(const std::string& filename)
{
    auto reader = fiction::network_reader<fiction::tec_ptr>(filename, std::cout);

    if (const auto ntks = reader.get_networks(); !ntks.empty())
    {
        return *ntks.front();
    }
    else
    {
        throw std::runtime_error("Could not parse specification file");
    }
}

fiction::offset::ucoord_t create_tile(const uint32_t x, const uint32_t y, const uint32_t z = 0)
{
    return {x, y, z};
}

fiction::cart_gate_clk_lyt create_gate_level_layout(const std::tuple<uint32_t, uint32_t>& dimension,
                                                    const std::string& name, const std::string& clocking)
{
    if (const auto scheme = fiction::get_clocking_scheme<fiction::cart_gate_clk_lyt>(clocking); scheme.has_value())
    {
        return {{std::get<0>(dimension), std::get<1>(dimension), 1}, *scheme, name};
    }
    else
    {
        throw std::runtime_error("Given name does not refer to a supported clocking scheme");
    }
}

uint64_t area(const fiction::cart_gate_clk_lyt& lyt)
{
    return lyt.area();
}

void resize(fiction::cart_gate_clk_lyt& lyt, const std::tuple<uint32_t, uint32_t>& dimension)
{
    lyt.resize({std::get<0>(dimension), std::get<1>(dimension), 1});
}

std::tuple<std::size_t, std::size_t> design_rule_checking(const fiction::cart_gate_clk_lyt& lyt)
{
    fiction::gate_level_drv_stats  st{};
    fiction::gate_level_drv_params ps{};

    std::stringstream stream{};
    ps.out = &stream;

    fiction::gate_level_drvs(lyt, ps, &st);

    return {st.drvs, st.warnings};
}

[[nodiscard]] py::list as_py_lists(const std::vector<fiction::tt>& spec)
{
    py::list spec_py_list{};

    for (const auto& tt : spec)
    {
        py::list tt_py_list{};

        const auto binary_tt_string = kitty::to_binary(tt);

        for (auto it = std::cbegin(binary_tt_string); it != std::cend(binary_tt_string); ++it)
        {
            tt_py_list.append(py::int_(*it == '0' ? 0 : 1));
        }

        spec_py_list.append(tt_py_list);
    }

    return spec_py_list;
}

template <typename NtkOrLyt>
py::list simulate(const NtkOrLyt& ntk_or_lyt)
{
    try
    {
        return as_py_lists(mockturtle::simulate<fiction::tt>(
            ntk_or_lyt, mockturtle::default_simulator<fiction::tt>(static_cast<unsigned>(ntk_or_lyt.num_pis()))));
    }
    catch (const std::bad_alloc&)
    {
        throw std::runtime_error("Layout has too many inputs to store its truth table");
    }
}

PYBIND11_MODULE(pyfiction, m)
{
    // docstring
    m.doc() = "Python interface for the fiction framework for Field-coupled Nanotechnologies";

    // logic networks
    py::class_<fiction::tec_nt>(m, "logic_network").def(py::init<>(&create_logic_network), "filename"_a);
    m.def("simulate", &simulate<fiction::tec_nt>, "ntk"_a, "Simulates the truth table of a network");

    // coordinates
    py::class_<fiction::offset::ucoord_t>(m, "tile").def(py::init<>(&create_tile), "x"_a, "y"_a, "z"_a = 0);

    // gate-level layouts
    py::class_<fiction::cart_gate_clk_lyt>(m, "gate_level_layout")
        .def(py::init<>(&create_gate_level_layout), "dimension"_a, "name"_a, "clocking"_a)
        .def("create_pi", &fiction::cart_gate_clk_lyt::create_pi, "name"_a, "t"_a)
        .def("create_po", &fiction::cart_gate_clk_lyt::create_po, "s"_a, "name"_a, "t"_a)
        .def("is_pi_tile", &fiction::cart_gate_clk_lyt::is_pi_tile, "t"_a)
        .def("is_po_tile", &fiction::cart_gate_clk_lyt::is_po_tile, "t"_a)
        .def("create_buf", &fiction::cart_gate_clk_lyt::create_buf, "a"_a, "t"_a)
        .def("create_not", &fiction::cart_gate_clk_lyt::create_not, "a"_a, "t"_a)
        .def("create_and", &fiction::cart_gate_clk_lyt::create_and, "a"_a, "b"_a, "t"_a)
        .def("create_nand", &fiction::cart_gate_clk_lyt::create_nand, "a"_a, "b"_a, "t"_a)
        .def("create_or", &fiction::cart_gate_clk_lyt::create_or, "a"_a, "b"_a, "t"_a)
        .def("create_nor", &fiction::cart_gate_clk_lyt::create_nor, "a"_a, "b"_a, "t"_a)
        .def("create_xor", &fiction::cart_gate_clk_lyt::create_xor, "a"_a, "b"_a, "t"_a)
        .def("create_xnor", &fiction::cart_gate_clk_lyt::create_xnor, "a"_a, "b"_a, "t"_a)
        .def("create_maj", &fiction::cart_gate_clk_lyt::create_maj, "a"_a, "b"_a, "c"_a, "t"_a);
    m.def("area", &area, "lyt"_a, "Return layout area");
    m.def("resize", &resize, "lyt"_a, "dimension"_a, "Resize a layout");
    m.def("simulate", &simulate<fiction::cart_gate_clk_lyt>, "lyt"_a, "Simulates the truth table of a layout");
    m.def("drc", &design_rule_checking, "lyt"_a, "Design rule checking that returns (#DRVs, #Warnings)");
}
