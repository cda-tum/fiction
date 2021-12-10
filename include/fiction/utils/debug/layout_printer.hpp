//
// Created by marcel on 15.09.21.
//

#ifndef FICTION_LAYOUT_PRINTER_HPP
#define FICTION_LAYOUT_PRINTER_HPP

#include <fmt/format.h>

#include <iostream>

namespace fiction::debug
{

template <typename Lyt>
void print_node_to_tile_assignments(const Lyt& lyt)
{
    std::cout << std::endl << "---- NODE TO TILE ----" << std::endl;

    lyt.foreach_node(
        [&lyt](const auto& n)
        {
            std::cout << fmt::format("node {} @ {}", n, lyt.get_tile(n)) << std::endl;
            std::cout << "\t with fanins:" << std::endl;
            lyt.foreach_fanin(n,
                              [&lyt](const auto& f)
                              {
                                  auto fn = lyt.get_node(f);
                                  std::cout << fmt::format("\t   {} @ {}", fn, lyt.get_tile(fn)) << std::endl;
                              });
            std::cout << std::endl;
            std::cout << "\t with fanouts:" << std::endl;
            lyt.foreach_fanout(n, [&lyt](const auto& fn)
                               { std::cout << fmt::format("\t   {} @ {}", fn, lyt.get_tile(fn)) << std::endl; });
        });

    std::cout << std::endl << "----------------------" << std::endl;
}

template <typename Lyt>
void print_tile_to_node_assignments(const Lyt& lyt)
{
    std::cout << std::endl << "---- TILE TO NODE ----" << std::endl;

    lyt.foreach_tile(
        [&lyt](const auto& t)
        {
            std::cout << fmt::format("tile {}", t) << std::endl;

            const auto n = lyt.get_node(t);

            if (n != 0)
            {
                std::cout << fmt::format("node {} @ {}", n, lyt.get_tile(n)) << std::endl;
                std::cout << "\t with fanins:" << std::endl;
                lyt.foreach_fanin(n,
                                  [&lyt](const auto& f)
                                  {
                                      auto fn = lyt.get_node(f);
                                      std::cout << fmt::format("\t   {} @ {}", fn, lyt.get_tile(fn)) << std::endl;
                                  });
                std::cout << std::endl;
                std::cout << "\t with fanouts:" << std::endl;
                lyt.foreach_fanout(n, [&lyt](const auto& fn)
                                   { std::cout << fmt::format("\t   {} @ {}", fn, lyt.get_tile(fn)) << std::endl; });

                std::cout << std::endl;
            }
        });

    std::cout << std::endl << "----------------------" << std::endl;
}

}  // namespace fiction::debug

#endif  // FICTION_LAYOUT_PRINTER_HPP
