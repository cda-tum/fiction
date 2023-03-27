//
// Created by marcel on 26.05.21.
//

#ifndef FICTION_NAME_UTILS_HPP
#define FICTION_NAME_UTILS_HPP

#include "fiction/utils/placement_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <string>
#include <string_view>

namespace fiction
{

/**
 * Helper function to conveniently fetch the name from a layout or network as they use different function names for the
 * same purpose.
 *
 * @tparam NtkOrLyt Network or layout type.
 * @param ntk_or_lyt Network or layout object.
 * @return Name of given network or layout.
 */
template <typename NtkOrLyt>
std::string get_name(const NtkOrLyt& ntk_or_lyt) noexcept
{
    if constexpr (mockturtle::has_get_network_name_v<NtkOrLyt>)
    {
        return ntk_or_lyt.get_network_name();
    }
    else if constexpr (fiction::has_get_layout_name_v<NtkOrLyt>)
    {
        return ntk_or_lyt.get_layout_name();
    }

    return {};
}
/**
 * Helper function to conveniently assign a name to a layout or network as they use different function names for the
 * same purpose.
 *
 * @tparam NtkOrLyt Network or layout type.
 * @param ntk_or_lyt Network or layout object.
 * @param name Name to assign to given network or layout.
 */
template <typename NtkOrLyt>
void set_name(NtkOrLyt& ntk_or_lyt, const std::string_view& name) noexcept
{
    if constexpr (mockturtle::has_set_network_name_v<NtkOrLyt>)
    {
        return ntk_or_lyt.set_network_name(name.data());
    }
    else if constexpr (fiction::has_set_layout_name_v<NtkOrLyt>)
    {
        return ntk_or_lyt.set_layout_name(name.data());
    }
}
/**
 * Helper function to conveniently assign the name of a source network or layout to a target network or layout as they
 * use different function names for the same purpose. This function comes in handy when networks are translated or
 * layouts are being created from networks that are supposed to have the same name.
 *
 * @tparam NtkOrLytSrc Source network or layout type.
 * @tparam NtkOrLytDest Target network or layout type.
 * @param ntk_or_lyt_src Source network or layout whose name is to be assigned to `ntk_or_lyt_dest`.
 * @param ntk_or_lyt_dest Target network or layout that is to be assigned `ntk_or_lyt_src`'s name.
 */
template <typename NtkOrLytSrc, typename NtkOrLytDest>
void restore_network_name(const NtkOrLytSrc& ntk_or_lyt_src, NtkOrLytDest& ntk_or_lyt_dest) noexcept
{
    static_assert(mockturtle::is_network_type_v<NtkOrLytSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkOrLytDest>, "NtkDest is not a network type");

    std::string network_name{};

    if constexpr (mockturtle::has_get_network_name_v<NtkOrLytSrc>)
    {
        network_name = ntk_or_lyt_src.get_network_name();
    }
    else if constexpr (fiction::has_get_layout_name_v<NtkOrLytSrc>)
    {
        network_name = ntk_or_lyt_src.get_layout_name();
    }

    if constexpr (mockturtle::has_set_network_name_v<NtkOrLytDest>)
    {
        ntk_or_lyt_dest.set_network_name(network_name);
    }
    else if constexpr (fiction::has_set_layout_name_v<NtkOrLytDest>)
    {
        ntk_or_lyt_dest.set_layout_name(network_name);
    }
}
/**
 * Assigns input names from one network to another. Matching inputs are identified by their index. Since gate-level
 * layout's are network types as well, this function naturally works for them, too.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network type.
 * @param ntk_src Source logic network whose input names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose inputs are to be assigned `ntk_src`'s names.
 */
template <typename NtkSrc, typename NtkDest>
void restore_input_names(const NtkSrc& ntk_src, NtkDest& ntk_dest) noexcept
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    if constexpr (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc> &&
                  mockturtle::has_set_name_v<NtkDest>)
    {
        static_assert(mockturtle::has_foreach_pi_v<NtkSrc>, "NtkSrc does not implement the foreach_pi function");
        static_assert(mockturtle::has_make_signal_v<NtkSrc>, "NtkSrc does not implement the make_signal function");
        static_assert(mockturtle::has_make_signal_v<NtkDest>, "NtkDest does not implement the make_signal function");
        static_assert(mockturtle::has_pi_at_v<NtkDest>, "NtkDest does not implement the pi_at function");

        ntk_src.foreach_pi(
            [&ntk_src, &ntk_dest](const auto& pi, auto i)
            {
                if (const auto pi_signal = ntk_src.make_signal(pi); ntk_src.has_name(pi_signal))
                {
                    ntk_dest.set_name(ntk_dest.make_signal(ntk_dest.pi_at(i)), ntk_src.get_name(pi_signal));
                }
            });
    }
}
/**
 * Assigns output names from one network to another. Matching outputs are identified by their order. Since gate-level
 * layout's are network types as well, this function naturally works for them, too.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network type.
 * @param ntk_src Source logic network whose output names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose outputs are to be assigned `ntk_src`'s names.
 */
