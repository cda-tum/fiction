//
// Created by gregor on 12/30/18.
//

#include "svg_writer.h"

namespace svg
{
    std::string generate_svg_string(fcn_cell_layout_ptr fcl, bool simple)
    {
        // Collects ALL tile-descriptions
        std::stringstream tile_descriptions{};

        // Used for generating tile-descriptions with information about the tile's coordinates and clockzone
        // It is needed because cells may not be in "tile-order" when read from cell-layout
        coord_to_tile_mapping coord_to_tile{};
        coord_to_cell_list_mapping coord_to_cells{};
        coord_to_latch_mapping coord_to_latch_tile{};
        coord_to_cell_list_mapping coord_to_latch_cells{};

        // Used to determine the color of cells, tiles and text based on its clockzone
        std::vector<std::string>
            cell_colors{{clockzone_1_cell, clockzone_2_cell, clockzone_3_cell, clockzone_4_cell}},
            tile_colors{{clockzone_1_tile, clockzone_2_tile, clockzone_3_tile, clockzone_4_tile}},
            text_colors{{clockzone_12_text, clockzone_12_text, clockzone_34_text, clockzone_34_text}};

        // Adds all non-empty cells from the layout to their correct tiles; it generates the "body"
        // of all the tile-descriptions to be used later
        for (const auto& c : fcl->cells() | iter::filterfalse([fcl](const fcn_cell_layout::cell& _c)
                                                              { return fcl->is_free_cell(_c); }))
        {
            std::size_t clockzone = *fcl->cell_clocking(c);
            auto tile_coords = std::make_pair(static_cast<std::size_t>(ceil(c[X] / fcl->get_library()->gate_x_size())),
                                              static_cast<std::size_t>(ceil(c[Y] / fcl->get_library()->gate_y_size())));

            std::size_t latch_delay = fcl->get_latch(c);
            std::string current_cells;

            if (latch_delay)
            {
                try
                {
                    current_cells = coord_to_latch_cells.at(tile_coords);
                }
                catch (...)
                {
                    // If this is called, then there is no tile for the current cell yet
                    // It also makes sure that all required tiles are created
                    coord_to_latch_tile[tile_coords] = std::make_tuple(latch, clockzone, latch_delay);
                }
            }
            else
            {
                try
                {
                    current_cells = coord_to_cells.at(tile_coords);
                }
                catch (...)
                {
                    // If this is called, then there is no tile for the current cell yet
                    // It also makes sure that all required tiles are created
                    coord_to_tile[tile_coords] = std::make_pair(tile, clockzone);
                }
            }

            // Determines cell type and color
            std::string cell_description, cell_color;
            switch (fcl->get_cell_type(c))
            {
                case fcn::EMPTY_CELL:
                    break;
                case fcn::NORMAL_CELL:
                {
                    cell_color = latch_delay ? clockzone_latch_cell : cell_colors[clockzone];
                    if (simple)
                    {
                        cell_description = simple_cell;
                    }
                    else if (cell[Z] != 0 && fcl->get_cell_mode(c) == fcn::cell_mode::VERTICAL)
                    {
                        cell_description = via;
                    }
                    else if ((c[Z] == 0 && !fcl->is_free_cell(fcl->above(c))) ||
                             (c[Z] != 0 && fcl->is_free_cell(fcl->below(c))))
                    {
                        cell_description = cross;
                    }
                    else if (c[Z] != 0)
                    {
                        break;
                    }
                    else
                    {
                        cell_description = cell;
                    }
                    break;
                }
                case fcn::INPUT_CELL:
                {
                    cell_color = pi_cell;
                    cell_description = simple ? simple_cell : cell;
                    break;
                }
                case fcn::OUTPUT_CELL:
                {
                    cell_color = po_cell;
                    cell_description = simple ? simple_cell : cell;
                    break;
                }
                case fcn::CONST_0_CELL:
                {
                    cell_color = "#000000";
                    cell_description = simple ? simple_cell : const0;
                    break;
                }
                case fcn::CONST_1_CELL:
                {
                    cell_color = "#000000";
                    cell_description = simple ? simple_cell : const1;
                    break;
                }
                default:
                {
                    throw std::invalid_argument("unsupported cell type(s) occurred in layout");
                }
            }


            // Represent the x- and y-coordinates inside the c's tile
            std::size_t in_tile_x = c[X] % fcl->get_library()->gate_x_size();
            std::size_t in_tile_y = c[Y] % fcl->get_library()->gate_y_size();

            // Current cell-description can now be appended to the description of all cells in the current tile
            if (latch_delay)
            {
                coord_to_latch_cells[tile_coords] = current_cells.append(
                        fmt::format(cell_description, cell_color,
                                    starting_offset_latch_cell_x + in_tile_x * cell_distance,
                                    starting_offset_latch_cell_y + in_tile_y * cell_distance));

            }
            else
            {
                coord_to_cells[tile_coords] = current_cells.append(
                        fmt::format(cell_description, cell_color,
                                    starting_offset_cell_x + in_tile_x * cell_distance,
                                    starting_offset_cell_y + in_tile_y * cell_distance));
            }
        }

        // All cell-descriptions are done and tiles have been created
        // Associate tiles with cell-descriptions now; coordinates of tiles are used for tile- and cell-descriptions
        for (const auto& ct : coord_to_tile)
        {
            auto coord = ct.first;
            auto descr = ct.second.first;
            auto czone = ct.second.second;

            auto cell_descriptions = coord_to_cells.at(coord);

            double x_pos = starting_offset_tile_x + coord.first  * tile_distance;
            double y_pos = starting_offset_tile_y + coord.second * tile_distance;

            descr = fmt::format(descr, x_pos, y_pos, tile_colors[czone], cell_descriptions,
                                simple ? "" : text_colors[czone],
                                simple ? "" : std::to_string(czone + 1));

            tile_descriptions << descr;
        }

        // Add the descriptions of latch-tiles to the whole image
        for (const auto& ct : coord_to_latch_tile)
        {
            auto coord = ct.first;
            auto descr = std::get<0>(ct.second);
            auto czone_up = std::get<1>(ct.second);
            auto latch_delay = std::get<2>(ct.second);
            auto czone_lo = czone_up + latch_delay % fcl->num_clocks();

            auto cell_descriptions = coord_to_latch_cells.at(coord);

            double x_pos = starting_offset_latch_x + coord.first * tile_distance;
            double y_pos = starting_offset_latch_y + coord.second * tile_distance;

            descr = fmt::format(descr, x_pos, y_pos, tile_colors[czone_lo], tile_colors[czone_up], cell_descriptions,
                                text_colors[czone_up], simple ? "" : std::to_string(czone_up + 1),
                                text_colors[czone_lo],
                                simple ? "" : std::to_string(czone_lo + 1));

            tile_descriptions << descr;
        }

        std::size_t length_x = fcl->x() / fcl->get_library()->gate_x_size();
        std::size_t length_y = fcl->y() / fcl->get_library()->gate_y_size();

        double viewbox_x = 2 * viewbox_distance + length_x * tile_distance;
        double viewbox_y = 2 * viewbox_distance + length_y * tile_distance;

        return fmt::format(header, boost::lexical_cast<std::string>(viewbox_x),
                                   boost::lexical_cast<std::string>(viewbox_y), tile_descriptions.str());
    }
}
