//
// Created by marcel on 30.08.21.
//

#ifndef FICTION_SYNCHRONIZATION_ELEMENT_LAYOUT_HPP
#define FICTION_SYNCHRONIZATION_ELEMENT_LAYOUT_HPP

#include "fiction/traits.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <set>
#include <unordered_map>

namespace fiction
{

/**
 * This layout provides synchronization elements on top of a clocked layout as a technology extension to the FCN concept
 * proposed in "Synchronization of Clocked Field-Coupled Circuits" by F. Sill Torres, M. Walter, R. Wille, D. Große, and
 * R. Drechsler in IEEE NANO 2018. More in-depth information can be found in "Design Automation for Field-coupled
 * Nanotechnologies" by M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by Springer Nature in 2022.
 *
 * A synchronization element is a clock zone whose clock signal is altered such that it provides an extended Hold phase
 * of a multitude of full clock cycles. It thereby stalls information in place and acts as a latch. Additionally, the
 * pipeline-like behavior of FCN circuits allows clock zones that act as synchronization elements to transmit
 * information to every clock number instead of just its consecutive one.
 *
 * The exploration of synchronization elements further allows for interesting sequential applications like D Latches
 * built from single MAJ gates as also proposed in the above publications. However, fiction does not yet support
 * sequential FCN circuits. Therefore, support for these applications is limited. It is advised to use synchronization
 * elements only on wire tiles.
 *
 * @tparam ClockedLayout The clocked layout type whose clocking system should be extended by synchronization elements.
 */
template <typename ClockedLayout>
class synchronization_element_layout : public ClockedLayout
{
  public:
#pragma region Types and constructors

    using sync_elem_t = uint8_t;

    struct synchronization_element_layout_storage
    {
        explicit synchronization_element_layout_storage() noexcept = default;

        std::unordered_map<uint64_t, sync_elem_t> se_map{};
    };

    using base_type = synchronization_element_layout;

    using storage = std::shared_ptr<synchronization_element_layout_storage>;

    /**
     * Standard constructor. Creates a clocked synchronization element layout of the given aspect ratio. To this end, it
     * calls ClockedLayout's standard constructor.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit synchronization_element_layout(const aspect_ratio<ClockedLayout>& ar = {}) :
            ClockedLayout(ar),
            strg{std::make_shared<synchronization_element_layout_storage>()}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Standard constructor. Creates a clocked synchronization element layout of the given aspect ratio and clocks it
     * via the given clocking scheme. To this end, it calls ClockedLayout's standard constructor.
     *
     * @param ar Highest possible position in the layout.
     * @param scheme Clocking scheme to apply to this layout.
     */
    synchronization_element_layout(const aspect_ratio<ClockedLayout>&               ar,
                                   const typename ClockedLayout::clocking_scheme_t& scheme) :
            ClockedLayout(ar, scheme),
            strg{std::make_shared<synchronization_element_layout_storage>()}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Copy constructor from another layout's storage.
     *
     * @param s Storage of another synchronization_element_layout.
     */
    explicit synchronization_element_layout(std::shared_ptr<synchronization_element_layout_storage> s) :
            strg{std::move(s)}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }

#pragma endregion

#pragma region Clocking

