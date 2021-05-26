//
// Created by marcel on 26.05.21.
//

#ifndef FICTION_NETWORK_WRITER_HPP
#define FICTION_NETWORK_WRITER_HPP

#include "dot_drawers.hpp"

#include <mockturtle/io/write_dot.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fiction::debug
{

template <typename Ntk>
void write_dot_network(const Ntk& ntk, const std::string& name = "ntk", const std::filesystem::path& p = {"./"})
{
    std::ofstream file{p / (name + ".dot")};

    mockturtle::write_dot(ntk, file, fiction::topology_dot_drawer<Ntk, true>{});

    file.close();
}

}  // namespace fiction::debug

#endif  // FICTION_NETWORK_WRITER_HPP
