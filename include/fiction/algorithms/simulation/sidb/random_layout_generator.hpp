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

namespace fiction
{
template <typename Lyt>
void generate_random_layouts(const typename Lyt::aspect_ratio& max_coordinate = {}, uint64_t number_placed_sidbs = 0,
                             const std::string& layout_name = "", const std::string& path = "",
                             uint64_t maximal_attempts = 10E6, const bool prevent_positive_charges = true,
                             std::vector<Lyt>& all_layouts = {})
{
    Lyt                    lyt{max_coordinate, layout_name};
    const auto             x_dimension = max_coordinate.x;
    const auto             y_dimension = max_coordinate.y;
    static std::mt19937_64 generator(std::random_device{}());
    uint64_t               loop_counter = 0;

    while (lyt.num_cells() < number_placed_sidbs && loop_counter < maximal_attempts)
    {
        std::uniform_int_distribution<uint64_t> dist_x(0, x_dimension);
        std::uniform_int_distribution<uint64_t> dist_y(0, y_dimension);
        const auto                              random_x_coordinate = dist_x(generator);
        const auto                              random_y_coordinate = dist_y(generator);
        const auto random_coordinate = typename Lyt::coordinate({random_x_coordinate, random_y_coordinate});

        bool constraint_violation = false;

        if (prevent_positive_charges)
        {
            lyt.foreach_cell(
                [&lyt, &random_coordinate, &constraint_violation](const auto& c1)
                {
                    if (euclidean_distance<Lyt>(lyt, c1, random_coordinate) < 2)
                    {
                        constraint_violation = true;
                    }
                });
        }

        if (!constraint_violation)
        {
            lyt.assign_cell_type(random_coordinate, Lyt::cell_type::NORMAL);
        }
        loop_counter += 1;

        //        if (lyt.num_cells() == number_placed_sidbs - 4)
        //        {
        //            const auto random_x_coordinate_close = dist_x(generator);
        //            const auto random_y_coordinate_close = dist_y(generator);
        //            const auto random_coordinate_close =
        //                typename Lyt::coordinate({random_x_coordinate_close, random_y_coordinate_close});
        //            lyt.assign_cell_type({random_coordinate_close.x, random_coordinate_close.y},
        //            Lyt::cell_type::NORMAL); lyt.assign_cell_type({random_coordinate_close.x+1,
        //            random_coordinate_close.y}, Lyt::cell_type::NORMAL);
        //            lyt.assign_cell_type({random_coordinate_close.x+2, random_coordinate_close.y},
        //            Lyt::cell_type::NORMAL); lyt.assign_cell_type({random_coordinate_close.x+3,
        //            random_coordinate_close.y}, Lyt::cell_type::NORMAL);
        //        }
    }

    uint64_t counter = 0;
    for (const auto& old_lyt : all_layouts)
    {

        old_lyt.foreach_cell(
            [&all_layouts, &counter, lyt](const auto& cell_old) mutable
            {
                lyt.foreach_cell(
                    [&all_layouts, &counter, &cell_old](const auto& cell_new) mutable
                    {
                        if (cell_new == cell_old)
                        {
                            counter += 1;
                        }
                    });
            });
    }

    if (lyt.num_cells() == number_placed_sidbs && counter == 0)
    {
        write_sqd_layout(lyt, path + lyt.get_layout_name());
    }
}

}  // namespace fiction

#endif  // FICTION_RANDOM_LAYOUT_GENERATOR_HPP
