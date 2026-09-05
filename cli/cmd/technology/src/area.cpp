/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Implements the `area` command.
 * @author Marcel Walter (marcelwa)
 */

#include "cmd/technology/include/area.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/technology/fcn/area.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <type_traits>
#include <variant>

namespace alice
{

area_command::area_command(const environment::ptr& e) :
        command(e, "Prints the area usage (in nm²) of the current cell layout in store. Dimensions for each "
                   "cell use default values from QCADesigner (QCA), NMLSim (iNML), or SiQAD (SiDB) if no value is "
                   "provided.")
{
    add_option("--width,-x", width, "Cell width in nm");
    add_option("--height,-y", height, "Cell height in nm");
    add_option("--hspace", hspace, "Horizontal cell spacing in nm");
    add_option("--vspace", vspace, "Vertical cell spacing in nm");
}

void area_command::execute()
{
    // reset area
    st = {};

    auto& s = store<fiction::cell_layout_t>();

    // error case: empty cell layout store
    if (s.empty())
    {
        env->out() << "[w] no cell layout in store\n";
        return;
    }

    auto lyt = s.current();

    const auto calculate_area = [this](auto&& lyt_ptr)
    {
        using Lyt  = typename std::decay_t<decltype(lyt_ptr)>::element_type;
        using tech = typename fiction::cli::store_technology<Lyt>::type;

        const double width_nm  = is_set("width") ? width : tech::CELL_WIDTH;
        const double height_nm = is_set("height") ? height : tech::CELL_HEIGHT;
        const double hspace_nm = is_set("hspace") ? hspace : tech::CELL_HSPACE;
        const double vspace_nm = is_set("vspace") ? vspace : tech::CELL_VSPACE;

        const fiction::fcn::area_params<tech> ps{width_nm, height_nm, hspace_nm, vspace_nm};

        if constexpr (fiction::cli::is_sidb_store_v<Lyt>)
        {
            st.area = fiction::fcn::area(fiction::cli::sidb_layout_of(*lyt_ptr), ps, &st);
        }
        else
        {
            st.area = fiction::fcn::area(*lyt_ptr, ps, &st);
        }
    };

    std::visit(calculate_area, lyt);

    st.report(env->out());
}

nlohmann::json area_command::log() const
{
    return {{"width (nm)", st.width}, {"height (nm)", st.height}, {"area (nm²)", st.area}};
}

}  // namespace alice
