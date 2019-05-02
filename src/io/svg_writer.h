//
// Created by gregor on 12/30/18.
//

#ifndef FICTION_SVG_WRITER_H
#define FICTION_SVG_WRITER_H

#include "fcn_cell_layout.h"
#include <iostream>
#include <math.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include <fmt/format.h>
#include <itertools.hpp>

namespace svg
{
    // General layout
    constexpr const double viewbox_distance = 11;
    constexpr const double tile_distance = 119;
    constexpr const double cell_distance = 23;

    // Generic tiles
    constexpr const int starting_offset_tile_x = -175;
    constexpr const int starting_offset_tile_y = -58;
    constexpr const double starting_offset_cell_x = 3.5;
    constexpr const double starting_offset_cell_y = -40.25;

    // Latch tiles
    constexpr const double starting_offset_latch_x = -483.5;
    constexpr const int starting_offset_latch_y = -57;
    constexpr const int starting_offset_latch_cell_x = 312;
    constexpr const int starting_offset_latch_cell_y = -41;

    // Colors
    constexpr const char *clockzone_1_cell = "86e291";
    constexpr const char *clockzone_2_cell = "ffa5fa";
    constexpr const char *clockzone_3_cell = "00c8bc";
    constexpr const char *clockzone_4_cell = "ffffff";
    constexpr const char *clockzone_latch_cell = "ffe33a";
    constexpr const char *clockzone_1_tile = "ffffff";
    constexpr const char *clockzone_2_tile = "bfbfbf";
    constexpr const char *clockzone_3_tile = "7f7f7f";
    constexpr const char *clockzone_4_tile = "3f3f3f";
    constexpr const char *clockzone_12_text = "000000";
    constexpr const char *clockzone_34_text = "ffffff";
    constexpr const char *pi_cell = "008dc8";
    constexpr const char *po_cell = "e28686";

    // SVG parts with placeholders
    constexpr const char *header = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
                                   "<!-- Created by fiction (https://github.com/marcelwa/fiction) -->\n"
                                   "\n"
                                   "<svg\n"
                                   "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
                                   "xmlns:cc=\"http://creativecommons.org/ns#\"\n"
                                   "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
                                   "xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
                                   "xmlns=\"http://www.w3.org/2000/svg\"\n"
                                   "viewBox=\"0 0 {} {}\"\n"
                                   "version=\"1.1\">\n"
                                   "<metadata>\n"
                                   "<rdf:RDF>\n"
                                   "<cc:Work\n"
                                   "rdf:about=\"\">\n"
                                   "<dc:format>image/svg+xml</dc:format>\n"
                                   "<dc:type\n"
                                   "rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n"
                                   "<dc:title></dc:title>\n"
                                   "</cc:Work>\n"
                                   "</rdf:RDF>\n"
                                   "</metadata>\n"
                                   "<g>\n"
                                   "{}\n"
                                   "</g>\n"
                                   "</svg>";

    constexpr const char *tile = "<g\n"
                                 "transform=\"translate({0},{1})\">\n"
                                 "<rect\n"
                                 "ry=\"1.4357216\"\n"
                                 "y=\"69.390663\"\n"
                                 "x=\"186.11467\"\n"
                                 "height=\"118.80357\"\n"
                                 "width=\"118.80357\"\n"
                                 "style=\"fill:#{2};stroke:#000000;\" />\n"
                                 "{3}\n"
                                 "<text\n"
                                 "y=\"179.25412\"\n"
                                 "x=\"288.74234\"\n"
                                 "style=\"font-style:normal;font-weight:normal;font-size:12.5px;line-height:125%;font-family:'Noto Sans';fill:#{4};stroke:none;\"\n"
                                 "xml:space=\"preserve\"><tspan\n"
                                 "y=\"179.25412\"\n"
                                 "x=\"288.74234\">{5}</tspan></text>\n"
                                 "</g>";

