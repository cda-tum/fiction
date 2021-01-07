//
// Created by gregor on 12/30/18.
//

#include "svg_writer.h"

namespace svg
{
    std::string generate_svg_string(fcn_cell_layout_ptr fcl, bool simple)
    {
        // The existence of a library implies the utilization of a regular clocking scheme. Vice versa, the existence of
        // a regular clocking implies that there has to be a library assigned to the layout. A scheme itself does not
        // give enough information about some cell's clocking because it is dependent on its position within a tile.
        // Therefore, the zones can only be deterministically assigned to a circuit if their sizes are known. The tile
        // size is only given in gate-libraries. Consequently, there has to be one assigned in order to enable for
        // unambiguous clock zone mapping.
        if(fcl->get_library() == nullptr)
            return generate_cell_based_svg(fcl, simple);
        else
            return generate_tile_based_svg(fcl, simple);
    }

    std::string generate_tile_based_svg(fcn_cell_layout_ptr fcl, bool simple)
    {
        // Collects ALL tile-descriptions
        std::stringstream tile_descriptions{};

        // Used for generating tile-descriptions with information about the tile's coordinates and clock zone
        // It is needed because cells may not be in "tile-order" when read from a cell layout
        coord_to_tile_mapping coord_to_tile{};
        coord_to_cell_list_mapping coord_to_cells{};
        coord_to_latch_mapping coord_to_latch_tile{};
        coord_to_cell_list_mapping coord_to_latch_cells{};

        // Used to determine the color of cells, tiles and text based on its clock zone
        static const std::vector<std::string>
            tile_colors{{clock_zone_1_tile, clock_zone_2_tile, clock_zone_3_tile, clock_zone_4_tile}},
            text_colors{{clock_zone_12_text, clock_zone_12_text, clock_zone_34_text, clock_zone_34_text}};

        // Adds all non-empty cells from the layout to their correct tiles; it generates the "body"
        // of all the tile-descriptions to be used later
        for (const auto& c : fcl->cells() | iter::filterfalse([fcl](const fcn_cell_layout::cell& _c)
                                                              { return fcl->is_free_cell(_c); }))
        {
            auto clock_zone = *fcl->cell_clocking(c);
            auto tile_coords = std::make_pair(static_cast<coord_t>(ceil(c[X] / fcl->get_library()->gate_x_size())),
                                              static_cast<coord_t>(ceil(c[Y] / fcl->get_library()->gate_y_size())));

            auto latch_delay = fcl->get_latch(c);
            std::string current_cells;

            if (latch_delay)
            {
                if (auto latch_it = coord_to_latch_cells.find(tile_coords); latch_it != coord_to_latch_cells.end())
                {
                    current_cells = latch_it->second;
                }
                else
                {
                    // If this is called, then there is no tile for the current cell yet
                    // It also makes sure that all required tiles are created
                    coord_to_latch_tile[tile_coords] = {latch, clock_zone, latch_delay};
                }

            }
            else
            {
                if (auto cell_it = coord_to_cells.find(tile_coords); cell_it != coord_to_cells.end())
                {
                    current_cells = cell_it->second;
                }
                else
                {
                    // If this is called, then there is no tile for the current cell yet
                    // It also makes sure that all required tiles are created
                    coord_to_tile[tile_coords] = {tile, clock_zone};
                }
            }


            // Represent the x- and y-coordinates inside the c's tile
            coord_t in_tile_x = c[X] % fcl->get_library()->gate_x_size();
            coord_t in_tile_y = c[Y] % fcl->get_library()->gate_y_size();

            // Determines cell type and color
            auto desc_col = generate_description_color(fcl, c, simple);

            // Current cell-description can now be appended to the description of all cells in the current tile
            if (latch_delay)
            {
                coord_to_latch_cells[tile_coords] = current_cells.append(
                        fmt::format(desc_col.first, desc_col.second,
                                    starting_offset_latch_cell_x + in_tile_x * cell_distance,
                                    starting_offset_latch_cell_y + in_tile_y * cell_distance));

            }
            else
            {
                coord_to_cells[tile_coords] = current_cells.append(
                        fmt::format(desc_col.first, desc_col.second,
                                    starting_offset_cell_x + in_tile_x * cell_distance,
                                    starting_offset_cell_y + in_tile_y * cell_distance));
            }
        }

        // All cell-descriptions are done and tiles have been created
        // Associate tiles with cell-descriptions now; coordinates of tiles are used for tile- and cell-descriptions
        for (const auto& [coord, tdscr] : coord_to_tile)
        {
            auto [x, y] = coord;
            auto [descr, czone] = tdscr;

            auto cell_descriptions = coord_to_cells[coord];

            double x_pos = starting_offset_tile_x + x * tile_distance;
            double y_pos = starting_offset_tile_y + y * tile_distance;

            auto c_descr = fmt::format(descr, x_pos, y_pos, tile_colors[czone], cell_descriptions,
                    simple ? "" : text_colors[czone],
                    simple ? "" : std::to_string(czone + 1));

            tile_descriptions << c_descr;
        }

        // Add the descriptions of latch-tiles to the whole image
        for (const auto& [coord, ldscr] : coord_to_latch_tile)
        {
            auto [x, y] = coord;
            auto [descr, czone_up, latch_delay] = ldscr;
            auto czone_lo = czone_up + latch_delay % fcl->num_clocks();

            auto cell_descriptions = coord_to_latch_cells[coord];

            double x_pos = starting_offset_latch_x + x * tile_distance;
            double y_pos = starting_offset_latch_y + y * tile_distance;

            auto t_descr = fmt::format(descr, x_pos, y_pos, tile_colors[czone_lo], tile_colors[czone_up], cell_descriptions,
                                text_colors[czone_up], simple ? "" : std::to_string(czone_up + 1),
                                text_colors[czone_lo],
                                simple ? "" : std::to_string(czone_lo + 1));

            tile_descriptions << t_descr;
        }

        coord_t length_x = fcl->x() / fcl->get_library()->gate_x_size();
        coord_t length_y = fcl->y() / fcl->get_library()->gate_y_size();

        double viewbox_x = 2 * viewbox_distance + length_x * tile_distance;
        double viewbox_y = 2 * viewbox_distance + length_y * tile_distance;

        return fmt::format(header, fiction::VERSION, fiction::REPO, boost::lexical_cast<std::string>(viewbox_x),
                                   boost::lexical_cast<std::string>(viewbox_y), tile_descriptions.str());
    }

