//
// Created by Jan Drewniok on 17.01.23.
//

#ifndef FICTION_ENERGY_DISTRIBUTION_HPP
#define FICTION_ENERGY_DISTRIBUTION_HPP

#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/constants.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This struct stores the energy state of an SiDB layout. The energy state consists of the electrostatic potential
 * energy and the degeneracy of the state.
 */
struct energy_state
{
    /**
     * Default constructor.
     *
     * @param electrostatic_potential_energy The electrostatic potential energy of the charge distribution (eV).
     * @param degeneracy The degeneracy of the state.
     */
    energy_state(const double electrostatic_potential_energy, const uint64_t degeneracy) :
            electrostatic_potential_energy{electrostatic_potential_energy},
            degeneracy{degeneracy}
    {}
    /**
     * The electrostatic potential energy of the charge distribution (eV).
     */
    double electrostatic_potential_energy;
    /**
     * The degeneracy of the state.
     */
    uint64_t degeneracy;
};

/**
 * This class is used to store the energy distribution of an SiDB layout. The energy distribution is a map that contains
 * the electrostatic potential as a key and its degeneracy as a value. To be more precise, if two different charge
 * distributions occur with the same energy, the degeneracy value of the energy state is 2.
 */
class energy_distribution
{
  public:
    /**
     * Default constructor.
     */
    energy_distribution() = default;
    /**
     * Returns the nth state (energy + degeneracy) in the energy distribution.
     *
     * @param state_index The index of the state to be retrieved.
     * @return The energy and degeneracy of the state at the specified index. If the index is out of range,
     * `std::nullopt` is returned instead.
     */
    [[nodiscard]] std::optional<energy_state> get_nth_state(const uint64_t state_index) const noexcept
    {
        if (state_index < distribution.size())
        {
            const auto it =
                std::next(distribution.cbegin(),
                          static_cast<typename std::iterator_traits<decltype(distribution.cbegin())>::difference_type>(
                              state_index));
            return energy_state{it->first, it->second};
        }

        return std::nullopt;
    }
    /**
     * Returns the degeneracy value (number of states) with the given energy value.
     *
     * @param energy The energy value for which the excited state number is to be determined.
     * @return The excited state number of the given energy value. If the energy value is not found, `std::nullopt` is
     * returned instead.
     */
    [[nodiscard]] std::optional<uint64_t> degeneracy_of_given_energy(const double energy) const noexcept
    {
        uint64_t degeneracy = 0;
        for (const auto& [key, value] : distribution)
        {
            if (std::abs(key - energy) < constants::ERROR_MARGIN)
            {
                return degeneracy;
            }
            ++degeneracy;
        }
        return std::nullopt;
    }
    /**
     * Adds a state to the energy distribution.
     *
     * @param energy The energy of the state to be added.
     * @param degeneracy The degeneracy of the state to be added.
     */
    void add_state(const double energy, const uint64_t degeneracy) noexcept
    {
        distribution[energy] = degeneracy;
    }
    /**
     * Returns the number of states in the energy distribution.
     *
     * @return The number of states in the energy distribution.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
        return distribution.size();
    }
    /**
     * Checks if the energy distribution is empty.
     *
     * @return `true` if the energy distribution is empty, `false` otherwise.
     */
    [[nodiscard]] bool empty() const noexcept
    {
        return distribution.empty();
    }
    /**
     * Returns the maximum energy value in the energy distribution.
     *
     * @return The maximum energy value in the energy distribution.
     */
    [[nodiscard]] double max_energy() const noexcept
    {
        return distribution.crbegin()->first;
    }
    /**
     * Returns the minimum energy value in the energy distribution.
     *
     * @return The minimum energy value in the energy distribution.
     */
    [[nodiscard]] double min_energy() const noexcept
    {
        return distribution.cbegin()->first;
    }
    /**
     * Applies a function to all states in the energy distribution.
     *
     * @tparam Fn Functor type.
     * @param fn Functor to apply to each key-value pair.
     */
    template <typename Fn>
    void for_each(Fn&& fn) const
    {
        for (const auto& [energy, degeneracy] : distribution)
        {
            std::invoke(std::forward<Fn>(fn), energy, degeneracy);
        }
    }

  private:
    /**
     * The energy distribution map. The key is the energy value and the value is the degeneracy of the energy value.
     */
    std::map<double, uint64_t> distribution;
};
/**
 * This function takes in a vector of `charge_distribution_surface` objects and returns a map containing the system
 * energy and the number of occurrences of that energy in the input vector. To compare two energy values for equality,
 * the comparison uses a tolerance specified by `constants::ERROR_MARGIN`.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param charge_distributions A vector of `charge_distribution_surface` objects for which statistics are to be
 * computed.
 * @return A map containing the system energy as the key and the number of occurrences of that energy in the input
 * vector as the value.
 */
template <typename Lyt>
[[nodiscard]] energy_distribution
calculate_energy_distribution(const std::vector<charge_distribution_surface<Lyt>>& charge_distributions)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    energy_distribution distribution{};

    std::vector<charge_distribution_surface<Lyt>> cds_updated_charge_index{};
    cds_updated_charge_index.reserve(charge_distributions.size());

    // collect all unique charge indices
    std::set<uint64_t> unique_charge_index{};

    for (auto& lyt : charge_distributions)
    {
        lyt.charge_distribution_to_index_general();
        unique_charge_index.insert(lyt.get_charge_index_and_base().first);
        cds_updated_charge_index.push_back(lyt);
    }

    std::vector<charge_distribution_surface<Lyt>> unique_cds{};
    unique_cds.reserve(unique_charge_index.size());

    std::set<double> unique_energies{};

    for (const auto& charge_index : unique_charge_index)
    {
        for (const auto& lyt : cds_updated_charge_index)
        {
            if (lyt.get_charge_index_and_base().first == charge_index)
            {
                // Check if the energy is already in the unique set with tolerance
                bool       is_unique = true;
                const auto energy    = lyt.get_electrostatic_potential_energy();
                for (const auto& unique_energy : unique_energies)
                {
                    if (std::fabs(energy - unique_energy) < constants::ERROR_MARGIN)
                    {
                        is_unique = false;
                        break;
                    }
                }

                // Add the layout and energy if it's unique
                if (is_unique)
                {
                    unique_energies.insert(energy);
                }

                unique_cds.push_back(lyt);

                break;
            }
        }
    }

    for (const auto& energy : unique_energies)
    {
        const auto number_of_states_with_given_energy = static_cast<uint64_t>(std::count_if(
            unique_cds.cbegin(), unique_cds.cend(), [&energy](const auto& lyt)
            { return std::abs(lyt.get_electrostatic_potential_energy() - energy) < constants::ERROR_MARGIN; }));

        distribution.add_state(energy, number_of_states_with_given_energy);
    }

    return distribution;
}

}  // namespace fiction

#endif  // FICTION_ENERGY_DISTRIBUTION_HPP