    constexpr const char *latch = "<g\n"
                                  "transform=\"translate({},{})\">\n"
                                  "<g>\n"
                                  "<g>\n"
                                  "<path\n"
                                  "d=\"m 613.38686,68.413109 0,118.803571 -118.80358,0\"\n"
                                  "style=\"color:#000000;solid-color:#000000;solid-opacity:1;fill:#{};fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:1.19643486;stroke-opacity:1;enable-background:accumulate\" />\n"
                                  "<path\n"
                                  "d=\"m 494.58328,187.21668 0,-118.80357 118.80358,0\"\n"
                                  "style=\"color:#000000;solid-color:#000000;solid-opacity:1;fill:#{};fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:1.19643486;stroke-opacity:1;enable-background:accumulate\" />\n"
                                  "</g>\n"
                                  "<rect\n"
                                  "style=\"color:#000000;solid-color:#000000;solid-opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:1.19643486;stroke-opacity:1;enable-background:accumulate\"\n"
                                  "width=\"118.80357\"\n"
                                  "height=\"118.80357\"\n"
                                  "x=\"494.58328\"\n"
                                  "y=\"68.413109\"\n"
                                  "ry=\"1.4357216\" />\n"
                                  "</g>\n"
                                  "{}\n"
                                  "<text\n"
                                  "y=\"86.340652\"\n"
                                  "x=\"504.43588\"\n"
                                  "style=\"font-style:normal;font-weight:normal;font-size:12.5px;line-height:125%;font-family:'Noto Sans';letter-spacing:0px;word-spacing:0px;fill:#{};fill-opacity:1;\"\n"
                                  "xml:space=\"preserve\"><tspan\n"
                                  "y=\"86.340652\"\n"
                                  "x=\"504.43588\">{}</tspan></text>\n"
                                  "<text\n"
                                  "y=\"178.27962\"\n"
                                  "x=\"596.48468\"\n"
                                  "style=\"font-style:normal;font-weight:normal;font-size:12.5px;line-height:125%;font-family:'Noto Sans';letter-spacing:0px;word-spacing:0px;fill:#{};fill-opacity:1;\"\n"
                                  "xml:space=\"preserve\"><tspan\n"
                                  "y=\"178.27962\"\n"
                                  "x=\"596.48468\">{}</tspan></text>\n"
                                  "</g>";


    constexpr const char *cell = "<g\n"
                                 "style=\"fill:#{0};\"\n"
                                 "transform=\"translate({1},{2})\">\n"
                                 "<rect\n"
                                 "style=\"fill:#{0};stroke:#000000;\"\n"
                                 "width=\"20\"\n"
                                 "height=\"20\"\n"
                                 "x=\"185.86807\"\n"
                                 "y=\"112.92032\"\n"
                                 "ry=\"1.5\" />\n"
                                 "<g\n"
                                 "style=\"fill:#{0};\"\n"
                                 "transform=\"translate(-0.22642517,-0.09405518)\">\n"
                                 "<circle\n"
                                 "r=\"2.7314816\"\n"
                                 "cx=\"191.0945\"\n"
                                 "cy=\"118.01437\"\n"
                                 "style=\"opacity:1;fill:#{0};stroke:#000000;\" />\n"
                                 "<circle\n"
                                 "r=\"2.7314816\"\n"
                                 "cx=\"191.0945\"\n"
                                 "cy=\"128.01437\"\n"
                                 "style=\"opacity:1;fill:#{0};stroke:#000000;\" />\n"
                                 "<circle\n"
                                 "r=\"2.7314816\"\n"
                                 "cx=\"201.0945\"\n"
                                 "cy=\"128.01437\"\n"
                                 "style=\"opacity:1;fill:#{0};stroke:#000000;\" />\n"
                                 "<circle\n"
                                 "r=\"2.7314816\"\n"
                                 "cx=\"201.0945\"\n"
                                 "cy=\"118.01437\"\n"
                                 "style=\"opacity:1;fill:#{0};stroke:#000000;\" />\n"
                                 "</g>\n"
                                 "</g>";

    constexpr const char *const0 = "<g\n"
                                   "style=\"fill:#000000;fill-opacity:1\"\n"
                                   "transform=\"translate({1},{2})\">\n"
                                   "<rect\n"
                                   "style=\"fill:#000000;stroke:#000000;\"\n"
                                   "width=\"20\"\n"
                                   "height=\"20\"\n"
                                   "x=\"185.86807\"\n"
                                   "y=\"112.92032\"\n"
                                   "ry=\"1.5\" />\n"
                                   "<g\n"
                                   "style=\"fill:#000000;fill-opacity:1\"\n"
                                   "transform=\"translate(-0.22642517,-0.09405518)\">\n"
                                   "<circle\n"
                                   "r=\"2.7314816\"\n"
                                   "cx=\"191.0945\"\n"
                                   "cy=\"118.01437\"\n"
                                   "style=\"fill:#ffffff;stroke:#ffffff;\" />\n"
                                   "<circle\n"
                                   "r=\"2.7314816\"\n"
                                   "cx=\"201.0945\"\n"
                                   "cy=\"128.01437\"\n"
                                   "style=\"fill:#ffffff;stroke:#ffffff;\" />\n"
                                   "</g>\n"
                                   "</g>";

    constexpr const char *const1 = "<g\n"
                                   "style=\"fill:#000000;\"\n"
                                   "transform=\"translate({1},{2})\">\n"
                                   "<rect\n"
                                   "style=\"fill:#000000;stroke:#000000;\"\n"
                                   "width=\"20\"\n"
                                   "height=\"20\"\n"
                                   "x=\"185.86807\"\n"
                                   "y=\"112.92032\"\n"
                                   "ry=\"1.5\" />\n"
                                   "<g\n"
                                   "style=\"fill:#000000;\"\n"
                                   "transform=\"translate(-0.22642517,-0.09405518)\">\n"
                                   "<circle\n"
                                   "r=\"2.7314816\"\n"
                                   "cx=\"191.0945\"\n"
                                   "cy=\"128.01437\"\n"
                                   "style=\"fill:#ffffff;stroke:#ffffff;\" />\n"
                                   "<circle\n"
                                   "r=\"2.7314816\"\n"
                                   "cx=\"201.0945\"\n"
                                   "cy=\"118.01437\"\n"
                                   "style=\"fill:#ffffff;stroke:#ffffff;\" />\n"
                                   "</g>\n"
                                   "</g>";

