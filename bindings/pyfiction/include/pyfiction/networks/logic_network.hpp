//
// Created by marcel on 07.06.22.
//

#ifndef PYFICTION_LOGIC_NETWORK_HPP
#define PYFICTION_LOGIC_NETWORK_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/io/network_reader.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

//#include <mockturtle/networks/aig.hpp>
//#include <mockturtle/networks/mig.hpp>
//#include <mockturtle/networks/xag.hpp>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void network(pybind11::module& m, const std::string& network_name)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<Ntk>(m, network_name.c_str()).def(py::init<>())

        ;

    m.def(fmt::format("read_{}", network_name).c_str(),
          [](const std::string& filename) -> Ntk
          {
              auto reader = fiction::network_reader<std::shared_ptr<Ntk>>(filename, std::cout);

              if (const auto ntks = reader.get_networks(); !ntks.empty())
              {
                  return *ntks.front();
              }
              else
              {
                  throw std::runtime_error("Could not parse specification file");
              }
          });
}

}  // namespace detail

inline void logic_network(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    /**
     * Logic network.
     */
    detail::network<fiction::tec_nt>(m, "logic_network");
    //    detail::network<fiction::aig_nt>(m, "aig_network");
    //    detail::network<fiction::mig_nt>(m, "mig_network");
    //    detail::network<fiction::xag_nt>(m, "xag_network");
}

}  // namespace pyfiction

#endif  // PYFICTION_LOGIC_NETWORK_HPP
