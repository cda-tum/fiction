//
// Created by marcel on 30.08.21.
//

#ifndef FICTION_SYNCHRONIZATION_ELEMENT_LAYOUT_HPP
#define FICTION_SYNCHRONIZATION_ELEMENT_LAYOUT_HPP

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <set>
#include <unordered_map>

namespace fiction
{

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

    explicit synchronization_element_layout(const typename ClockedLayout::aspect_ratio& ar = {}) :
            ClockedLayout(ar),
            strg{std::make_shared<synchronization_element_layout_storage>()}
    {}

    synchronization_element_layout(const typename ClockedLayout::aspect_ratio&      ar,
                                   const typename ClockedLayout::clocking_scheme_t& scheme) :
            ClockedLayout(ar, scheme),
            strg{std::make_shared<synchronization_element_layout_storage>()}
    {}

    explicit synchronization_element_layout(std::shared_ptr<synchronization_element_layout_storage> s) :
            strg{std::move(s)}
    {}

#pragma endregion

#pragma region Clocking

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

    [[nodiscard]] sync_elem_t is_synchronization_element(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        if (auto it = strg->se_map.find(static_cast<uint64_t>(cz)); it != strg->se_map.end())
        {
            return it->second != sync_elem_t{0};
        }

        return false;
    }

    [[nodiscard]] sync_elem_t get_synchronization_element(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        if (auto it = strg->se_map.find(static_cast<uint64_t>(cz)); it != strg->se_map.end())
        {
            return it->second;
        }

        return 0;
    }

    [[nodiscard]] bool is_incoming_clocked(const typename ClockedLayout::clock_zone& cz1,
                                           const typename ClockedLayout::clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
            return false;

        return ClockedLayout::is_incoming_clocked(cz1, cz2) || is_synchronization_element(cz1) ||
               is_synchronization_element(cz2);
    }

    [[nodiscard]] bool is_outgoing_clocked(const typename ClockedLayout::clock_zone& cz1,
                                           const typename ClockedLayout::clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
            return false;

        return ClockedLayout::is_outgoing_clocked(cz1, cz2) || is_synchronization_element(cz1) ||
               is_synchronization_element(cz2);
    }

#pragma endregion

#pragma region Iteration

    template <typename Container>
    [[nodiscard]] Container incoming_clocked_zones(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        const auto adj = ClockedLayout::template adjacent_coordinates<Container>(cz);

        Container incoming{};

        std::copy_if(std::cbegin(adj), std::cend(adj), std::inserter(incoming, std::cend(incoming)),
                     [this, &cz](const auto& ct) { return is_incoming_clocked(cz, ct); });

        return incoming;
    }

    template <typename Container>
    [[nodiscard]] Container outgoing_clocked_zones(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        const auto adj = ClockedLayout::template adjacent_coordinates<Container>(cz);

        Container outgoing{};

        std::copy_if(std::cbegin(adj), std::cend(adj), std::inserter(outgoing, std::cend(outgoing)),
                     [this, &cz](const auto& ct) { return is_outgoing_clocked(cz, ct); });

        return outgoing;
    }

#pragma endregion

#pragma region Structural properties

    [[nodiscard]] uint32_t num_se() const noexcept
    {
        return static_cast<uint32_t>(strg->se_map.size());
    }

    [[nodiscard]] typename ClockedLayout::degree_t
    in_degree(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        return static_cast<typename ClockedLayout::degree_t>(
            incoming_clocked_zones<std::set<typename ClockedLayout::clock_zone>>(cz).size());
    }

    [[nodiscard]] typename ClockedLayout::degree_t
    out_degree(const typename ClockedLayout::clock_zone& cz) const noexcept
    {
        return static_cast<typename ClockedLayout::degree_t>(
            outgoing_clocked_zones<std::set<typename ClockedLayout::clock_zone>>(cz).size());
    }

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
