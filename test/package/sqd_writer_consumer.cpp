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
 * @brief Checks SiQAD simulation output through an installed fiction package.
 */

#include <fiction/technology/sidb/simulation/io/write_sqd_sim_result.hpp>

#include <sstream>
#include <string>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::io;

/**
 * @brief Write escaped metadata through the installed package.
 * @return Zero when the writer emits the expected metadata and document end.
 */
int main()
{
    result sim_result{};
    sim_result.algorithm_name = "&<>\r";
    sim_result.additional_simulation_parameters.emplace("label", std::string{"&<>\r"});
    std::ostringstream stream{};
    write_sqd_sim_result(sim_result, stream);
    const auto document = stream.str();
    return document.find("<engine>&amp;&lt;&gt;&#13;</engine>") != std::string::npos &&
                   document.find("<label>&amp;&lt;&gt;&#13;</label>") != std::string::npos &&
                   document.ends_with("</sim_out>\n") ?
               0 :
               1;
}