template <typename NtkSrc, typename NtkDest>
void restore_output_names(const NtkSrc& ntk_src, NtkDest& ntk_dest) noexcept
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    if constexpr (mockturtle::has_has_output_name_v<NtkSrc> && mockturtle::has_get_output_name_v<NtkSrc> &&
                  mockturtle::has_set_output_name_v<NtkDest>)
    {
        static_assert(mockturtle::has_foreach_po_v<NtkSrc>, "NtkSrc does not implement the foreach_po function");

        ntk_src.foreach_po(
            [&ntk_src, &ntk_dest]([[maybe_unused]] const auto& po, const auto i)
            {
                if (ntk_src.has_output_name(i))
                {
                    auto name = ntk_src.get_output_name(i);

                    ntk_dest.set_output_name(i, name);
                }
            });
    }
}
/**
 * Assigns all signal names from one network to another. For this purpose, a mapping between signals is needed in terms
 * of a `mockturtle::node_map`. Since gate-level layout's are network types as well, this function naturally works for
 * them, too.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network type.
 * @param ntk_src Source logic network whose signal names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose signal names are to be assigned `ntk_src`'s names.
 * @param old2new Mapping of signals from `ntk_src` to `ntk_dest`.
 */
template <typename NtkSrc, typename NtkDest>
void restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest,
                          const mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc>& old2new) noexcept
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    if constexpr (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc> &&
                  mockturtle::has_set_name_v<NtkDest>)
    {
        static_assert(mockturtle::has_foreach_node_v<NtkSrc>, "NtkSrc does not implement the foreach_node function");
        static_assert(mockturtle::has_foreach_fanin_v<NtkSrc>, "NtkSrc does not implement the foreach_fanin function");
        static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node function");

        const auto restore_signal_name = [&ntk_src, &ntk_dest, &old2new](const auto& f)
        {
            if (ntk_src.has_name(f))
            {
                const auto name = ntk_src.get_name(f);

                ntk_dest.set_name((old2new[ntk_src.get_node(f)]), name);
            }
        };

        ntk_src.foreach_node([&ntk_src, &restore_signal_name](const auto& n)
                             { ntk_src.foreach_fanin(n, restore_signal_name); });
    }
}
/**
 * Same as the other restore_signal_names function but this overload uses a `mockturtle::node_map` with a
 * branching_signal_container that is specifically used for networks or layouts that allow branches to be distinct,
 * e.g., by their position on the layout.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network type.
 * @tparam fanout_size Maximum fanout size in the network.
 * @param ntk_src Source logic network whose signal names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose signal names are to be assigned `ntk_src`'s names.
 * @param old2new Mapping of signals from `ntk_src` to `ntk_dest` using a branching_signal_container.
 */
template <typename NtkSrc, typename NtkDest, uint16_t fanout_size = 2>
void restore_signal_names(
    const NtkSrc& ntk_src, NtkDest& ntk_dest,
    const mockturtle::node_map<branching_signal_container<NtkDest, NtkSrc, fanout_size>, NtkSrc>& old2new) noexcept
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    if constexpr (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc> &&
                  mockturtle::has_set_name_v<NtkDest>)
    {
        static_assert(mockturtle::has_foreach_node_v<NtkSrc>, "NtkSrc does not implement the foreach_node function");
        static_assert(mockturtle::has_foreach_fanin_v<NtkSrc>, "NtkSrc does not implement the foreach_fanin function");
        static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node function");

        const auto restore_signal_name = [&ntk_src, &ntk_dest, &old2new](const auto& n, const auto& f)
        {
            if (ntk_src.has_name(f))
            {
                const auto name = ntk_src.get_name(f);

                ntk_dest.set_name((old2new[ntk_src.get_node(f)][n]), name);
            }
        };

        ntk_src.foreach_node(
            [&ntk_src, &restore_signal_name](const auto& n)
            { ntk_src.foreach_fanin(n, [&restore_signal_name, &n](const auto& f) { restore_signal_name(n, f); }); });
    }
}
/**
 * Transfers all input and output names as well as the network/layout name from one network to another. This function
 * calls `restore_network_name`, `restore_input_names`, and `restore_output_names`.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network type.
 * @param ntk_src Source logic network whose I/O names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose I/O names are to be assigned `ntk_src`'s names.
 */
template <typename NtkSrc, typename NtkDest>
void restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest) noexcept
{
    restore_network_name(ntk_src, ntk_dest);
    restore_input_names(ntk_src, ntk_dest);
    restore_output_names(ntk_src, ntk_dest);
}
/**
 * Transfers all signal and output names as well as the network/layout name from one network to another. This function
 * calls `restore_network_name`, `restore_signal_names`, and `restore_output_names`.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network type.
 * @tparam T Mapping type to identify signals by. Currently, `mockturtle::signal<NtkDest>` and
 * `branching_signal_container<NtkDest, NtkSrc, fanout_size>` are supported.
 * @param ntk_src Source logic network whose signal names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose signal names are to be assigned `ntk_src`'s names.
 * @param old2new Mapping of signals from `ntk_src` to `ntk_dest` using a signal identifier.
 */
template <typename NtkSrc, typename NtkDest, typename T>
void restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, mockturtle::node_map<T, NtkSrc>& old2new) noexcept
{
    restore_network_name(ntk_src, ntk_dest);
    restore_input_names(ntk_src, ntk_dest);
    restore_signal_names(ntk_src, ntk_dest, old2new);
    restore_output_names(ntk_src, ntk_dest);
}

}  // namespace fiction

#endif  // FICTION_NAME_UTILS_HPP