    std::string generate_cell_based_svg(fcn_cell_layout_ptr fcl, bool simple)
    {
        std::stringstream cell_descriptions{};

        static const std::vector<std::string> cell_colors{{clock_zone_1_cell, clock_zone_2_cell, clock_zone_3_cell, clock_zone_4_cell}};

        for (const auto& c : fcl->cells() | iter::filterfalse([fcl](const fcn_cell_layout::cell& _c)
                                                              { return fcl->is_free_cell(_c); }))
        {

            // Determines cell type and color
            auto desc_col = generate_description_color(fcl, c, simple);

            // Current cell-description can now be appended to the description of all cells
            if (fcl->get_latch(c))
            {
                cell_descriptions << fmt::format(desc_col.first, desc_col.second,
                        starting_offset_tile_x + starting_offset_latch_cell_x + c[X] * cell_distance,
                        starting_offset_tile_y + starting_offset_latch_cell_y + c[Y] * cell_distance);
            }
            else
            {
                cell_descriptions << fmt::format(desc_col.first, desc_col.second,
                        starting_offset_tile_x + starting_offset_cell_x + c[X] * cell_distance,
                        starting_offset_tile_y + starting_offset_cell_y + c[Y] * cell_distance);
            }
        }

        double viewbox_x = 2 * viewbox_distance + fcl->x() * cell_distance;
        double viewbox_y = 2 * viewbox_distance + fcl->y() * cell_distance;

        return fmt::format(header, fiction::VERSION, fiction::REPO, boost::lexical_cast<std::string>(viewbox_x),
                                   boost::lexical_cast<std::string>(viewbox_y), cell_descriptions.str());
    }

    std::pair<std::string, std::string> generate_description_color(fcn_cell_layout_ptr fcl, const fcn_cell_layout::cell& c, bool simple)
    {
        std::string cell_description, cell_color{};
        static const std::vector<std::string> cell_colors{{clock_zone_1_cell, clock_zone_2_cell, clock_zone_3_cell, clock_zone_4_cell}};

        switch (fcl->get_cell_type(c))
        {
            case fcn::EMPTY_CELL:
            {
                break;
            }
            case fcn::NORMAL_CELL:
            {
                cell_color = fcl->get_latch(c) ? clock_zone_latch_cell : cell_colors[*fcl->cell_clocking(c)];
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
            case fcn::qca::CONST_0_CELL:
            {
                cell_color = "#000000";
                cell_description = simple ? simple_cell : const0;
                break;
            }
            case fcn::qca::CONST_1_CELL:
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

        return std::make_pair(cell_description, cell_color);
    }
}
