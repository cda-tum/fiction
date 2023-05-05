//
// Created by Jan Drewniok on 05.04.23.
//

#ifndef FICTION_RANDOM_LAYOUT_GENERATOR_HPP
#define FICTION_RANDOM_LAYOUT_GENERATOR_HPP

#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/io/write_sqd_layout.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_nm_position.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <iostream>
#include <vector>

namespace fiction
{
/**
 * This struct stores the parameters for the *generate_random_layouts* algorithm.
 */
template <typename Lyt>
struct random_layout_params
{
    /**
     * Aspect ratio of the layout (i.e. the size of the layout).
     */
    typename Lyt::aspect_ratio max_coordinate;
    /**
     * Number of SiDBs that are placed on the layout.
     */
    uint64_t number_placed_sidbs = 0;
    /**
     * If positively charged SiDBs should be prevented, SiDBs are not placed closer than the minimal_spacing.
     */
    bool prevent_positive_charges = true;
    /**
     * If positively charged SiDBs should be prevented, SiDBs are not placed closer than this value (2 cells as
     * Euclidean distance by default).
     */
    double minimal_spacing = 2;
    /**
     * Maximal number of attempts to generate the layout.
     */
    uint64_t maximal_attempts = 10E6;
};

namespace detail
{
template <typename Lyt>
class generate_random_layout_impl
{
  public:
    generate_random_layout_impl(const random_layout_params<Lyt>& params, std::ostream& s,
                                std::vector<Lyt>& all_layouts) :

            parameter{params},
            os{s},
            previous_layouts{all_layouts}
    {}

    void run()
    {
        bool successful_generation = false;
        while (!successful_generation && parameter.maximal_attempts)
        {
            // layout is initialized with given aspect ratio and name.
            Lyt lyt{parameter.max_coordinate};
            // maximal x-coordinate is stored.
            auto x_dimension = parameter.max_coordinate.x;
            // maximal y-coordinate is stored.
            auto                   y_dimension = parameter.max_coordinate.y;
            static std::mt19937_64 generator(std::random_device{}());
            uint64_t               attempt_counter = 0;
            // this while stops if either all SiDBs are placed or the maximum number of attempts were performed.
            while (lyt.num_cells() < parameter.number_placed_sidbs && attempt_counter < parameter.maximal_attempts)
            {
                // uniform distribution of [0,x_coordinate].
                std::uniform_int_distribution<uint64_t> dist_x(0u, static_cast<uint64_t>(x_dimension));
                // uniform distribution of [0,y_coordinate].
                std::uniform_int_distribution<uint64_t> dist_y(0u, static_cast<uint64_t>(y_dimension));
                // random integer from [0,x_coordinate] is selected.
                const auto random_x_coordinate = dist_x(generator);
                // random integer from [0,y_coordinate] is selected.
                const auto random_y_coordinate = dist_y(generator);
                // coordinate is constructed [x_coordinate, y_coordinate]
                const auto random_coordinate = typename Lyt::coordinate({random_x_coordinate, random_y_coordinate});

                bool constraint_violation_positive_sidbs = false;

                if (parameter.prevent_positive_charges)
                {
                    // it checks if new coordinate is not closer than 2 cells (Euclidean distance).
                    lyt.foreach_cell(
                        [this, &lyt, &random_coordinate, &constraint_violation_positive_sidbs](const auto& c1)
                        {
                            if (euclidean_distance<Lyt>(lyt, c1, random_coordinate) < parameter.minimal_spacing)
                            {
                                constraint_violation_positive_sidbs = true;
                            }
                        });
                }

                if (!constraint_violation_positive_sidbs)
                {
                    lyt.assign_cell_type(random_coordinate, Lyt::cell_type::NORMAL);
                }
                attempt_counter += 1;
            }

            // it checks if new-found layout is identical to an already found layout (all_layouts).
            uint64_t identical_layout_counter = 0;
            for (const auto& old_lyt : previous_layouts)
            {
                old_lyt.foreach_cell(
                    [&identical_layout_counter, lyt](const auto& cell_old) mutable
                    {
                        lyt.foreach_cell(
                            [&identical_layout_counter, &cell_old](const auto& cell_new) mutable
                            {
                                if (cell_new == cell_old)
                                {
                                    identical_layout_counter += 1;
                                }
                            });
                    });
            }

            // if all SiDBs are placed and the new layout is not a duplication, the layout is written to the
            // std::ostream.
            if (lyt.num_cells() == parameter.number_placed_sidbs && identical_layout_counter == 0)
            {
                layout = lyt;
                write_sqd_layout(lyt, os);
                successful_generation = true;
            }
        }
    }

    Lyt get_layout() const
    {
        return layout;
    }

  private:
    Lyt                       layout;
    random_layout_params<Lyt> parameter;
    std::ostream&             os;
    std::vector<Lyt>          previous_layouts;
};
}  // namespace detail

/**
 * This algorithm generates a layout with randomly distributed SiDBs.
 *
 * @tparam Lyt Cell-level with fiction coordinates.
 * @param params All parameters needed for the generation of random layouts.
 * @param os The output stream to write into.
 * @param all_layouts Previous generated layouts to avoid duplication.
 */
template <typename Lyt>
Lyt generate_random_layout(const random_layout_params<Lyt>& params, std::ostream& os, std::vector<Lyt> layouts)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(!has_siqad_coord_v<Lyt>, "Lyt is based on SiQAD coordinates");

    detail::generate_random_layout_impl<Lyt> p{params, os, layouts};
    p.run();
    return p.get_layout();
}
/**
 * This algorithm generates a layout with randomly distributed SiDBs.
 *
 * @tparam Lyt Cell-level with fiction coordinates.
 * @param params All parameters needed for the generation of random layouts.
 * @param filename The file name to create and write into. Should preferably use the `.sqd` extension.
 * @param all_layouts Previous generated layouts to avoid duplication.
 */
template <typename Lyt>
Lyt generate_random_layout(const random_layout_params<Lyt>& params, const std::string_view& filename,
                           std::vector<Lyt>& all_layouts = {})
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    const auto lyt = generate_random_layout(params, os, all_layouts);
    os.close();
    return lyt;
}

}  // namespace fiction

#endif  // FICTION_RANDOM_LAYOUT_GENERATOR_HPP
