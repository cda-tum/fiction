//
// Created by marcel on 30.08.21.
//

#ifndef FICTION_SYNCHRONIZATION_ELEMENT_LAYOUT_HPP
#define FICTION_SYNCHRONIZATION_ELEMENT_LAYOUT_HPP

#include "fiction/traits.hpp"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This layout provides synchronization elements on top of a clocked layout as a technology extension to the FCN concept
 * proposed in \"Synchronization of Clocked Field-Coupled Circuits\" by F. Sill Torres, M. Walter, R. Wille, D. Große,
 * and R. Drechsler in IEEE NANO 2018. More in-depth information can be found in \"Design Automation for Field-coupled
 * Nanotechnologies\" by M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by Springer Nature in 2022.
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
     * calls `ClockedLayout`'s standard constructor.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit synchronization_element_layout(const typename ClockedLayout::aspect_ratio_type& ar = {}) :
            ClockedLayout(ar),
            strg{std::make_shared<synchronization_element_layout_storage>()}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Standard constructor. Creates a clocked synchronization element layout of the given aspect ratio and clocks it
     * via the given clocking scheme. To this end, it calls `ClockedLayout`'s standard constructor.
     *
     * @param ar Highest possible position in the layout.
     * @param scheme Clocking scheme to apply to this layout.
     */
    synchronization_element_layout(const typename ClockedLayout::aspect_ratio_type&      ar,
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
    /**
     * Copy constructor from another `ClockedLayout`.
     *
     * @param lyt Clocked layout.
     */
    explicit synchronization_element_layout(const ClockedLayout& lyt) :
            ClockedLayout(lyt),
            strg{std::make_shared<synchronization_element_layout_storage>()}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] synchronization_element_layout clone() const noexcept
    {
        auto copy = synchronization_element_layout(ClockedLayout::clone());
        copy.strg = std::make_shared<synchronization_element_layout_storage>(*strg);

        return copy;
    }

#pragma endregion

