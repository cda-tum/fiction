//
// Created by Jan Drewniok on 05.04.23.
//

#ifndef FICTION_GENERATED_RANDOM_LAYOUTS_HPP
#define FICTION_GENERATED_RANDOM_LAYOUTS_HPP

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
void generate_random_layouts(const typename Lyt::aspect_ratio& max_coordinate = {}, uint64_t number_placed_DBs = 0,
                             const std::string layout_name = "", std::string path = "",
                             uint64_t maximal_attempts = 10E6, const bool prevent_positive_charges = true)
{
    Lyt                    lyt{max_coordinate, layout_name};
    const auto             x_dimension = max_coordinate.x;
    const auto             y_dimension = max_coordinate.y;
    static std::mt19937_64 generator(std::random_device{}());
    uint64_t               loop_counter = 0;

    while (lyt.num_cells() < number_placed_DBs && loop_counter < maximal_attempts)
    {
        std::uniform_int_distribution<uint64_t> dist_x(0, x_dimension);
        std::uniform_int_distribution<uint64_t> dist_y(0, y_dimension);
        const auto                              random_x_coordinate = dist_x(generator);
        const auto                              random_y_coordinate = dist_y(generator);
        const auto random_coordiante = typename Lyt::coordinate({random_x_coordinate, random_y_coordinate});

        bool distance_violance = false;
        lyt.foreach_cell(
            [&lyt, &random_coordiante, &distance_violance](const auto& c1)
            {
                if (euclidean_distance<Lyt>(lyt, c1, random_coordiante) < 3)
                {
                    distance_violance = true;
                };
            });

        if (!distance_violance)
        {
            lyt.assign_cell_type(random_coordiante, Lyt::cell_type::NORMAL);
            loop_counter += 1;
        }
    }
    write_sqd_layout(lyt, path + lyt.get_layout_name());
}

}  // namespace fiction

#endif  // FICTION_GENERATED_RANDOM_LAYOUTS_HPP
