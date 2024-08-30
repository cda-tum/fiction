//
// Created by Jan Drewniok on 23.11.22.
//

#ifndef FICTION_CHARGE_DISTRIBUTION_SURFACE_HPP
#define FICTION_CHARGE_DISTRIBUTION_SURFACE_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_nm_distance.hpp"
#include "fiction/technology/sidb_nm_position.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <optional>
#include <random>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * An enumeration of modes for the dependent cell.
 */
enum class dependent_cell_mode
{
    /**
     * The charge state of the dependent cell is not changed based on the local electrostatic potential at its position.
     */
    FIXED,
    /**
     * The charge state of the dependent cell is changed based on the local electrostatic potential at its position.
     */
    VARIABLE
};

/**
 * An enumeration of modes for calculation of the electrostatic potential energy of a given charge distribution.
 */
enum class energy_calculation
{
    /**
     * The electrostatic potential energy of a given charge distribution is not updated after it is changed.
     */
    KEEP_OLD_ENERGY_VALUE,
    /**
     * The electrostatic potential energy of a given charge distribution is updated after it is changed.
     */
    UPDATE_ENERGY
};

/**
 * An enumeration of modes for the charge distribution surface.
 */
enum class cds_configuration
{
    /**
     * The charge distribution is exclusively used to store the charge states.
     */
    ONLY_CHARGE_LOCATION,
    /**
     * In addition to the location of the charge states, the electrostatic computation is conducted and stored.
     */
    CHARGE_LOCATION_AND_ELECTROSTATIC
};

/**
 * An enumeration of modes to decide if the previous charge distribution is used to simply the computation of the
 * properties of a new charge distribution.
 */
enum class charge_distribution_history
{
    /**
     * The previous charge distribution is used.
     */
    CONSIDER,
    /**
     * The previous charge distribution is not used. Hence, the local electrostatic potential of the given charge
     * distribution is calculated from scratch.
     */
    NEGLECT
};

/**
 * An enumeration of modes to specifying if the charge index should be recomputed fully.
 */
enum class charge_index_recomputation
{
    /**
     * The charge index is recomputed from scratch.
     */
    FROM_SCRATCH,
    /**
     * The charge index is recomputed with the leading zeroes ignored. This optimization can be applied if we know that
     * the charge index was incremented after the last charge index computation.
     */
    IGNORE_LEADING_ZEROES
};

/**
 * An enumeration of modes for handling the charge distribution when assigning a charge index to the charge distribution
 * surface.
 */
enum class charge_distribution_mode
{
    /**
     * The charge distribution is updated after the charge index is assigned to the charge distribution surface.
     */
    UPDATE_CHARGE_DISTRIBUTION,
    /**
     * The charge distribution is kept and is not updated after a charge index is assigned to the charge distribution
     * surface.
     */
    KEEP_CHARGE_DISTRIBUTION
};

/**
 * An enumeration of modes for handling the charge index during charge state assignment.
 */
enum class charge_index_mode
{
    /**
     * The charge state is assigned to the cell and the charge index is updated.
     */
    UPDATE_CHARGE_INDEX,
    /**
     * The charge state is assigned to the cell but the old charge index is kept.
     */
    KEEP_CHARGE_INDEX
};

/**
 * A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
 * SiDBs' charge states.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam has_sidb_charge_distribution Automatically determines whether a charge distribution interface is already
 * present.
 */
template <typename Lyt, bool has_charge_distribution_interface =
                            std::conjunction_v<has_assign_charge_state<Lyt>, has_get_charge_state<Lyt>>>
class charge_distribution_surface : public Lyt
{};

template <typename Lyt>
class charge_distribution_surface<Lyt, true> : public Lyt
{
  public:
    explicit charge_distribution_surface(const Lyt& lyt) : Lyt(lyt) {}
};

template <typename Lyt>
class charge_distribution_surface<Lyt, false> : public Lyt
{
  public:
    using charge_index_base = typename std::pair<uint64_t, uint8_t>;

    struct charge_distribution_storage
    {
      private:
        /**
         * The distance matrix is a vector of vectors storing the euclidean distance in nm.
         */
        using distance_matrix = std::vector<std::vector<double>>;
        /**
         * The potential matrix is a vector of vectors storing the charge-less electrostatic potentials in Volt (V).
         */
        using potential_matrix = std::vector<std::vector<double>>;
        /**
         * It is a vector that stores the local electrostatic potential in Volt (V).
         */
        using local_potential = std::vector<double>;

      public:
        /**
         * Standard constructor for the charge_distribution_storage.
         *
         * @param params Physical parameters used for the simulation (µ_minus, base number, ...).
         */
        explicit charge_distribution_storage(const sidb_simulation_parameters& params = sidb_simulation_parameters{}) :
                simulation_parameters{params}
        {}
        /**
         * Stores all physical parameters used for the simulation.
         */
        sidb_simulation_parameters simulation_parameters{};
        /**
         * All cells that are occupied by an SiDB are stored in order.
         */
        std::vector<typename Lyt::cell> sidb_order{};
        /**
         * All cells that cannot be positively charged in a physically valid layout.
         */
        std::vector<typename Lyt::cell> sidb_order_without_three_state_cells{};
        /**
         * The SiDBs' charge states are stored. Corresponding cells are stored in `sidb_order`.
         */
        std::vector<sidb_charge_state> cell_charge{};
        /**
         * Distance between SiDBs are stored as matrix (unit: nm).
         */
        distance_matrix nm_dist_mat{};
        /**
         * Electrostatic potential between SiDBs are stored as matrix (here, still charge-independent, unit: V).
         */
        potential_matrix pot_mat{};
        /**
         * Electrostatic potential at each SiDB position which is generated by defects (unit: eV).
         */
        std::unordered_map<typename Lyt::cell, double> defect_local_pot{};
        /**
         * External electrostatic potential in V at each SiDB position (can be used when different potentials are
         * applied to different SiDBs).
         */
        std::unordered_map<typename Lyt::cell, double> local_external_pot{};
        /**
         * Electrostatic potential at each SiDB position. Has to be updated when charge distribution is changed (unit:
         * V).
         */
        local_potential local_pot{};
        /**
         * Stores the electrostatic energy of a given charge distribution (unit: eV).
         */
        double system_energy{0.0};
        /**
         * Label if given charge distribution is physically valid (see https://ieeexplore.ieee.org/document/8963859).
         */
        bool validity = false;
        /**
         * Each charge distribution is assigned a unique index (first entry of pair), second one stores the base number
         * (2- or 3-state simulation).
         */
        charge_index_base charge_index_and_base{};
        /**
         * Charge index of the sublayout (collection of SiDBs that could be positively charged for a specific charge
         * configuration of the layout).
         */
        uint64_t charge_index_sublayout{};
        /**
         * Depending on the number of SiDBs and the base number, a maximal number of possible charge distributions
         * exists.
         */
        uint64_t max_charge_index{};
        /**
         * Depending on the number of SiDBs in the SiDBs, a maximal number of possible charge distributions
         * exists.
         */
        uint64_t max_charge_index_sulayout{};
        /**
         * This pair stores the cell index and its previously charge state (important when all possible charge
         * distributions are enumerated and checked for physical validity).
         */
        std::pair<int64_t, int8_t> cell_history_gray_code{};
        /**
         * This vector stores the cells and its previously charge states of the charge distribution before the charge
         * index was changed.
         */
        std::vector<std::pair<uint64_t, int8_t>> cell_history{};
        /**
         * This unordered map stores the cells and the placed defect.
         */
        std::unordered_map<typename Lyt::cell, const sidb_defect> defects{};
        /**
         * Dependent cell is the cell which charge state is determined by all other SiDBs in the layout.
         */
        typename Lyt::cell dependent_cell{};
        /**
         * Charge index of the dependent cell in the layout.
         */
        uint64_t dependent_cell_index{};
        /**
         * This vector collects all cells that could potentially be positively charged based on the maximum possible
         * local potential.
         */
        std::vector<typename Lyt::cell> three_state_cells{};
        /**
         * True indicates that the dependent SiDB is in the sublayout.
         */
        bool dependent_cell_in_sub_layout{};
    };

    using storage = std::shared_ptr<charge_distribution_storage>;

    /**
     * Standard constructor for empty layouts.
     *
     * @param params Physical parameters used for the simulation (µ_minus, base number, ...).
     * @param cs The charge state used for the initialization of all SiDBs, default is a negative charge.
     */
    explicit charge_distribution_surface(const sidb_simulation_parameters& params = sidb_simulation_parameters{},
                                         const sidb_charge_state           cs     = sidb_charge_state::NEGATIVE) :
            Lyt(),
            strg{std::make_shared<charge_distribution_storage>(params)}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

