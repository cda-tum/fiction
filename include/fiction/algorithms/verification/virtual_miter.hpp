//
// Created by benjamin on 7/31/24.
//

#ifndef FICTION_VIRTUAL_MITER_HPP
#define FICTION_VIRTUAL_MITER_HPP

#include "fiction/networks/virtual_pi_network.hpp"

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <optional>

namespace fiction
{

/*! \brief Creates a combinational miter from two networks.
 *
 * This method combines two networks that have the same number of primary
 * inputs and the same number of primary outputs into a miter.  The miter
 * has the same number of inputs and one primary output.  This output is the
 * OR of XORs of all primary output pairs.  In other words, the miter outputs
 * 1 for all input assignments in which the two input networks differ.
 *
 * All networks may have different types.  The method returns an optional, which
 * is `nullopt`, whenever the two input networks don't match in their number of
 * primary inputs and primary outputs.
 */
template <class NtkDest, class NtkSource1, class NtkSource2>
std::optional<NtkDest> miter(NtkSource1 const& ntk1, NtkSource2 const& ntk2)
{
    static_assert(mockturtle::is_network_type_v<NtkSource1>, "NtkSource1 is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkSource2>, "NtkSource2 is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    static_assert(mockturtle::has_num_pis_v<NtkSource1>, "NtkSource1 does not implement the num_pis method");
    static_assert(mockturtle::has_num_pos_v<NtkSource1>, "NtkSource1 does not implement the num_pos method");
    static_assert(mockturtle::has_num_pis_v<NtkSource2>, "NtkSource2 does not implement the num_pis method");
    static_assert(mockturtle::has_num_pos_v<NtkSource2>, "NtkSource2 does not implement the num_pos method");
    static_assert(mockturtle::has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi method");
    static_assert(mockturtle::has_create_po_v<NtkDest>, "NtkDest does not implement the create_po method");
    static_assert(mockturtle::has_create_xor_v<NtkDest>, "NtkDest does not implement the create_xor method");
    static_assert(mockturtle::has_create_nary_or_v<NtkDest>, "NtkDest does not implement the create_nary_or method");

    auto num_pis1 = ntk1.num_pis();
    auto num_pis2 = ntk2.num_pis();
    if constexpr (has_num_real_pis_v<NtkSource1>)
    {
        num_pis1 = ntk1.num_real_pis();
    }
    if constexpr (has_num_real_pis_v<NtkSource2>)
    {
        num_pis2 = ntk2.num_real_pis();
    }
    /* both networks must have same number of inputs and outputs */
    if ((num_pis1 != num_pis2) || (ntk1.num_pos() != ntk2.num_pos()))
    {
        return std::nullopt;
    }

    /* create primary inputs */
    NtkDest                                  dest;
    std::vector<mockturtle::signal<NtkDest>> pis;
    for (auto i = 0u; i < num_pis1; ++i)
    {
        pis.push_back(dest.create_pi());
    }

    std::shared_ptr<NtkSource1> ntk1_copy;
    if constexpr (has_remove_virtual_input_nodes_v<NtkSource1>)
    {
        ntk1_copy = std::make_shared<NtkSource1>(ntk1);
        ntk1_copy->remove_virtual_input_nodes();
    }
    else
    {
        ntk1_copy = std::make_shared<NtkSource1>(ntk1);  // You may need const_cast here to remove the constness
    }

    std::shared_ptr<NtkSource2> ntk2_copy;
    if constexpr (has_remove_virtual_input_nodes_v<NtkSource2>)
    {
        ntk2_copy = std::make_shared<NtkSource2>(ntk2);
        ntk2_copy->remove_virtual_input_nodes();
    }
    else
    {
        ntk2_copy = std::make_shared<NtkSource2>(ntk2);  // You may need const_cast here to remove the constness
    }

    /* copy networks */
    const auto pos1 = mockturtle::cleanup_dangling(*ntk1_copy, dest, pis.begin(), pis.end());
    const auto pos2 = mockturtle::cleanup_dangling(*ntk2_copy, dest, pis.begin(), pis.end());

    if constexpr (mockturtle::has_EXODC_interface_v<NtkSource1>)
    {
        ntk1_copy->build_oe_miter(dest, pos1, pos2);
        return dest;
    }
    if constexpr (mockturtle::has_EXODC_interface_v<NtkSource2>)
    {
        ntk2_copy->build_oe_miter(dest, pos1, pos2);
        return dest;
    }

    /* create XOR of output pairs */
    std::vector<mockturtle::signal<NtkDest>> xor_outputs;
    std::transform(pos1.begin(), pos1.end(), pos2.begin(), std::back_inserter(xor_outputs),
                   [&](auto const& o1, auto const& o2) { return dest.create_xor(o1, o2); });

    /* create big OR of XOR gates */
    dest.create_po(dest.create_nary_or(xor_outputs));

    return dest;
}

}  // namespace fiction

#endif  // FICTION_VIRTUAL_MITER_HPP