    /**
     * Assigns a synchronization element to the provided clock zone.
     *
     * @param cz Clock zone to turn into a synchronization element.
     * @param se Number of full clock cycles to extend cz's Hold phase by. If this value is 0, cz is turned back into a
     * normal clock zone.
     */
    void assign_synchronization_element(const typename ClockedLayout::clock_zone& cz, const sync_elem_t se) noexcept
    {
        if (se == sync_elem_t{0})
        {
            strg->se_map.erase(static_cast<uint64_t>(cz));
        }
        else
        {
            strg->se_map[static_cast<uint64_t>(cz)] = se;
        }
    }
    /**
     * Check whether the provided clock zone is a synchronization element.
     *
     * @param cz Clock zone to check.
     * @return True iff cz is a synchronization element.
     */
    [[nodiscard]] bool is_synchronization_element(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        if (auto it = strg->se_map.find(static_cast<uint64_t>(cz)); it != strg->se_map.end())
        {
            return it->second != sync_elem_t{0};
        }

        return false;
    }
    /**
     * Returns the Hold phase extension in clock cycles of clock zone cz.
     *
     * @param cz Clock zone to check.
     * @return Synchronization element value, i.e., Hold phase extension, of clock zone cz.
     */
    [[nodiscard]] sync_elem_t get_synchronization_element(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        if (auto it = strg->se_map.find(static_cast<uint64_t>(cz)); it != strg->se_map.end())
        {
            return it->second;
        }

        return 0;
    }
    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Evaluates whether clock zone
     * cz2 feeds information to clock zone cz1, i.e., whether cz2 is clocked with a clock number that is lower by 1
     * modulo num_clocks() or if one of them is a synchronization element. Due to their extended Hold phase, they feed
     * information to any clock number.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether it feeds information to cz1.
     * @return True iff cz2's clock number is lower by 1 modulo num_clocks() or if cz1 or cz2 are synchronization
     * elements.
     */
    [[nodiscard]] bool is_incoming_clocked(const typename ClockedLayout::clock_zone& cz1,
                                           const typename ClockedLayout::clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        return ClockedLayout::is_incoming_clocked(cz1, cz2) || is_synchronization_element(cz1) ||
               is_synchronization_element(cz2);
    }
    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Evaluates whether clock zone
     * cz1 feeds information to clock zone cz2, i.e., whether cz2 is clocked with a clock number that is higher by 1
     * modulo num_clocks() or if one of them is a synchronization element. Due to their extended Hold phase, they feed
     * information to any clock number.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether it accepts information from cz1.
     * @return True iff cz2's clock number is higher by 1 modulo num_clocks() or if cz1 or cz2 are synchronization
     * elements.
     */
    [[nodiscard]] bool is_outgoing_clocked(const typename ClockedLayout::clock_zone& cz1,
                                           const typename ClockedLayout::clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        return ClockedLayout::is_outgoing_clocked(cz1, cz2) || is_synchronization_element(cz1) ||
               is_synchronization_element(cz2);
    }

#pragma endregion

#pragma region Iteration

    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Returns a container of type
     * Container with all clock zones that are incoming to the given one.
     *
     * @tparam Container Container type that holds clock zones.
     * @param cz Base clock zone.
     * @return A container of type Container with all clock zones that are incoming to cz.
     */
    template <typename Container>
    [[nodiscard]] Container incoming_clocked_zones(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        const auto adj = ClockedLayout::template adjacent_coordinates<Container>(cz);

        Container incoming{};

        std::copy_if(std::cbegin(adj), std::cend(adj), std::inserter(incoming, std::cend(incoming)),
                     [this, &cz](const auto& ct) { return is_incoming_clocked(cz, ct); });

        return incoming;
    }
    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Applies a function to all
     * incoming clock zones of a given one in accordance with incoming_clocked_zones.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by mockturtle::foreach_element.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of cz's incoming clock zones.
     */
    template <typename Fn>
    void foreach_incoming_clocked_zone(const typename ClockedLayout::clock_zone& cz, Fn&& fn) const
    {
        const auto incoming = incoming_clocked_zones<std::set<typename ClockedLayout::clock_zone>>(cz);

        mockturtle::detail::foreach_element(incoming.cbegin(), incoming.cend(), fn);
    }
    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Returns a container of type
     * Container with all clock zones that are outgoing from the given one.
     *
     * @tparam Container Container type that holds clock zones.
     * @param cz Base clock zone.
     * @return A container of type Container with all clock zones that are outgoing from cz.
     */
    template <typename Container>
    [[nodiscard]] Container outgoing_clocked_zones(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        const auto adj = ClockedLayout::template adjacent_coordinates<Container>(cz);

        Container outgoing{};

        std::copy_if(std::cbegin(adj), std::cend(adj), std::inserter(outgoing, std::cend(outgoing)),
                     [this, &cz](const auto& ct) { return is_outgoing_clocked(cz, ct); });

        return outgoing;
    }
    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Applies a function to all
     * outgoing clock zones of a given one in accordance with outgoing_clocked_zones.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by mockturtle::foreach_element.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of cz's outgoing clock zones.
     */
    template <typename Fn>
    void foreach_outgoing_clocked_zone(const typename ClockedLayout::clock_zone& cz, Fn&& fn) const
    {
        const auto outgoing = outgoing_clocked_zones<std::set<typename ClockedLayout::clock_zone>>(cz);

        mockturtle::detail::foreach_element(outgoing.cbegin(), outgoing.cend(), fn);
    }

#pragma endregion

#pragma region Structural properties

    /**
     * Returns the number of clock zones acting as synchronization elements in the layout.
     *
     * @return Number of synchronization elements in the layout.
     */
    [[nodiscard]] uint32_t num_se() const noexcept
    {
        return static_cast<uint32_t>(strg->se_map.size());
    }
    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Returns the number of
     * incoming clock zones to the given one.
     *
     * @param cz Base clock zone.
     * @return Number of cz's incoming clock zones.
     */
    [[nodiscard]] typename ClockedLayout::degree_t
    in_degree(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        return static_cast<typename ClockedLayout::degree_t>(
            incoming_clocked_zones<std::set<typename ClockedLayout::clock_zone>>(cz).size());
    }
    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Returns the number of
     * outgoing clock zones from the given one.
     *
     * @param cz Base clock zone.
     * @return Number of cz's outgoing clock zones.
     */
    [[nodiscard]] typename ClockedLayout::degree_t
    out_degree(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        return static_cast<typename ClockedLayout::degree_t>(
            outgoing_clocked_zones<std::set<typename ClockedLayout::clock_zone>>(cz).size());
    }
    /**
     * Overwrites the function from ClockedLayout to account for synchronization elements. Returns the number of
     * incoming plus outgoing clock zones of the given one.
     *
     * @param cz Base clock zone.
     * @return Number of cz's incoming plus outgoing clock zones.
     */
    [[nodiscard]] typename ClockedLayout::degree_t degree(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        const auto incoming = incoming_clocked_zones<std::set<typename ClockedLayout::clock_zone>>(cz);
        const auto outgoing = outgoing_clocked_zones<std::set<typename ClockedLayout::clock_zone>>(cz);

        std::set<typename ClockedLayout::clock_zone> merged_clock_zones{};
        std::merge(incoming.cbegin(), incoming.cend(), outgoing.cbegin(), outgoing.cend(),
                   std::inserter(merged_clock_zones, merged_clock_zones.end()));

        return static_cast<typename ClockedLayout::degree_t>(merged_clock_zones.size());
    }

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_SYNCHRONIZATION_ELEMENT_LAYOUT_HPP
