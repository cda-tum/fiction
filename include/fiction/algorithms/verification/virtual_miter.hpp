//
// Created by benjamin on 31.07.24.
//

#ifndef FICTION_VIRTUAL_MITER_HPP
#define FICTION_VIRTUAL_MITER_HPP

#include "fiction/traits.hpp"

#include <fiction/algorithms/network_transformation/delete_virtual_pis.hpp>

#include <mockturtle/traits.hpp>

#include <algorithm>
#include <optional>
#include <vector>

namespace fiction
{

/**
 * Removes virtual primary inputs from a network if supported. Otherwise the input network is returned unmodified.
 *
 * @tparam Ntk The network type.
 * @param network The input network to process.
 * @return The network with virtual primary inputs removed, or the original network if unsupported.
 */
template <typename Ntk>
Ntk handle_virtual_pis(const Ntk& network)
{
    if constexpr (has_num_virtual_pis_v<Ntk>)
    {
        return delete_virtual_pis(network);
    }
    else
    {
        return network;
    }
}

/**
 * Combines two networks into a combinational miter, similar to `mockturtle::miter`. Either input network  may include
 * virtual primary inputs (PIs). Virtual PIs are removed during miter construction, and all edges connected to them are
 * remapped to their corresponding original PIs, ensuring consistent PI counts when the networks are equivalent.
 *
 * The resulting miter connects two networks with the same number of inputs and produces a single primary output. This
 * output represents the OR of the XORs of all primary output pairs. Thus, the miter outputs 1 for any input assignment
 * where the primary outputs of the two networks differ.
 *
 * The input networks may have different types. If the two input networks have mismatched numbers of primary inputs or
 * outputs, the method returns `nullopt`.
 *
 * @tparam NtkDest The type of the resulting network.
 * @tparam NtkSource1 The type of the first input network.
 * @tparam NtkSource2 The type of the second input network.
 * @param ntk1_in The first input network.
 * @param ntk2_in The second input network.
 * @return An `optional` containing the virtual miter network if successful, or `nullopt` if the networks are
 * incompatible.
 */

template <typename NtkDest, typename NtkSrc1, typename NtkSrc2>
[[nodiscard]] std::optional<NtkDest> virtual_miter(const NtkSrc1& ntk1_in, const NtkSrc2& ntk2_in) noexcept
{
    static_assert(mockturtle::is_network_type_v<NtkSrc1>, "NtkSource1 is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkSrc2>, "NtkSource2 is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    static_assert(mockturtle::has_num_pis_v<NtkSrc1>, "NtkSource1 does not implement the num_pis method");
    static_assert(mockturtle::has_num_pos_v<NtkSrc1>, "NtkSource1 does not implement the num_pos method");
    static_assert(mockturtle::has_num_pis_v<NtkSrc2>, "NtkSource2 does not implement the num_pis method");
    static_assert(mockturtle::has_num_pos_v<NtkSrc2>, "NtkSource2 does not implement the num_pos method");
    static_assert(mockturtle::has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi method");
    static_assert(mockturtle::has_create_po_v<NtkDest>, "NtkDest does not implement the create_po method");
    static_assert(mockturtle::has_create_xor_v<NtkDest>, "NtkDest does not implement the create_xor method");
    static_assert(mockturtle::has_create_nary_or_v<NtkDest>, "NtkDest does not implement the create_nary_or method");

    // handle (delete and remap) virtual primary inputs
    const NtkSrc1 ntk1 = handle_virtual_pis(ntk1_in);
    const NtkSrc2 ntk2 = handle_virtual_pis(ntk2_in);

    /* both networks must have same number of inputs and outputs */
    if ((ntk1.num_pis() != ntk2.num_pis()) || (ntk1.num_pos() != ntk2.num_pos()))
    {
        return std::nullopt;
    }

    /* create primary inputs */
    NtkDest                                  dest;
    std::vector<mockturtle::signal<NtkDest>> pis{};
    pis.reserve(ntk1.num_pis());
    for (auto i = 0u; i < ntk1.num_pis(); ++i)
    {
        pis.push_back(dest.create_pi());
    }

    /* copy networks */
    const auto pos1 = cleanup_dangling(ntk1, dest, pis.cbegin(), pis.cend());
    const auto pos2 = cleanup_dangling(ntk2, dest, pis.cbegin(), pis.cend());

    if constexpr (mockturtle::has_EXODC_interface_v<decltype(ntk1)>)
    {
        ntk1.build_oe_miter(dest, pos1, pos2);
        return dest;
    }
    if constexpr (mockturtle::has_EXODC_interface_v<decltype(ntk2)>)
    {
        ntk2.build_oe_miter(dest, pos1, pos2);
        return dest;
    }

    /* create XOR of output pairs */
    std::vector<mockturtle::signal<NtkDest>> xor_outputs{};
    xor_outputs.reserve(ntk1.num_pos());
    std::transform(pos1.begin(), pos1.end(), pos2.begin(), std::back_inserter(xor_outputs),
                   [&](auto const& o1, auto const& o2) { return dest.create_xor(o1, o2); });

    /* create big OR of XOR gates */
    dest.create_po(dest.create_nary_or(xor_outputs));

    return dest;
}

}  // namespace fiction

#endif  // FICTION_VIRTUAL_MITER_HPP
