//
// Created by marcel on 26.05.21.
//

#ifndef FICTION_NAME_RESTORATION_HPP
#define FICTION_NAME_RESTORATION_HPP

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

namespace fiction
{

template <typename NtkSrc, typename NtkDest>
void restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest,
                   mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc>& old2new) noexcept
{
    if constexpr (mockturtle::has_get_network_name_v<NtkSrc> && mockturtle::has_set_network_name_v<NtkDest>)
    {
        ntk_dest.set_network_name(ntk_src.get_network_name());
    }

    if constexpr (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc> &&
                  mockturtle::has_set_name_v<NtkDest>)
    {
        const auto restore_signal_name = [&ntk_src, &ntk_dest, &old2new](const auto& f)
        {
            if (ntk_src.has_name(f))
            {
                auto name = ntk_src.get_name(f);

                ntk_dest.set_name(old2new[ntk_src.get_node(f)], name);
            }
        };

        ntk_src.foreach_node([&ntk_src, &restore_signal_name](const auto& n)
                             { ntk_src.foreach_fanin(n, restore_signal_name); });

        ntk_src.foreach_po(restore_signal_name);
    }
}

}  // namespace fiction

#endif  // FICTION_NAME_RESTORATION_HPP