#pragma region Clocking

    /**
     * Assigns a synchronization element to the provided clock zone.
     *
     * @param cz Clock zone to turn into a synchronization element.
     * @param se Number of full clock cycles to extend `cz`'s Hold phase by. If this value is 0, `cz` is turned back
     * into a normal clock zone.
     */
    void assign_synchronization_element(const clock_zone<ClockedLayout>& cz, const sync_elem_t se) noexcept
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
     * @return `true` iff `cz` is a synchronization element.
     */
    [[nodiscard]] bool is_synchronization_element(const clock_zone<ClockedLayout>& cz) const noexcept
    {
        if (auto it = strg->se_map.find(static_cast<uint64_t>(cz)); it != strg->se_map.end())
        {
            return it->second != sync_elem_t{0};
        }

        return false;
    }
    /**
     * Returns the Hold phase extension in clock cycles of clock zone `cz`.
     *
     * @param cz Clock zone to check.
     * @return Synchronization element value, i.e., Hold phase extension, of clock zone `cz`.
     */
    [[nodiscard]] sync_elem_t get_synchronization_element(const clock_zone<ClockedLayout>& cz) const noexcept
    {
        if (auto it = strg->se_map.find(static_cast<uint64_t>(cz)); it != strg->se_map.end())
        {
            return it->second;
        }

        return 0;
    }
    /**
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Evaluates whether clock
     * zone `cz2` feeds information to clock zone `cz1`, i.e., whether `cz2` is clocked with a clock number that is
     * lower by 1 modulo `num_clocks()` or if one of them is a synchronization element. Due to their extended Hold
     * phase, they feed information to any clock number.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether it feeds information to `cz1`.
     * @return `true` iff `cz2`'s clock number is lower by 1 modulo `num_clocks()` or if `cz1` or `cz2` are
     * synchronization elements.
     */
    [[nodiscard]] bool is_incoming_clocked(const clock_zone<ClockedLayout>& cz1,
                                           const clock_zone<ClockedLayout>& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        return ClockedLayout::is_incoming_clocked(cz1, cz2) || is_synchronization_element(cz1) ||
               is_synchronization_element(cz2);
    }
    /**
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Evaluates whether clock
     * zone `cz1` feeds information to clock zone `cz2`, i.e., whether `cz2` is clocked with a clock number that is
     * higher by 1 modulo `num_clocks()` or if one of them is a synchronization element. Due to their extended Hold
     * phase, they feed information to any clock number.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether it accepts information from `cz1`.
     * @return `true` iff `cz2`'s clock number is higher by 1 modulo `num_clocks()` or if `cz1` or `cz2` are
     * synchronization elements.
     */
    [[nodiscard]] bool is_outgoing_clocked(const clock_zone<ClockedLayout>& cz1,
                                           const clock_zone<ClockedLayout>& cz2) const noexcept
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
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Returns a container with
     * all clock zones that are incoming to the given one.
     *
     * @param cz Base clock zone.
     * @return A container with all clock zones that are incoming to `cz`.
     */
    [[nodiscard]] auto incoming_clocked_zones(const clock_zone<ClockedLayout>& cz) const noexcept
    {
        std::vector<clock_zone<ClockedLayout>> incoming{};
        // don't reserve memory; not worth copying the clocking scheme from `ClockedLayout` to make an estimation

        foreach_incoming_clocked_zone(cz, [&incoming](const auto& ct) { incoming.push_back(ct); });

        return incoming;
    }
    /**
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Applies a function to all
     * incoming clock zones of a given one in accordance with `incoming_clocked_zones`.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by `mockturtle::foreach_element`.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of `cz`'s incoming clock zones.
     */
    template <typename Fn>
    void foreach_incoming_clocked_zone(const clock_zone<ClockedLayout>& cz, Fn&& fn) const
    {
        ClockedLayout::foreach_adjacent_coordinate(cz,
                                                   [this, &cz, &fn](const auto& ct)
                                                   {
                                                       if (is_incoming_clocked(cz, ct))
                                                       {
                                                           std::invoke(std::forward<Fn>(fn), ct);
                                                       }
                                                   });
    }
    /**
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Returns a container with
     * all clock zones that are outgoing from the given one.
     *
     * @param cz Base clock zone.
     * @return A container with all clock zones that are outgoing from `cz`.
     */
    [[nodiscard]] auto outgoing_clocked_zones(const clock_zone<ClockedLayout>& cz) const noexcept
    {
        std::vector<clock_zone<ClockedLayout>> outgoing{};
        // don't reserve memory; not worth copying the clocking scheme from `ClockedLayout` to make an estimation

        foreach_outgoing_clocked_zone(cz, [&outgoing](const auto& ct) { outgoing.push_back(ct); });

        return outgoing;
    }
    /**
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Applies a function to all
     * outgoing clock zones of a given one in accordance with `outgoing_clocked_zones`.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by `mockturtle::foreach_element`.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of `cz`'s outgoing clock zones.
     */
    template <typename Fn>
    void foreach_outgoing_clocked_zone(const clock_zone<ClockedLayout>& cz, Fn&& fn) const
    {
        ClockedLayout::foreach_adjacent_coordinate(cz,
                                                   [this, &cz, &fn](const auto& ct)
                                                   {
                                                       if (is_outgoing_clocked(cz, ct))
                                                       {
                                                           std::invoke(std::forward<Fn>(fn), ct);
                                                       }
                                                   });
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
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Returns the number of
     * incoming clock zones to the given one.
     *
     * @param cz Base clock zone.
     * @return Number of `cz`'s incoming clock zones.
     */
    [[nodiscard]] typename ClockedLayout::degree_t in_degree(const clock_zone<ClockedLayout>& cz) const noexcept
    {
        typename ClockedLayout::degree_t idg{0};
        foreach_incoming_clocked_zone(cz, [&idg](const auto&) { ++idg; });

        return idg;
    }
    /**
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Returns the number of
     * outgoing clock zones from the given one.
     *
     * @param cz Base clock zone.
     * @return Number of `cz`'s outgoing clock zones.
     */
    [[nodiscard]] typename ClockedLayout::degree_t out_degree(const clock_zone<ClockedLayout>& cz) const noexcept
    {
        typename ClockedLayout::degree_t odg{0};
        foreach_outgoing_clocked_zone(cz, [&odg](const auto&) { ++odg; });

        return odg;
    }
    /**
     * Overwrites the function from `ClockedLayout` to account for synchronization elements. Returns the number of
     * incoming plus outgoing clock zones of the given one.
     *
     * @param cz Base clock zone.
     * @return Number of `cz`'s incoming plus outgoing clock zones.
     */
    [[nodiscard]] typename ClockedLayout::degree_t degree(const clock_zone<ClockedLayout>& cz) const noexcept
    {
        const auto clock_degree = static_cast<typename ClockedLayout::degree_t>(ClockedLayout::degree(cz));
        const auto se_degree    = static_cast<typename ClockedLayout::degree_t>(in_degree(cz) + out_degree(cz));
        return se_degree > clock_degree ? clock_degree : se_degree;
    }

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_SYNCHRONIZATION_ELEMENT_LAYOUT_HPP