        initialize(cs);
    }

    /**
     * Standard constructor for existing layouts.
     *
     * @param lyt SiDB cell-level layout.
     * @param params Physical parameters used for the simulation (µ_minus, base number, ...).
     * @param cs The charge state used for the initialization of all SiDBs, default is a negative charge.
     * @param configuration Specifies the configuration for charge distribution settings.
     *                      Determines whether only charge locations are considered or if
     *                      both charge locations and electrostatic interactions are included.
     */
    explicit charge_distribution_surface(
        const Lyt& lyt, const sidb_simulation_parameters& params = sidb_simulation_parameters{},
        const sidb_charge_state cs            = sidb_charge_state::NEGATIVE,
        const cds_configuration configuration = cds_configuration::CHARGE_LOCATION_AND_ELECTROSTATIC) :
            Lyt(lyt),
            strg{std::make_shared<charge_distribution_storage>(params)}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

        initialize(cs, configuration);
    };

    /**
     * Copy constructor.
     *
     * @param cds Other `charge_distribution_surface`.
     */
    charge_distribution_surface(const charge_distribution_surface<Lyt>& cds) :
            Lyt(cds),
            strg{std::make_shared<charge_distribution_storage>(*cds.strg)}
    {}
    /**
     * Copy assignment operator.
     *
     * @param other charge_distribution_surface.
     */
    charge_distribution_surface& operator=(const charge_distribution_surface& other)
    {
        if (this != &other)
        {
            strg = std::make_shared<charge_distribution_storage>(*other.strg);
        }

        return *this;
    }
    /**
     * This function returns the locations of all SiDBs in nm of the form `(x,y)`.
     *
     * @return Vector of SiDB nanometer positions (unit: nm).
     */
    [[nodiscard]] std::vector<std::pair<double, double>> get_all_sidb_locations_in_nm() const noexcept
    {
        std::vector<std::pair<double, double>> positions{};
        positions.reserve(strg->sidb_order.size());

        for (const auto& c : strg->sidb_order)
        {
            auto pos = sidb_nm_position<Lyt>(Lyt{}, c);
            positions.push_back(std::make_pair(pos.first, pos.second));
        }

        return positions;
    }
    /**
     * This function assigns a cell type to a given cell of the underlying cell-level layout.
     *
     * @param c Cell whose type is changed.
     * @param ct Cell type which is assigned to the given cell.
     */
    void assign_cell_type(const typename Lyt::cell& c, const typename Lyt::cell_type& ct) noexcept
    {
        Lyt::assign_cell_type(c, ct);
        initialize(sidb_charge_state::NEGATIVE);
    }
    /**
     * This function assigns the physical parameters for the simulation.
     *
     * @param params Physical parameters to be assigned.
     */
    void assign_physical_parameters(const sidb_simulation_parameters& params) noexcept
    {
        strg->simulation_parameters        = params;
        strg->charge_index_and_base.second = params.base;
        strg->max_charge_index =
            static_cast<uint64_t>(std::pow(strg->simulation_parameters.base, this->num_cells())) - 1;
        this->initialize_potential_matrix();
        this->update_local_potential();
        this->recompute_system_energy();
        this->validity_check();
    }
    /**
     * This function retrieves the physical parameters of the simulation.
     *
     * @return sidb_simulation_parameters struct containing the physical parameters of the simulation.
     */
    [[nodiscard]] sidb_simulation_parameters get_simulation_params() const noexcept
    {
        return strg->simulation_parameters;
    }

    /**
     * This function checks if any SiDB exhibits the given charge state.
     *
     * @param cs Charge state.
     */
    [[nodiscard]] bool charge_exists(const sidb_charge_state cs) const noexcept
    {
        return std::any_of(strg->cell_charge.cbegin(), strg->cell_charge.cend(),
                           [&cs](const sidb_charge_state c) { return c == cs; });
    }
    /**
     * This function searches the index of an SiDB.
     *
     * @param c The cell to find the index of.
     * @return The index of the cell in the layout. Returns -1 if the cell is not part of the layout.
     */
    [[nodiscard]] int64_t cell_to_index(const typename Lyt::cell& c) const noexcept
    {
        if (const auto it = std::find(strg->sidb_order.cbegin(), strg->sidb_order.cend(), c);
            it != strg->sidb_order.cend())
        {
            return static_cast<int64_t>(std::distance(strg->sidb_order.cbegin(), it));
        }

        return -1;
    }
    /**
     * This function assigns the given charge state to the given cell of the layout.
     *
     * @param c The cell to which a charge state is to be assigned.
     * @param cs The charge state to be assigned to the cell.
     * @param index_mode Mode to determine whether the charge index should be updated.
     */
    void assign_charge_state(const typename Lyt::cell& c, const sidb_charge_state cs,
                             const charge_index_mode index_mode = charge_index_mode::UPDATE_CHARGE_INDEX) noexcept
    {
        if (auto index = cell_to_index(c); index != -1)
        {
            strg->cell_charge[static_cast<uint64_t>(index)] = cs;
        }
        if (index_mode == charge_index_mode::UPDATE_CHARGE_INDEX)
        {
            this->charge_distribution_to_index();
        }
    }
    /**
     * This function assigns the given charge state to the cell of the layout at the specified index. It updates the
     * `cell_charge` member of `strg` object with the new charge state of the specified cell.
     *
     * @param index The index of the cell.
     * @param cs The charge state to be assign to the cell.
     */
    void assign_charge_by_cell_index(const uint64_t index, const sidb_charge_state cs) noexcept
    {
        strg->cell_charge[index] = cs;
        this->charge_distribution_to_index();
    }
    /**
     * This function assigns the charge state of all SiDBs in the layout to a given charge state.
     *
     * @param cs The charge state to be assigned to all the SiDBs.
     * @param index_mode Mode to determine whether the charge index should be updated.
     */
    void assign_all_charge_states(const sidb_charge_state cs,
                                  const charge_index_mode index_mode = charge_index_mode::UPDATE_CHARGE_INDEX) noexcept
    {
        for (uint64_t i = 0u; i < strg->cell_charge.size(); ++i)
        {
            strg->cell_charge[i] = cs;
        }
        if (index_mode == charge_index_mode::UPDATE_CHARGE_INDEX)
        {
            this->charge_distribution_to_index();
        }
        {
            this->charge_distribution_to_index();
        }
    }
    /**
     * This function assigns the dependent cell (i.e., cell which charge state is set based on the neighbor cells
     * and the population stability).
     *
     * @param c cell which is set as the dependent cell.
     *
     * @note c has to be part of the initialized charge distribution surface layout.
     */
    void assign_dependent_cell(const typename Lyt::cell& c) noexcept
    {
        assert(cell_to_index(c) != -1 && "dependent cell is not part of the layout");
        strg->dependent_cell   = c;
        strg->max_charge_index = static_cast<uint64_t>(
            std::pow(static_cast<double>(strg->simulation_parameters.base), this->num_cells() - 1) - 1);
        strg->dependent_cell_index = static_cast<uint64_t>(cell_to_index(strg->dependent_cell));
    }
    /**
     * This function assigns the base number for the simulation.
     *
     * @param base Base number to be assigned.
     */
    void assign_base_number(const uint8_t base) noexcept
    {
        strg->simulation_parameters.base   = base;
        strg->charge_index_and_base.second = base;
        if (!strg->dependent_cell.is_dead())
        {
            strg->max_charge_index =
                static_cast<uint64_t>(std::pow(static_cast<double>(base), this->num_cells() - 1) - 1);
        }
        else
        {
            strg->max_charge_index = static_cast<uint64_t>(std::pow(static_cast<double>(base), this->num_cells()) - 1);
        }
    }
    /**
     * This function adds a defect to the layout.
     *
     * @param c The cell to which a defect is added.
     * @param defect Defect which is added to the layout.
     */
    void add_sidb_defect_to_potential_landscape(const typename Lyt::cell& c, const sidb_defect& defect) noexcept
    {
        // check if defect is not placed on SiDB position
        if (std::find(strg->sidb_order.cbegin(), strg->sidb_order.cend(), c) == strg->sidb_order.end())
        {
            // check if defect was not added yet.
            if (strg->defects.find(c) == strg->defects.end())
            {
                strg->defects.insert({c, defect});
                this->foreach_cell(
                    [this, &c, &defect](const auto& c1)
                    {
                        const auto dist = sidb_nm_distance<Lyt>(*this, c1, c);
                        const auto pot  = chargeless_potential_generated_by_defect_at_given_distance(dist, defect);

                        strg->defect_local_pot[c1] += pot * static_cast<double>(defect.charge);
                    });

                this->update_after_charge_change(dependent_cell_mode::FIXED);
            }
            else
            {
                Lyt::foreach_cell(
                    [this, &c, &defect](const auto& c1)
                    {
                        const auto dist = sidb_nm_distance<Lyt>(*this, c1, c);

                        strg->defect_local_pot[c1] =
                            strg->defect_local_pot[c1] +
                            chargeless_potential_generated_by_defect_at_given_distance(dist, defect) *
                                static_cast<double>(defect.charge) -
                            chargeless_potential_generated_by_defect_at_given_distance(dist, strg->defects[c]) *
                                static_cast<double>(strg->defects[c].charge);
                    });

                strg->defects.erase(c);
                strg->defects.insert({c, defect});

                this->update_after_charge_change(dependent_cell_mode::FIXED);
            }
        }
    }
    /**
     * This function erases a defect to the layout.
     *
     * @param c The cell where a defect is erased.
     */
    void erase_defect(const typename Lyt::cell& c) noexcept
    {
        if (strg->defects.find(c) != strg->defects.cend())
        {
            this->foreach_cell(
                [this, &c](const auto& c1)
                {
                    strg->local_pot[static_cast<uint64_t>(cell_to_index(c1))] -=
                        chargeless_potential_generated_by_defect_at_given_distance(sidb_nm_distance<Lyt>(*this, c1, c),
                                                                                   strg->defects[c]) *
                        static_cast<double>(strg->defects[c].charge);
                    strg->defect_local_pot[c1] -= chargeless_potential_generated_by_defect_at_given_distance(
                                                      sidb_nm_distance<Lyt>(*this, c1, c), strg->defects[c]) *
                                                  static_cast<double>(strg->defects[c].charge);
                });
            strg->defects.erase(c);
        }
    }
    /**
     * This function assigns the given charge state to the cell (accessed by `index`) of the layout.
     *
     * @param index The index of the cell to which a charge state is to be assigned.
     * @param cs The charge state to be assigned to the cell.
     * @param index_mode Mode to determine whether the charge index should be updated.
     */
    void assign_charge_state_by_cell_index(
        const uint64_t index, const sidb_charge_state cs,
        const charge_index_mode index_mode = charge_index_mode::UPDATE_CHARGE_INDEX) noexcept
    {
        strg->cell_charge[index] = cs;

        if (index_mode == charge_index_mode::UPDATE_CHARGE_INDEX)
        {
            this->charge_distribution_to_index();
        }
    }
    /**
     * This function returns the charge state of a given cell.
     *
     * @param c The cell.
     * @return The charge state of the given cell.
     */
    [[nodiscard]] sidb_charge_state get_charge_state(const typename Lyt::cell& c) const noexcept
    {
        if (const auto index = cell_to_index(c); index != -1)
        {
            return strg->cell_charge[static_cast<uint64_t>(index)];
        }

        return sidb_charge_state::NONE;
    }
    /**
     * This function returns the charge state of a cell of the layout at a given index.
     *
     * @param index The index of the cell.
     * @return The charge state of the cell at the given index.
     */
    [[nodiscard]] sidb_charge_state get_charge_state_by_index(const uint64_t index) const noexcept
    {
        if (index < (strg->cell_charge.size()))
        {
            return strg->cell_charge[index];
        }

        return sidb_charge_state::NONE;
    }
    /**
     * This function returns all SiDB charges of the placed SiDBs as a vector.
     *
     * @return Vector of SiDB charge states.
     */
    [[maybe_unused]] [[nodiscard]] std::vector<sidb_charge_state> get_all_sidb_charges() const noexcept
    {
        return strg->cell_charge;
    }
    /**
     * This function can be used to detect which SiDBs must be negatively charged due to their location. Important:
     * This function must be applied to a charge layout where all SiDBs are negatively initialized.
     *
     * @return Vector of indices describing which SiDBs must be negatively charged.
     */
    [[nodiscard]] std::vector<int64_t> negative_sidb_detection() const noexcept
    {
        std::vector<int64_t> negative_sidbs{};
        negative_sidbs.reserve(this->num_cells());
        this->foreach_cell(
            [&negative_sidbs, this](const auto& c)
            {
                if (const auto local_pot = this->get_local_potential(c); local_pot.has_value())
                {
                    // Check if the maximum band bending is sufficient to shift (0/-) above the Fermi level. The local
                    // potential is converted from J to eV to compare the band bending with the Fermi level (which is
                    // also given in eV).
                    if ((-*local_pot + strg->simulation_parameters.mu_minus) < -physical_constants::POP_STABILITY_ERR)
                    {
                        negative_sidbs.push_back(cell_to_index(c));
                    }
                }
            });
        return negative_sidbs;
    }
    /**
     *  This function returns the distance between two cells in nanometer (unit: nm).
     *
     *  @param c1 the first cell to compare.
     *  @param c2 the second cell to compare.
     *  @return a constexpr double representing the distance in nm between the two cells.
     */
    [[nodiscard]] double get_nm_distance_between_sidbs(const typename Lyt::cell& c1,
                                                       const typename Lyt::cell& c2) const noexcept
    {
        if (const auto index1 = cell_to_index(c1), index2 = cell_to_index(c2); (index1 != -1) && (index2 != -1))
        {
            return strg->nm_dist_mat[static_cast<uint64_t>(index1)][static_cast<uint64_t>(index2)];
        }

        return 0.0;
    }
    /**
     * This function calculates and returns the distance between two cells in nanometer (accessed by indices) (unit:
     * nm).
     *
     * @param index1 The first index.
     * @param index2 The second index.
     * @return The distance in nanometer between `index1` and `index2` (indices correspond to unique SiDBs) (unit: nm).
     */
    [[nodiscard]] double get_nm_distance_by_indices(const uint64_t index1, const uint64_t index2) const noexcept
    {
        return strg->nm_dist_mat[index1][index2];
    }
    /**
     * This function calculates and returns the chargeless electrostatic potential between two cells (SiDBs) in Volt
     * (unit: V).
     *
     * @param index1 The first index.
     * @param index1 The second index.
     * @return The chargeless electrostatic potential between `index1` and `index2` (unit: V).
     */
    [[nodiscard]] double calculate_chargeless_potential_between_sidbs_by_index(const uint64_t index1,
                                                                               const uint64_t index2) const noexcept
    {
        assert(strg->simulation_parameters.lambda_tf > 0.0 && "lambda_tf has to be > 0.0");

        if (strg->nm_dist_mat[index1][index2] == 0.0)
        {
            return 0.0;
        }

        return (strg->simulation_parameters.k() / (strg->nm_dist_mat[index1][index2] * 1E-9) *
                std::exp(-strg->nm_dist_mat[index1][index2] / strg->simulation_parameters.lambda_tf) *
                physical_constants::ELEMENTARY_CHARGE);
    }
    /**
     * This function calculates and returns the chargeless potential in Volt of a pair of cells based on their distance
     * and simulation parameters (unit: V).
     *
     * @param c1 The first cell.
     * @param c2 The second cell.
     * @return The potential between c1 and c2 (unit: V).
     */
    [[nodiscard]] double calculate_chargeless_potential_between_sidbs(const typename Lyt::cell& c1,
                                                                      const typename Lyt::cell& c2) const noexcept
    {
        const auto index1 = static_cast<std::size_t>(cell_to_index(c1));
        const auto index2 = static_cast<std::size_t>(cell_to_index(c2));

        return calculate_chargeless_potential_between_sidbs_by_index(index1, index2);
    }
    /**
     * This function returns the chargeless electrostatic potential between two cells in V (unit: V).
     *
     * @note If the signed electrostatic potential \f$V_{i,j}\f$ is required, use the `get_potential_between_sidbs`
     * function.
     *
     * @param c1 The first cell.
     * @param c2 The second cell.
     * @return The chargeless electrostatic potential between `c1` and `c2`, i.e, \f$\frac{V_{i,j}}{n_j}\f$ (unit: V).
     */
    [[nodiscard]] double get_chargeless_potential_between_sidbs(const typename Lyt::cell& c1,
                                                                const typename Lyt::cell& c2) const noexcept
    {
        if (const auto index1 = cell_to_index(c1), index2 = cell_to_index(c2); (index1 != -1) && (index2 != -1))
        {
            return strg->pot_mat[static_cast<uint64_t>(index1)][static_cast<uint64_t>(index2)];
        }

        return 0.0;
    }
    /**
     * This function calculates and returns the chargeless potential of two indices (representing two SiDBs) in Volt.
     *
     * @param index1 The first index.
     * @param index2 The second index.
     * @return The potential between `index1` and `index2`.
     */
    [[nodiscard]] double get_chargeless_potential_by_indices(const uint64_t index1,
                                                             const uint64_t index2) const noexcept
    {
        return strg->pot_mat[index1][index2];
    }
    /**
     * This function calculates and returns the electrostatic potential at one cell (`c1`) generated by another cell
     * (`c2`) in Volt (unit: V).
     *
     * @note If the chargeless electrostatic potential \f$\frac{V_{i,j}}{n_j}\f$ is required, use the
     * `get_chargeless_potential_between_sidbs` function.
     *
     * @param c1 The first cell.
     * @param c2 The second cell.
     * @return The electrostatic potential between `c1` and `c2`, i.e., \f$V_{i,j}\f$ (unit: V).
     */
    [[nodiscard]] double get_potential_between_sidbs(const typename Lyt::cell& c1,
                                                     const typename Lyt::cell& c2) const noexcept
    {
        if (const auto index1 = cell_to_index(c1), index2 = cell_to_index(c2); (index1 != -1) && (index2 != -1))
        {
            return strg->pot_mat[static_cast<uint64_t>(index1)][static_cast<uint64_t>(index2)] *
                   charge_state_to_sign(get_charge_state(c2));
        }

        return 0.0;
    }
    /**
     * This function calculates the local electrostatic potential in Volt for each SiDB position, including external
     * electrostatic potentials (generated by electrodes, defects, etc.) (unit: V).
     *
     * @param history_mode `charge_distribution_history::NEGLECT` if the information (local electrostatic energy) of the
     * previous charge distribution is used to make the update more efficient, `charge_distribution_history::CONSIDER`
     * otherwise.
     */
    void update_local_potential(
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT) noexcept
    {
        if (history_mode == charge_distribution_history::NEGLECT)
        {
            strg->local_pot.resize(this->num_cells(), 0);

            for (uint64_t i = 0u; i < strg->sidb_order.size(); ++i)
            {
                double collect = 0.0;
                for (uint64_t j = 0u; j < strg->sidb_order.size(); j++)
                {
                    collect += strg->pot_mat[i][j] * static_cast<double>(charge_state_to_sign(strg->cell_charge[j]));
                }

                strg->local_pot[i] = collect;
            }

            for (const auto& [c, defect_pot] : strg->defect_local_pot)
            {
                strg->local_pot[static_cast<uint64_t>(cell_to_index(c))] += defect_pot;
            }

            for (const auto& [c, external_pot] : strg->local_external_pot)
            {
                strg->local_pot[static_cast<uint64_t>(cell_to_index(c))] += external_pot;
            }
        }
        else
        {
            if (strg->simulation_parameters.base == 2)
            {
                if (strg->cell_history_gray_code.first != -1)
                {
                    const auto cell_charge = charge_state_to_sign(
                        strg->cell_charge[static_cast<uint64_t>(strg->cell_history_gray_code.first)]);
                    const auto charge_diff = static_cast<double>(cell_charge - strg->cell_history_gray_code.second);
                    for (uint64_t j = 0u; j < strg->sidb_order.size(); j++)
                    {
                        strg->local_pot[j] +=
                            strg->pot_mat[static_cast<uint64_t>(strg->cell_history_gray_code.first)][j] * charge_diff;
                    }
                }
            }
            else
            {
                for (const auto& [changed_cell, charge] : strg->cell_history)
                {
                    for (uint64_t j = 0u; j < strg->sidb_order.size(); j++)
                    {
                        strg->local_pot[j] +=
                            strg->pot_mat[changed_cell][j] *
                            (static_cast<double>(charge_state_to_sign(strg->cell_charge[changed_cell])) - charge);
                    }
                }
            }
        }
    }
    /**
     * The function returns the local electrostatic potential at a given SiDB position in V.
     *
     * @param c The cell defining the SiDB position.
     * @return Local potential at given cell position. If there is no SiDB at the given cell, `std::nullopt` is
     * returned (unit: V).
     */
    std::optional<double> get_local_potential(const typename Lyt::cell& c) const noexcept
    {
        if (const auto index = cell_to_index(c); index != -1)
        {
            return strg->local_pot[static_cast<uint64_t>(index)];
        }

        return std::nullopt;
    }
    /**
     * This function returns the local electrostatic potential at a given index position in Volt (unit: V).
     *
     * @param index The index defining the SiDB position.
     * @return local potential at given index position. If there is no SiDB at the given index (which corresponds to a
     * unique cell), `std::nullopt` is returned (unit: V).
     */
    [[nodiscard]] std::optional<double> get_local_potential_by_index(const uint64_t index) const noexcept
    {
        if (index < strg->sidb_order.size())
        {
            return strg->local_pot[index];
        }
        return std::nullopt;
    }
    /**
     * This function assign the electrostatic system energy to zero (unit: eV). It can be used if only one SiDB is
     * charged.
     */
    void assign_system_energy_to_zero() noexcept
    {
        strg->system_energy = 0.0;
    }
    /**
     * This function calculates the system's total electrostatic potential energy and stores it in the storage (unit:
     * eV).
     */
    void recompute_system_energy() noexcept
    {
        double total_potential = 0.0;

        for (uint64_t i = 0; i < strg->local_pot.size(); ++i)
        {
            total_potential +=
                0.5 * strg->local_pot[i] * static_cast<double>(charge_state_to_sign(strg->cell_charge[i]));
        }

        double defect_energy = 0;
        for (const auto& [c, pot] : strg->defect_local_pot)
        {
            defect_energy +=
                pot *
                static_cast<double>(charge_state_to_sign(strg->cell_charge[static_cast<uint64_t>(cell_to_index(c))]));
        }

        double defect_interaction = 0;
        for (const auto& [cell1, defect1] : strg->defects)
        {
            for (const auto& [cell2, defect2] : strg->defects)
            {
                defect_interaction +=
                    chargeless_potential_at_given_distance(sidb_nm_distance<Lyt>(*this, cell1, cell2));
            }
        }
        strg->system_energy = total_potential + 0.5 * defect_energy + 0.5 * defect_interaction;
    }
    /**
     * This function returns the currently stored system's total electrostatic potential energy in eV.
     *
     * @return The system's total electrostatic potential energy (unit: eV).
     */
    [[nodiscard]] double get_system_energy() const noexcept
    {
        return strg->system_energy;
    }
    /**
     * The function updates the local potential (unit: Volt) and the system energy (unit: eV) after a charge change.
     *
     * @param dep_cell `dependent_cell_mode::FIXED` if the state of the dependent cell should not change,
     * `dependent_cell_mode::VARIABLE` if it should.
     * @param energy_calculation_mode `energy_calculation::UPDATE_ENERGY` if the electrostatic potential energy should
     * be updated, `energy_calculation::KEEP_ENERGY` otherwise.
     * @param history_mode `charge_distribution_history::NEGLECT` if the information (local electrostatic energy) of the
     * previous charge distribution is used to make the update more efficient, `charge_distribution_history::CONSIDER`
     * otherwise.
     */
    void update_after_charge_change(
        const dependent_cell_mode         dep_cell                = dependent_cell_mode::FIXED,
        const energy_calculation          energy_calculation_mode = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history history_mode            = charge_distribution_history::NEGLECT) noexcept
    {
        this->update_local_potential(history_mode);
        if (dep_cell == dependent_cell_mode::VARIABLE)
        {
            this->update_charge_state_of_dependent_cell();
        }
        if (energy_calculation_mode == energy_calculation::UPDATE_ENERGY)
        {
            this->recompute_system_energy();
        }
        this->validity_check();
    }
    /**
     * The physically validity of the current charge distribution is evaluated and stored in the storage struct. A
     * charge distribution is valid if the *Population Stability* and the *Configuration Stability* is fulfilled.
     */
    void validity_check() noexcept
    {
        uint64_t   population_stability_not_fulfilled_counter = 0;
        uint64_t   for_loop_counter                           = 0;
        const auto mu_p                                       = strg->simulation_parameters.mu_plus();

        for (const auto& it : strg->local_pot)  // this for-loop checks if the "population stability" is fulfilled.
        {
            bool valid = (((strg->cell_charge[for_loop_counter] == sidb_charge_state::NEGATIVE) &&
                           (-it + strg->simulation_parameters.mu_minus < physical_constants::POP_STABILITY_ERR)) ||
                          ((strg->cell_charge[for_loop_counter] == sidb_charge_state::POSITIVE) &&
                           (-it + mu_p > -physical_constants::POP_STABILITY_ERR)) ||
                          ((strg->cell_charge[for_loop_counter] == sidb_charge_state::NEUTRAL) &&
                           (-it + strg->simulation_parameters.mu_minus > physical_constants::POP_STABILITY_ERR) &&
                           (-it + mu_p < physical_constants::POP_STABILITY_ERR)));
            for_loop_counter += 1;
            if (!valid)
            {
                strg->validity = false;  // if at least one SiDB does not fulfill the population stability, the validity
                                         // of the given charge distribution is set to "false".
                population_stability_not_fulfilled_counter += 1;
                break;
            }
        }

        if ((population_stability_not_fulfilled_counter == 0) &&
            (for_loop_counter >
             0))  // if population stability is fulfilled for all SiDBs, the "configuration stability" is checked.
        {
            const auto hop_del =
                [this](const uint64_t c1, const uint64_t c2)  // energy change when charge hops between two SiDBs.
            { return strg->local_pot[c1] - strg->local_pot[c2] - strg->pot_mat[c1][c2]; };

            uint64_t hop_counter = 0;
            for (uint64_t i = 0u; i < strg->local_pot.size(); ++i)
            {
                if (strg->cell_charge[i] == sidb_charge_state::POSITIVE)  // we do nothing with SiDB+
                {
                    continue;
                }

                for (uint64_t j = 0u; j < strg->local_pot.size(); j++)
                {
                    if (hop_counter == 1)
                    {
                        break;
                    }

                    if (const auto e_del = hop_del(i, j);
                        (charge_state_to_sign(strg->cell_charge[j]) > charge_state_to_sign(strg->cell_charge[i])) &&
                        (e_del < -physical_constants::POP_STABILITY_ERR))  // Checks if energetically favored
                                                                           // hops exist between two SiDBs.
                    {
                        hop_counter = 1;

                        break;
                    }
                }
            }

            // If there is no jump that leads to a decrease in the potential energy of the system, the given charge
            // distribution satisfies metastability.
            strg->validity = hop_counter == 0;
        }
    }
    /**
     * This function returns the currently stored validity of the present charge distribution layout.
     *
     * @returns The validity of the present charge distribution.
     */
    [[nodiscard]] bool is_physically_valid() const noexcept
    {
        return strg->validity;
    }

    /**
     * The charge distribution of the charge distribution surface is converted to a unique index. It is used to map
     * every possible charge distribution of an SiDB layout to a unique index.
     *
     * IMPORTANT: This function can be used whenever a charge distribution needs to be converted to a charge index.
     * However, this function is not optimized compared to charge_distribution_to_index.
     */
    void charge_distribution_to_index_general() const noexcept
    {
        const uint8_t base = strg->simulation_parameters.base;

        uint64_t chargeindex = 0;
        uint64_t counter     = 0;

        for (const auto& c : strg->sidb_order)
        {
            chargeindex +=
                static_cast<uint64_t>(charge_state_to_sign(strg->cell_charge[static_cast<uint64_t>(cell_to_index(c))]) +
                                      int8_t{1}) *
                static_cast<uint64_t>(std::pow(base, this->num_cells() - 1u - counter));
            counter += 1;
        }

        strg->charge_index_and_base = {chargeindex, base};
    }
    /**
     * The charge distribution of the charge distribution surface is converted to a unique index. It is used to map
     * every possible charge distribution of an SiDB layout to a unique index.
     */
    void charge_distribution_to_index() const noexcept
    {
        const uint8_t base = strg->simulation_parameters.base;

        uint64_t chargeindex = 0;
        uint64_t counter     = 0;

        uint64_t chargeindex_sub_layout = 0;
        uint64_t counter_sub_layout     = 0;

        if (!strg->dependent_cell.is_dead())
        {
            if (!strg->three_state_cells.empty())
            {
                for (const auto& c : strg->three_state_cells)
                {
                    chargeindex_sub_layout +=
                        static_cast<uint64_t>(
                            charge_state_to_sign(strg->cell_charge[static_cast<uint64_t>(cell_to_index(c))]) +
                            int8_t{1}) *
                        static_cast<uint64_t>(std::pow(3, strg->three_state_cells.size() - counter_sub_layout - 1));
                    counter_sub_layout += 1;
                }
                for (const auto& c : strg->sidb_order_without_three_state_cells)
                {
                    if (c != strg->dependent_cell)
                    {
                        chargeindex +=
                            static_cast<uint64_t>(
                                (charge_state_to_sign(strg->cell_charge[static_cast<uint64_t>(cell_to_index(c))]) +
                                 int8_t{1})) *
                            static_cast<uint64_t>(std::pow(2, this->num_cells() - 1 - counter - 1));
                        counter += 1;
                    }
                }
            }
            // there are no positively charged SiDBs
            else
            {
                for (uint64_t c = 0; c < strg->cell_charge.size(); c++)
                {
                    if (c != static_cast<uint64_t>(cell_to_index(strg->dependent_cell)))
                    {
                        chargeindex += static_cast<uint64_t>((charge_state_to_sign(strg->cell_charge[c]) + 1) *
                                                             std::pow(base, this->num_cells() - 1 - counter - 1));
                        counter += 1;
                    }
                }
            }
        }
        // there is no dependent-cell chosen
        else
        {
            // there are SiDBs that can be positively charged
            if (!strg->three_state_cells.empty())
            {
                // iterate through SiDBs that can be positively charged
                for (const auto& c : strg->three_state_cells)
                {

                    chargeindex_sub_layout += static_cast<uint64_t>(
                        (charge_state_to_sign(strg->cell_charge[static_cast<uint64_t>(cell_to_index(c))]) + 1) *
                        std::pow(3, strg->three_state_cells.size() - 1 - counter_sub_layout));
                    counter_sub_layout += 1;
                }
                // iterate through SiDBs that cannot be positively charged
                for (const auto& c : strg->sidb_order_without_three_state_cells)
                {
                    chargeindex += static_cast<uint64_t>(
                        (charge_state_to_sign(strg->cell_charge[static_cast<uint64_t>(cell_to_index(c))]) + 1) *
                        std::pow(2, this->num_cells() - 1 - counter));
                    counter += 1;
                }
            }
            // there are no SiDBs that can be positively charged
            else
            {
                for (const auto& c : strg->sidb_order)
                {
                    chargeindex += static_cast<uint64_t>(
                        (charge_state_to_sign(strg->cell_charge[static_cast<uint64_t>(cell_to_index(c))]) + 1) *
                        std::pow(base, this->num_cells() - 1 - counter));
                    counter += 1;
                }
            }
        }

        strg->charge_index_and_base  = {chargeindex, base};
        strg->charge_index_sublayout = chargeindex_sub_layout;
    }
    /**
     * The charge index of the current charge distribution is returned.
     *
     * @return A pair with the charge index and the used base.
     */
    [[nodiscard]] charge_index_base get_charge_index_and_base() const noexcept
    {
        return strg->charge_index_and_base;
    }
    /**
     * The charge index is increased by one, but only if it is less than the maximum charge index for the given layout.
     * If that's the case, it is increased by one and afterward, the charge configuration is updated by invoking the
     * `index_to_charge_distribution()` function.
     *
     * @param dep_cell `dependent_cell_mode::FIXED` if the state of the dependent cell should not change,
     * `dependent_cell_mode::VARIABLE` if it should.
     * @param energy_calculation_mode `energy_calculation::UPDATE_ENERGY` if the electrostatic potential energy should
     * be updated, `energy_calculation::KEEP_ENERGY otherwise.
     * @param history_mode `charge_distribution_history::NEGLECT` if the information (local electrostatic energy) of the
     * previous charge distribution is used to make the update more efficient, `charge_distribution_history::CONSIDER`
     * otherwise.
     * @param engine The simulation engine used.
     */
    void increase_charge_index_by_one(
        const dependent_cell_mode          dep_cell                = dependent_cell_mode::FIXED,
        const energy_calculation           energy_calculation_mode = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history  history_mode            = charge_distribution_history::NEGLECT,
        const exact_sidb_simulation_engine engine                  = exact_sidb_simulation_engine::EXGS) noexcept
    {
        if (strg->charge_index_and_base.first < strg->max_charge_index)
        {
            strg->charge_index_and_base.first += 1;
            if (engine == exact_sidb_simulation_engine::QUICKEXACT)
            {
                this->index_to_charge_distribution_for_quickexact_simulation();
            }
            else
            {
                this->index_to_charge_distribution(charge_index_recomputation::IGNORE_LEADING_ZEROES);
            }
            this->update_after_charge_change(dep_cell, energy_calculation_mode, history_mode);
        }
    }
    /**
     * This function returns the maximum index of the cell-level layout.
     *
     * @returns The maximal possible charge distribution index.
     */
    [[nodiscard]] uint64_t get_max_charge_index() const noexcept
    {
        return strg->max_charge_index;
    }
    /**
     * Assigns a given charge index to the charge distribution layout. Charge distribution is updated
     * according to the set charge index.
     *
     * @param charge_index charge index of the new charge distribution.
     * @param cdc Setting to determine if the charge distribution should be updated after the charge index is assigned.
     */
    void assign_charge_index(
        const uint64_t                 charge_index,
        const charge_distribution_mode cdc = charge_distribution_mode::UPDATE_CHARGE_DISTRIBUTION) noexcept
    {
        assert((charge_index <= strg->max_charge_index) && "charge index is too large");
        strg->charge_index_and_base.first = charge_index;

        if (cdc == charge_distribution_mode::UPDATE_CHARGE_DISTRIBUTION)
        {
            this->index_to_charge_distribution();
        }
    }
    /**
     * This function is used for the *QuickSim* algorithm (see quicksim.hpp). It gets a vector with indices representing
     * negatively charged SiDBs as input. Afterward, a distant and a neutrally charged SiDB is localized using a min-max
     * diversity algorithm. This selected SiDB is set to "negative" and the index is added to the input vector such that
     * the next iteration works correctly.
     *
     * @param alpha A parameter for the algorithm (default: 0.7).
     * @param negative_indices Vector of SiDBs indices that are already negatively charged (double occupied).
     */
    void adjacent_search(const double alpha, std::vector<uint64_t>& negative_indices) noexcept
    {
        double     dist_max     = 0.0;
        const auto reserve_size = this->num_cells() - negative_indices.size();

        std::vector<uint64_t> index_vector{};
        index_vector.reserve(reserve_size);
        std::vector<double> distance{};
        distance.reserve(reserve_size);

        for (uint64_t unocc = 0u; unocc < strg->cell_charge.size(); unocc++)
        {
            if (strg->cell_charge[unocc] != sidb_charge_state::NEUTRAL)
            {
                continue;
            }

            const auto dist_min =
                std::accumulate(negative_indices.begin(), negative_indices.end(), std::numeric_limits<double>::max(),
                                [&](const double acc, const uint64_t occ)
                                { return std::min(acc, this->get_nm_distance_by_indices(unocc, occ)); });

            index_vector.push_back(unocc);
            distance.push_back(dist_min);

            if (dist_min > dist_max)
            {
                dist_max = dist_min;
            }
        }

        std::vector<uint64_t> candidates{};
        candidates.reserve(reserve_size);

        for (uint64_t i = 0u; i < distance.size(); ++i)
        {
            if (distance[i] >= (alpha * dist_max))
            {
                candidates.push_back(i);
            }
        }

        if (!candidates.empty())
        {
            static std::mt19937_64                  generator(std::random_device{}());
            std::uniform_int_distribution<uint64_t> dist(0, candidates.size() - 1);
            const auto                              random_element = index_vector[candidates[dist(generator)]];
            strg->cell_charge[random_element]                      = sidb_charge_state::NEGATIVE;
            negative_indices.push_back(random_element);

            strg->system_energy += -(*this->get_local_potential_by_index(random_element));

            for (uint64_t i = 0u; i < strg->pot_mat.size(); ++i)
            {
                strg->local_pot[i] += -(this->get_chargeless_potential_by_indices(i, random_element));
            }
        }
    }
    /**
     * This function can be used to assign a global external electrostatic potential in Volt (unit: V) to the layout
     * (e.g this could be a planar external electrode).
     *
     * @param potential_value Value of the global external electrostatic potential in Volt (e.g. -0.3).
     * Charge-transition levels are shifted by this value.
     * @param dep_cell `dependent_cell_mode::FIXED` if the state of the dependent cell should not change,
     * `dependent_cell_mode::VARIABLE` if it should.
     */
    void assign_global_external_potential(const double        potential_value,
                                          dependent_cell_mode dep_cell = dependent_cell_mode::FIXED) noexcept
    {
        if (potential_value != 0.0)
        {
            this->foreach_cell([this, &potential_value](const auto& c)
                               { strg->local_external_pot.insert({c, potential_value}); });
            this->update_after_charge_change(dep_cell);
        }
    }
    /**
     * This function determines if given layout has to be simulated with three states since positively charged SiDBs
     * can occur due to the local potential analysis. In addition, all SiDBs that can be positively charged are
     * collected.
     *
     * @note All SiDBs have to be set to negatively charged.
     *
     * @return return value is true when three state simulation is required.
     */
    bool is_three_state_simulation_required() noexcept
    {
        this->update_after_charge_change();
        strg->three_state_cells                    = {};
        strg->sidb_order_without_three_state_cells = {};
        bool required                              = false;
        strg->dependent_cell_in_sub_layout         = false;

        // check if all SiDBs are negatively charged
        this->foreach_cell(
            [this]([[maybe_unused]] const auto& c) {
                assert(this->get_charge_state(c) == sidb_charge_state::NEGATIVE &&
                       "All SiDBs have to be negatively charged");
            });

        // Each SiDB is checked to see if the local electrostatic potential is high enough
        // to cause a positively charged SiDB
        this->foreach_cell(
            [&required, this](const auto& c)
            {
                if (const auto local_pot = this->get_local_potential(c); local_pot.has_value())
                {
                    if ((-(*local_pot) + strg->simulation_parameters.mu_plus()) >
                        -physical_constants::POP_STABILITY_ERR)
                    {
                        if (c == strg->dependent_cell)
                        {
                            strg->dependent_cell_in_sub_layout = true;
                        }
                        else
                        {
                            strg->three_state_cells.emplace_back(c);
                            required = true;
                        }
                    }
                }
            });

        // sort all SiDBs that can be positively charged by the defined < relation
        std::sort(strg->three_state_cells.begin(), strg->three_state_cells.end());

        // collect all SiDBs that are not among the SiDBs that can be positively charged
        for (const auto& c : strg->sidb_order)
        {
            if (std::find(strg->three_state_cells.cbegin(), strg->three_state_cells.cend(), c) ==
                    strg->three_state_cells.end() &&
                c != strg->dependent_cell)
            {
                strg->sidb_order_without_three_state_cells.push_back(c);
            }
        }

        // sort all SiDBs that cannot be positively charged by the defined < relation
        std::sort(strg->sidb_order_without_three_state_cells.begin(), strg->sidb_order_without_three_state_cells.end());

        // if SiDBs exist that can be positively charged, 3-state simulation is required
        if (required)
        {
            this->assign_base_number_to_three();
        }

        return required;
    }
    /**
     * Counts the number of SiDBs with a negative charge state.
     *
     * @return The number of SiDBs with a negative charge state.
     */
    [[nodiscard]] std::size_t num_negative_sidbs() const noexcept
    {
        uint64_t count_negative_sidbs = 0;
        this->foreach_cell([this, &count_negative_sidbs](const auto& c)
                           { count_negative_sidbs += (get_charge_state(c) == sidb_charge_state::NEGATIVE) ? 1 : 0; });
        return count_negative_sidbs;
    }
    /**
     * Counts the number of SiDBs with a neutral charge state.
     *
     * @return The number of SiDBs with a neutral charge state.
     */
    [[nodiscard]] std::size_t num_neutral_sidbs() const noexcept
    {
        std::size_t count_neutral_sidbs = 0;
        this->foreach_cell([this, &count_neutral_sidbs](const auto& c)
                           { count_neutral_sidbs += (get_charge_state(c) == sidb_charge_state::NEUTRAL) ? 1 : 0; });
        return count_neutral_sidbs;
    }
    /**
     * Counts the number of SiDBs with a positive charge state.
     *
     * @return The number of SiDBs with a positive charge state.
     */
    [[nodiscard]] std::size_t num_positive_sidbs() const noexcept
    {
        std::size_t count_positive_sidbs = 0;
        this->foreach_cell([this, &count_positive_sidbs](const auto& c)
                           { count_positive_sidbs += (get_charge_state(c) == sidb_charge_state::POSITIVE) ? 1 : 0; });
        return count_positive_sidbs;
    }
    /**
     * This functions returns all cells that could be positively charged. However, this must not be necessarily the
     * case in a physically valid layout.
     *
     * @return All cell that could be positively charged.
     */
    std::vector<typename Lyt::cell> get_positive_candidates() const noexcept
    {
        return strg->three_state_cells;
    }
    /**
     * This function searches the index of a cell which is part of the sublayout (i.e. it should be a cell which can
     * be positively charged).
     *
     * @param c Cell that should be part of the sublayout.
     * @return Index (i.e. position in the vector) of the input cell.
     */
    [[nodiscard]] int64_t three_state_cell_to_index(const typename Lyt::cell& c) const noexcept
    {
        if (const auto it = std::find(strg->three_state_cells.cbegin(), strg->three_state_cells.cend(), c);
            it != strg->three_state_cells.cend())
        {
            return static_cast<int64_t>(std::distance(strg->three_state_cells.cbegin(), it));
        }

        return -1;
    }
    /**
     * This function searches the index of a cell which is not part of the sublayout (i.e. it should be a cell which
     * is either neutrally or negatively charged).
     *
     * @param c Cell that should not be part of the sublayout.
     * @return Index (i.e. position in the vector) of the input cell.
     */
    [[nodiscard]] int64_t two_state_cell_to_index(const typename Lyt::cell& c) const noexcept
    {
        if (const auto it = std::find(strg->sidb_order_without_three_state_cells.cbegin(),
                                      strg->sidb_order_without_three_state_cells.cend(), c);
            it != strg->sidb_order_without_three_state_cells.cend())
        {
            return static_cast<int64_t>(std::distance(strg->sidb_order_without_three_state_cells.cbegin(), it));
        }

        return -1;
    }
    /**
     * This function searches the cell of a given index.
     *
     * @param c The index to find the cell of.
     * @return The cell in the layout for the given index. Returns dead-coordinate if the index is not assigned to a
     * not empty cell in the layout.
     */
    [[nodiscard]] typename Lyt::cell index_to_cell(const uint64_t index) const noexcept
    {
        if (index < strg->sidb_order.size())
        {
            return strg->sidb_order[index];
        }

        return {};
    }
    /**
     * This function finds the cell for a given index which is a candidate to be positively charged of a given
     * index.
     *
     * @param index The index to find the cell of (cell is candidate to be positively charged).
     * @return Positive cell candidate. Dead-coordinate is returned if the index is not assigned to a not
     * empty cell in the layout.
     */
    [[nodiscard]] typename Lyt::cell index_to_three_state_cell(const uint64_t index) const noexcept
    {
        if (index < strg->three_state_cells.size())
        {
            return strg->three_state_cells[index];
        }

        return {};
    }
    /**
     * This function finds the cell which can only be neutrally or negatively charged of a given index.
     *
     * @param index The index to find the cell of.
     * @return The cell (which cannot be positively charged) in the layout for the given index. Dead-coordinate is
     * returned if the index is not assigned to a not empty cell in the layout.
     */
    [[nodiscard]] typename Lyt::cell index_to_two_state_cell(const uint64_t index) const noexcept
    {
        if (index < strg->sidb_order_without_three_state_cells.size())
        {
            return strg->sidb_order_without_three_state_cells[index];
        }

        return {};
    }
    /**
     * This function calculates and returns the chargeless electrostatic potential in Volt for a given distance in
     * nanometer.
     *
     * @param distance Distance in nanometer between position and defect (unit: nm).
     * @return The chargeless electrostatic potential at a given distance (unit: V).
     */
    [[nodiscard]] double chargeless_potential_at_given_distance(const double distance) const noexcept
    {
        if (distance == 0.0)
        {
            return 0.0;
        }

        return (strg->simulation_parameters.k() / (distance * 1E-9) *
                std::exp(-distance / strg->simulation_parameters.lambda_tf) * physical_constants::ELEMENTARY_CHARGE);
    }
    /**
     * This function calculates the chargeless potential in Volt generated by a defect at a given distance in
     * nanometer.
     *
     * @param distance Distance between position and defect (unit: nm.
     * @param sidb_defect Defect (including defect specific parameters).
     * @return The chargeless electrostatic potential in Volt generated by the defect at a given distance (unit: V).
     */
    [[nodiscard]] double
    chargeless_potential_generated_by_defect_at_given_distance(const double       distance,
                                                               const sidb_defect& defect = sidb_defect{}) const noexcept
    {
        if (distance == 0.0)
        {
            return 0.0;
        }

        return strg->simulation_parameters.k() * strg->simulation_parameters.epsilon_r / defect.epsilon_r /
               (distance * 1e-9) * std::exp(-distance / defect.lambda_tf) * physical_constants::ELEMENTARY_CHARGE;
    }
    /**
     * This function can be used to assign an external local electrostatic potential in Volt to the layout. All
     * important attributes of the charge layout are updated automatically.
     *
     * @param cell Cell to which the local external potential is applied.
     * @param external_voltage External electrostatic potential in Volt applied to different cells.
     */
    void
    assign_local_external_potential(const std::unordered_map<typename Lyt::cell, double>& external_potential) noexcept
    {
        strg->local_external_pot = external_potential;
        this->update_after_charge_change();
    }
    /**
     * This function returns the local external electrostatic potential in Volt applied to the layout.
     *
     * @return External electrostatic potential as unordered map. The cell is used as key and the external
     * electrostatic potential in Volt (unit: V) at its position as value.
     */
    std::unordered_map<typename Lyt::cell, double> get_local_external_potentials() noexcept
    {
        return strg->local_external_pot;
    }
    /**
     * This function returns the local electrostatic potentials which are generated by defects.
     *
     * @return Local electrostatic potential in Volt generated by the defects at each each cell.
     */
    std::unordered_map<typename Lyt::cell, double> get_local_defect_potentials() noexcept
    {
        return strg->defect_local_pot;
    }
    /**
     * This function returns the defects.
     *
     * @return Placed defects with cell position and type.
     */
    std::unordered_map<typename Lyt::cell, const sidb_defect> get_defects() noexcept
    {
        return strg->defects;
    }
    /**
     * The charge state of the dependent-SiDB is updated based on the local electrostatic potential at its position.
     * All other local electrostatic potentials are then also updated if the charge state of the dependent-SiDB has
     * changed.
     */
    void update_charge_state_of_dependent_cell() noexcept
    {
        if (!strg->dependent_cell.is_dead())
        {
            const auto loc_pot_cell = -strg->local_pot[strg->dependent_cell_index];
            if ((loc_pot_cell + strg->simulation_parameters.mu_minus) < physical_constants::POP_STABILITY_ERR)
            {
                if (strg->cell_charge[strg->dependent_cell_index] != sidb_charge_state::NEGATIVE)
                {
                    const auto charge_diff = (-charge_state_to_sign(strg->cell_charge[strg->dependent_cell_index]) - 1);
                    for (uint64_t i = 0u; i < strg->pot_mat.size(); ++i)
                    {
                        if (i != strg->dependent_cell_index)
                        {
                            strg->local_pot[i] +=
                                (this->get_chargeless_potential_by_indices(i, strg->dependent_cell_index)) *
                                charge_diff;
                        }
                    }
                    strg->cell_charge[strg->dependent_cell_index] = sidb_charge_state::NEGATIVE;
                }
            }
            else if ((loc_pot_cell + strg->simulation_parameters.mu_plus()) > -physical_constants::POP_STABILITY_ERR)
            {
                // dependent-cell can only be positively charged when the base number is set to three state
                // simulation.
                if (strg->charge_index_and_base.second == 3 || strg->three_state_cells.size() != 0)
                {
                    if (strg->cell_charge[strg->dependent_cell_index] != sidb_charge_state::POSITIVE)
                    {
                        const auto charge_diff =
                            (-charge_state_to_sign(strg->cell_charge[strg->dependent_cell_index]) + 1);
                        strg->cell_charge[strg->dependent_cell_index] = sidb_charge_state::POSITIVE;
                        for (uint64_t i = 0u; i < strg->pot_mat.size(); ++i)
                        {
                            if (i != strg->dependent_cell_index)
                            {
                                strg->local_pot[i] +=
                                    (this->get_chargeless_potential_by_indices(i, strg->dependent_cell_index)) *
                                    charge_diff;
                            }
                        }
                    }
                }
            }

            else
            {
                if (strg->cell_charge[strg->dependent_cell_index] != sidb_charge_state::NEUTRAL)
                {
                    const auto charge_diff = (-charge_state_to_sign(strg->cell_charge[strg->dependent_cell_index]));
                    for (uint64_t i = 0u; i < strg->pot_mat.size(); ++i)
                    {
                        if (i != strg->dependent_cell_index)
                        {
                            strg->local_pot[i] +=
                                (this->get_chargeless_potential_by_indices(i, strg->dependent_cell_index)) *
                                charge_diff;
                        }
                    }
                    strg->cell_charge[strg->dependent_cell_index] = sidb_charge_state::NEUTRAL;
                }
            }
        }
    }
    /**
     * This function returns the charge index of the sublayout (cells that can be positively charged).
     *
     * @returns The charge distribution index of the sublayout.
     */
    [[nodiscard]] uint64_t get_charge_index_of_sub_layout() const noexcept
    {
        return strg->charge_index_sublayout;
    }
    /**
     * This function changes the current charge distribution based on two given Gray codes (Important: The two Gray
     * codes should only differ by one bit)
     *
     * @param new_gray_code Gray code as uint64_t of the new charge distribution.
     * @param old_gray_code Gray code as uint64_t of the previous charge distribution layout.
     */
    void charge_index_gray_code_to_charge_distribution(const uint64_t new_gray_code,
                                                       const uint64_t old_gray_code) noexcept
    {
        strg->cell_history_gray_code = {};

        const std::bitset<64> r_new(new_gray_code);
        const std::bitset<64> r_old(old_gray_code);
        const std::bitset<64> diff = r_new ^ r_old;

        uint64_t index_changed = 0;

        // Check if there are differing bits (if 'diff' is not equal to 0).
        if (diff != 0)
        {
            // Find the bit position of the first difference. This position then describes the cell index of the
            // SiDB that has changed its charge state.
            while (index_changed < diff.size() && !diff.test(index_changed))
            {
                index_changed++;
            }

            const auto sign_old = static_cast<int8_t>(-1 * static_cast<int8_t>(r_old[index_changed]));
            const auto sign_new = static_cast<int8_t>(-1 * static_cast<int8_t>(r_new[index_changed]));

            if (index_changed < strg->dependent_cell_index)
            {
                strg->cell_history_gray_code.first  = static_cast<int64_t>(index_changed);
                strg->cell_history_gray_code.second = sign_old;
                this->assign_charge_state_by_cell_index(index_changed, sign_to_charge_state(sign_new),
                                                        charge_index_mode::KEEP_CHARGE_INDEX);
            }
            else
            {
                strg->cell_history_gray_code.first  = static_cast<int64_t>(index_changed) + 1;
                strg->cell_history_gray_code.second = sign_old;
                this->assign_charge_state_by_cell_index(index_changed + 1, sign_to_charge_state(sign_new),
                                                        charge_index_mode::KEEP_CHARGE_INDEX);
            }
        }
        else
        {
            strg->cell_history_gray_code.first  = -1;
            strg->cell_history_gray_code.second = 0;
        }
    }
    /**
     * The charge index of the sublayout is increased by one and the charge distribution is updated correspondingly.
     *
     * @param dependent_cell `dependent_cell_mode::FIXED` if the state of the dependent cell should not change,
     * `dependent_cell_mode::VARIABLE` if it should.
     * @param energy_calculation_mode `energy_calculation::UPDATE_ENERGY` if the electrostatic potential energy should
     * be updated, `energy_calculation::KEEP_ENERGY` otherwise.
     * @param history_mode `charge_distribution_history::NEGLECT` if the information (local electrostatic energy) of the
     * previous charge distribution is used to make the update more efficient, `charge_distribution_history::CONSIDER`
     * otherwise.
     * @param engine The simulation engine used.
     */
    void increase_charge_index_of_sub_layout_by_one(
        const dependent_cell_mode          dependent_cell_fixed    = dependent_cell_mode::FIXED,
        const energy_calculation           recompute_system_energy = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history  consider_history        = charge_distribution_history::NEGLECT,
        const exact_sidb_simulation_engine engine                  = exact_sidb_simulation_engine::QUICKEXACT) noexcept
    {
        if (strg->charge_index_sublayout < strg->max_charge_index_sulayout)
        {
            strg->charge_index_sublayout += 1;
            if (engine == exact_sidb_simulation_engine::QUICKEXACT)
            {
                this->index_to_charge_distribution_for_quickexact_simulation();
            }
            else
            {
                this->index_to_charge_distribution(charge_index_recomputation::IGNORE_LEADING_ZEROES);
            }
            this->update_after_charge_change(dependent_cell_fixed, recompute_system_energy, consider_history);
        }
    }
    /**
     * The charge index is assigned by a Gray code number in decimal.
     *
     * @param current_gray_code Gray code in decimal representing the new charge distribution.
     * @param previous_gray_code Gray code in decimal representing the old charge distribution.
     * @param dep_cell `dependent_cell_mode::FIXED` if the state of the dependent cell should not change,
     * `dependent_cell_mode::VARIABLE` if it should.
     * @param energy_calculation_mode `energy_calculation::UPDATE_ENERGY` if the electrostatic potential energy should
     * be updated, `energy_calculation::KEEP_ENERGY` otherwise.
     * @param history_mode `charge_distribution_history::NEGLECT` if the information (local electrostatic energy) of the
     * previous charge distribution is used to make the update more efficient, `charge_distribution_history::CONSIDER`
     * otherwise.
     */
    void assign_charge_index_by_gray_code(
        const uint64_t current_gray_code, const uint64_t previous_gray_code,
        const dependent_cell_mode         dep_cell         = dependent_cell_mode::FIXED,
        const energy_calculation          energy_calc_mode = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history history_mode     = charge_distribution_history::NEGLECT) noexcept
    {
        if (current_gray_code <= strg->max_charge_index)
        {
            this->assign_charge_index_by_two_gray_codes(current_gray_code, previous_gray_code);
            this->update_after_charge_change(dep_cell, energy_calc_mode, history_mode);
        }
    }
    /**
     * Resets the charge index of the sublayout (cells of the layout that can also be positively charged).
     *
     * @param engine The simulation engine used.
     */
    void reset_charge_index_sub_layout(
        const exact_sidb_simulation_engine engine = exact_sidb_simulation_engine::QUICKEXACT) noexcept
    {
        strg->charge_index_sublayout = 0;
        if (engine == exact_sidb_simulation_engine::QUICKEXACT)
        {
            this->index_to_charge_distribution_for_quickexact_simulation();
        }
        else
        {
            this->index_to_charge_distribution();
        }
        this->update_after_charge_change(dependent_cell_mode::VARIABLE, energy_calculation::KEEP_OLD_ENERGY_VALUE,
                                         charge_distribution_history::CONSIDER);
    }
    /**
     * Returns the maximum index of the sublayout (cells that can be positively charged).
     *
     * @returns The maximal possible charge distribution index of the sublayout.
     */
    [[nodiscard]] uint64_t get_max_charge_index_sub_layout() const noexcept
    {
        return strg->max_charge_index_sulayout;
    }
    /**
     * Assign a given charge index to the charge distribution layout. This function should be used if new and old
     * charge index are given as Gray code to provide high performance.
     *
     * @param gray_code charge index (as Gray code in decimal) of the new charge distribution.
     * @param gray_code_old charge index (as Gray code in decimal) of the old charge distribution.
     */
    void assign_charge_index_by_two_gray_codes(const uint64_t gray_code, const uint64_t gray_code_old) noexcept
    {
        strg->charge_index_and_base.first = gray_code;
        this->charge_index_gray_code_to_charge_distribution(gray_code, gray_code_old);
    }
    /**
     * This function returns all SiDBs of the layout.
     *
     * @return Vector with all cells.
     */
    [[nodiscard]] std::vector<typename Lyt::cell> get_sidb_order() const noexcept
    {
        return strg->sidb_order;
    }
    /**
     * This function can be used to add an SiDB to the layout. The SiDB is only added to the cell_charge and the
     * sidb_order vector.
     *
     * @param c Cell which is added to the layout.
     * @param charge Charge state of the added cell.
     */
    void add_sidb(const typename Lyt::cell& c, const sidb_charge_state charge) noexcept
    {
        strg->cell_charge.push_back(charge);
        strg->sidb_order.push_back(c);

        // sort sidbs by the relation given by the coordinates and sort charge vector accordingly
        std::vector<std::pair<typename Lyt::cell, sidb_charge_state>> combined_vector{};
        combined_vector.reserve(strg->cell_charge.size());

        for (size_t i = 0; i < strg->sidb_order.size(); i++)
        {
            combined_vector.emplace_back(strg->sidb_order[i], strg->cell_charge[i]);
        }

        std::sort(combined_vector.begin(), combined_vector.end());

        for (size_t i = 0; i < combined_vector.size(); i++)
        {
            strg->sidb_order[i]  = combined_vector[i].first;
            strg->cell_charge[i] = combined_vector[i].second;
        }
    }

  private:
    storage strg;

    /**
     * Initialization function used for the construction of the charge distribution surface.
     *
     * @param configuration Specifies the configuration for charge distribution settings.
     *                      Determines whether only charge locations are considered or if
     *                      both charge locations and electrostatic interactions are included.
     */
    void
    initialize(const sidb_charge_state cs            = sidb_charge_state::NEGATIVE,
               const cds_configuration configuration = cds_configuration::CHARGE_LOCATION_AND_ELECTROSTATIC) noexcept
    {
        const auto param_copy       = strg->simulation_parameters;
        strg                        = std::make_shared<charge_distribution_storage>();
        strg->simulation_parameters = param_copy;
        strg->sidb_order.reserve(this->num_cells());
        strg->cell_charge.reserve(this->num_cells());
        this->foreach_cell([this](const auto& c1) { strg->sidb_order.push_back(c1); });
        std::sort(strg->sidb_order.begin(), strg->sidb_order.end());
        this->foreach_cell([this, &cs](const auto&) { strg->cell_charge.push_back(cs); });

        assert((((this->num_cells() < 41) && (strg->simulation_parameters.base == 3)) ||
                ((strg->simulation_parameters.base == 2) && (this->num_cells() < 64))) &&
               "number of SiDBs is too large");

        this->charge_distribution_to_index();
        strg->max_charge_index = static_cast<uint64_t>(
            std::pow(static_cast<double>(strg->simulation_parameters.base), this->num_cells()) - 1);

        if (configuration == cds_configuration::CHARGE_LOCATION_AND_ELECTROSTATIC)
        {
            this->initialize_nm_distance_matrix();
            this->initialize_potential_matrix();
            if constexpr (is_sidb_defect_surface_v<Lyt>)
            {
                Lyt::foreach_sidb_defect([this](const auto cd)
                                         { add_sidb_defect_to_potential_landscape(cd.first, cd.second); });
            }
            this->update_local_potential();
            this->recompute_system_energy();
            this->validity_check();
        }
    };
    /**
     * This function is used when three state simulations are required (i.e., is_three_state_simulation_required =
     * true) to set the base number to three. However, it is distinguished between the cells that can be positively
     * charged an the ones that cannot.
     *
     * @note is_three_state_simulation_required() has to be executed first.
     */
    void assign_base_number_to_three() noexcept
    {
        strg->simulation_parameters.base   = 3;
        strg->charge_index_and_base.second = 2;
        if (!strg->dependent_cell.is_dead())
        {
            if (!strg->three_state_cells.empty())
            {

                strg->max_charge_index =
                    static_cast<uint64_t>(std::pow(2, this->num_cells() - 1 - strg->three_state_cells.size()) - 1);
                strg->max_charge_index_sulayout =
                    static_cast<uint64_t>(std::pow(3, strg->three_state_cells.size()) - 1);
            }
        }
        else
        {
            strg->max_charge_index          = static_cast<uint64_t>(std::pow(3, this->num_cells()) - 1);
            strg->max_charge_index_sulayout = static_cast<uint64_t>(std::pow(3, strg->three_state_cells.size()) - 1);
        }
        if (strg->max_charge_index == 0)
        {
            this->assign_charge_index(0);
        }
    }

    /**
     * Initializes the distance matrix between all the cells of the layout.
     */
    void initialize_nm_distance_matrix() noexcept
    {
        strg->nm_dist_mat =
            std::vector<std::vector<double>>(this->num_cells(), std::vector<double>(this->num_cells(), 0.0));

        for (uint64_t i = 0u; i < strg->sidb_order.size(); ++i)
        {
            for (uint64_t j = 0u; j < strg->sidb_order.size(); j++)
            {
                strg->nm_dist_mat[i][j] = sidb_nm_distance<Lyt>(*this, strg->sidb_order[i], strg->sidb_order[j]);
            }
        }
    }
    /**
     * Initializes the potential matrix between all the cells of the layout.
     */
    void initialize_potential_matrix() noexcept
    {
        strg->pot_mat =
            std::vector<std::vector<double>>(this->num_cells(), std::vector<double>(this->num_cells(), 0.0));

        for (uint64_t i = 0u; i < strg->sidb_order.size(); ++i)
        {
            for (uint64_t j = 0u; j < strg->sidb_order.size(); j++)
            {
                strg->pot_mat[i][j] = calculate_chargeless_potential_between_sidbs_by_index(i, j);
            }
        }
    }

    /**
     *  The stored unique index is converted to a charge distribution.
     *
     *  @note This function is only used for *QuickExact*
     */
    void index_to_charge_distribution_for_quickexact_simulation() noexcept
    {
        // This scope is executed if the function is used by `quickexact`.
        // Cell_history collects the cells (SiDBs) that have changed their charge state.
        strg->cell_history = {};
        strg->cell_history.reserve(this->num_cells());

        if (strg->charge_index_sublayout == 0)
        {
            for (const auto& c : strg->three_state_cells)
            {
                this->assign_charge_state(c, sidb_charge_state::NEGATIVE, charge_index_mode::KEEP_CHARGE_INDEX);
            }
        }

        if (strg->charge_index_and_base.first == 0)
        {
            for (const auto& c : strg->sidb_order_without_three_state_cells)
            {
                this->assign_charge_state(c, sidb_charge_state::NEGATIVE, charge_index_mode::KEEP_CHARGE_INDEX);
            }
        }

        auto       charge_quot_positive = strg->charge_index_sublayout;
        const auto base_positive        = 3;
        auto       counter              = strg->three_state_cells.size() - 1;
        // Firstly, the charge distribution of the sublayout (i.e., collection of SiDBs that can be positively
        // charged) is updated.
        while (charge_quot_positive > 0)
        {
            const auto    charge_quot_int = static_cast<int64_t>(charge_quot_positive);
            const auto    base_int        = static_cast<int64_t>(base_positive);
            const int64_t quotient_int    = charge_quot_int / base_int;
            const int64_t remainder_int   = charge_quot_int % base_int;
            charge_quot_positive          = static_cast<uint64_t>(quotient_int);

            const auto sign = sign_to_charge_state(static_cast<int8_t>(remainder_int - 1));
            if (const auto new_chargesign = this->get_charge_state_by_index(
                    static_cast<uint64_t>(cell_to_index(index_to_three_state_cell(counter))));
                new_chargesign != sign)
            {
                strg->cell_history.emplace_back(
                    static_cast<uint64_t>(cell_to_index(index_to_three_state_cell(counter))),
                    charge_state_to_sign(new_chargesign));
                this->assign_charge_state_by_cell_index(
                    static_cast<uint64_t>(cell_to_index(index_to_three_state_cell(counter))), sign,
                    charge_index_mode::KEEP_CHARGE_INDEX);
            }
            counter -= 1;
        }

        const auto dependent_cell_index_two_state = two_state_cell_to_index(strg->dependent_cell);
        auto       charge_quot                    = strg->charge_index_and_base.first;
        const auto base                           = strg->charge_index_and_base.second;
        auto       counter_negative               = strg->sidb_order_without_three_state_cells.size() - 1;

        // Secondly, the charge distribution of the layout (only SiDBs which can be either neutrally or negatively
        // charged) is updated.
        while (charge_quot > 0)
        {
            const auto    charge_quot_int = static_cast<int64_t>(charge_quot);
            const auto    base_int        = static_cast<int64_t>(base);
            const int64_t quotient_int    = charge_quot_int / base_int;
            const int64_t remainder_int   = charge_quot_int % base_int;
            charge_quot                   = static_cast<uint64_t>(quotient_int);
            // If the current position is not the dependent-cell position, the charge state is updated.
            if (counter_negative == static_cast<uint64_t>(dependent_cell_index_two_state))
            {
                counter_negative -= 1;
            }
            const auto sign = sign_to_charge_state(static_cast<int8_t>(remainder_int - 1));
            if (const auto new_chargesign = this->get_charge_state_by_index(static_cast<uint64_t>(
                    cell_to_index(index_to_two_state_cell(static_cast<uint64_t>(counter_negative)))));
                new_chargesign != sign)
            {
                strg->cell_history.emplace_back(static_cast<uint64_t>(cell_to_index(
                                                    index_to_two_state_cell(static_cast<uint64_t>(counter_negative)))),
                                                charge_state_to_sign(new_chargesign));
                this->assign_charge_state_by_cell_index(static_cast<uint64_t>(cell_to_index(index_to_two_state_cell(
                                                            static_cast<uint64_t>(counter_negative)))),
                                                        sign, charge_index_mode::KEEP_CHARGE_INDEX);
            }
            counter_negative -= 1;
            // If the current position is the dependent cell position, first the counter_negative is decremented
            // by one to get to the next cell position to update its charge state.
        }
    }
    /**
     *  The stored unique index is converted to a charge distribution.
     *
     *  @param charge_index_recompuation_mode Flag that can be set to `IGNORE_LEADING_ZEROES` if the charge index
     * was incremented before calling this function to enable an optimization. When set to `IGNORE_LEADING_ZEROES`,
     * the leading zeroes of the charge index are ignored.
     */
    void index_to_charge_distribution(const charge_index_recomputation charge_index_recomputation_mode =
                                          charge_index_recomputation::FROM_SCRATCH) noexcept
    {
        // A charge index of zero corresponds to a layout with all SiDBs set to negative.
        if (strg->charge_index_and_base.first == 0)
        {
            this->assign_all_charge_states(sidb_charge_state::NEGATIVE);
            return;
        }

        const auto dependent_cell_index = cell_to_index(strg->dependent_cell);
        const bool has_dependent_cell   = dependent_cell_index >= 0;

        const auto base        = static_cast<uint64_t>(strg->charge_index_and_base.second);
        uint64_t   charge_quot = strg->charge_index_and_base.first;

        auto counter = static_cast<int64_t>(this->num_cells() - 1);

        while (charge_quot > 0)
        {
            const auto charge_state = sign_to_charge_state(static_cast<int8_t>(charge_quot % base) - 1);

            // Dependent-SiDB is skipped since its charge state is not changed based on the charge index.

            // If the counter is at the dependent-cell location, it is reduced by one to get to the next
            // cell position.
            if (has_dependent_cell && counter == dependent_cell_index)
            {
                // The charge state is only changed (i.e., the function assign_charge_state_by_cell_index is
                // called), if the nw charge state differs to the previous one. Only then will the cell be
                // added to the charge_distribution_history.
                counter -= 1;
            }

            this->assign_charge_state_by_cell_index(static_cast<uint64_t>(counter), charge_state,
                                                    charge_index_mode::KEEP_CHARGE_INDEX);

            charge_quot /= base;
            counter -= 1;
        }

        if (charge_index_recomputation_mode == charge_index_recomputation::IGNORE_LEADING_ZEROES)
        {
            return;
        }

        // If the counter is >= 0, then the first <counter> cells should be assigned a negative charge state.
        for (uint64_t i = 0; static_cast<int64_t>(i) <= counter; ++i)
        {
            this->assign_charge_state_by_cell_index(i, sidb_charge_state::NEGATIVE,
                                                    charge_index_mode::KEEP_CHARGE_INDEX);
        }
    }
};

template <class T>
charge_distribution_surface(const T&) -> charge_distribution_surface<T>;

template <class T>
charge_distribution_surface(const T&, const sidb_simulation_parameters&) -> charge_distribution_surface<T>;

template <class T>
charge_distribution_surface(const T&, const sidb_simulation_parameters&,
                            const sidb_charge_state cs) -> charge_distribution_surface<T>;

template <class T>
charge_distribution_surface(const T&, const sidb_simulation_parameters&, sidb_charge_state cs,
                            cds_configuration cds_configuration) -> charge_distribution_surface<T>;

}  // namespace fiction

#endif  // FICTION_CHARGE_DISTRIBUTION_SURFACE_HPP