    constexpr const char *via = "<g\n"
                                "style=\"fill:#{0};\"\n"
                                "transform=\"translate({1},{2})\">\n"
                                "<g>\n"
                                "<rect\n"
                                "transform=\"rotate(90)\"\n"
                                "ry=\"1.5\"\n"
                                "y=\"-205.86807\"\n"
                                "x=\"112.92032\"\n"
                                "height=\"20\"\n"
                                "width=\"20\"\n"
                                "style=\"fill:#{0};stroke:#000000;\" />\n"
                                "<circle\n"
                                "r=\"7.446404\"\n"
                                "transform=\"rotate(90)\"\n"
                                "cx=\"122.92032\"\n"
                                "cy=\"-195.86807\"\n"
                                "style=\"fill:none;stroke:#000000;\" />\n"
                                "</g>\n"
                                "</g>";

    constexpr const char *cross = "<g\n"
                                  "style=\"fill:#{0};fill-opacity:1\"\n"
                                  "transform=\"translate({1},{2})\">\n"
                                  "<g>\n"
                                  "<rect\n"
                                  "transform=\"rotate(90)\"\n"
                                  "ry=\"1.5\"\n"
                                  "y=\"-205.86807\"\n"
                                  "x=\"112.92032\"\n"
                                  "height=\"20\"\n"
                                  "width=\"20\"\n"
                                  "style=\"fill:#{0};stroke:#000000;\" />\n"
                                  "<g\n"
                                  "style=\"stroke-width:1.05832684;stroke-miterlimit:4;stroke-dasharray:none\"\n"
                                  "transform=\"matrix(0.94695856,0,0,0.94341063,31.332271,8.4394994)\">\n"
                                  "<path\n"
                                  "style=\"stroke:#000000;\"\n"
                                  "d=\"m 165.8261,113.42213 c 15.83125,15.8311 15.8515,15.85136 15.8515,15.85136\"/>\n"
                                  "<path\n"
                                  "style=\"stroke:#000000;\"\n"
                                  "d=\"m 181.6776,113.42213 c -15.83124,15.83111 -15.85149,15.85137 -15.85149,15.85137\"/>\n"
                                  "</g>\n"
                                  "</g>\n"
                                  "</g>";

    constexpr const char *simple_cell = "<g\n"
                                        "transform=\"translate({1},{2})\">\n"
                                        "<rect\n"
                                        "ry=\"1.5\"\n"
                                        "y=\"112.92032\"\n"
                                        "x=\"185.86807\"\n"
                                        "height=\"20\"\n"
                                        "width=\"20\"\n"
                                        "style=\"fill:#{0};stroke:#000000;\" />\n"
                                        "</g>";


    /**
     * Alias for a tile coordinate.
     */
    using coordinate = std::pair<std::size_t, std::size_t>;
    /**
     * Alias for an SVG description of a tile containing also its clockzone.
     */
    using tile_description = std::pair<std::string, std::size_t>;
    /**
     * Maps coordinates of tiles to their string representation and their clockzone.
     */
    using coord_to_tile_mapping = std::unordered_map<coordinate, tile_description, boost::hash<coordinate>>;
    /**
     * Maps coordinates of tiles to a string representation of the cells contained within them.
     */
    using coord_to_cell_list_mapping = std::unordered_map<coordinate, std::string, boost::hash<coordinate>>;
    /**
     * Alias for an SVG description of a latch tile containing also its clockzone and its offset.
     */
    using svg_latch = std::tuple<std::string, std::size_t, std::size_t>;
    /**
     * Maps coordinates of latch tiles to tuples containing their string representation, their clockzone,
     * and their latch offset.
     */
    using coord_to_latch_mapping = std::unordered_map<coordinate, svg_latch, boost::hash<coordinate>>;

    /**
     * Returns an SVG string representing the given cell layout.
     *
     * Currently, this works for tile-based 4-phase QCA cell layouts generated from a 5 x 5 gate library.
     * TODO make this generic for cell-based designs
     *
     * @param fcl The cell layout to generate an SVG representation for.
     * @param simple Flag to indicate that the SVG representation should be generated with less details. Recommended
     *               for large layouts.
     * @return The SVG string containing a visual representation of the given layout.
     */
    std::string generate_svg_string(fcn_cell_layout_ptr fcl, bool simple);
}

#endif //FICTION_SVG_WRITER_H
