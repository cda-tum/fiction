//
// Created by marcel on 26.04.18.
//

#include "fcn_clocking_scheme.h"


fcn_clocking_scheme::fcn_clocking_scheme(const std::string& name, const fcn_clock::cutout& c, const fcn_clock::number n, const bool r) noexcept
        :
        name(name),
        scheme(c),
        num_clocks(n),
        regular(r),
        cutout_y(scheme.size()),
        cutout_x(cutout_y ? scheme[0].size() : 0)
{}

fcn_clocking_scheme::fcn_clocking_scheme(std::string&& name, fcn_clock::cutout&& c, fcn_clock::number&& n, bool&& r) noexcept
        :
        name(std::move(name)),
        scheme(std::move(c)),
        num_clocks(std::move(n)),
        regular(std::move(r)),
        cutout_y(scheme.size()),
        cutout_x(cutout_y ? scheme[0].size() : 0)
{}

std::optional<fcn_clocking_scheme> get_clocking_scheme(const std::string& name) noexcept
{
    static const std::unordered_map<std::string, fcn_clocking_scheme> scheme_lookup
    {{
        { "OPEN3", open_3_clocking },
        { "OPEN4", open_4_clocking },
        { "OPEN", open_4_clocking },
        { "2DDWAVE3", twoddwave_3_clocking },
        { "DIAG3", twoddwave_3_clocking },
        { "2DDWAVE4", twoddwave_4_clocking },
        { "DIAG4", twoddwave_4_clocking },
        { "2DDWAVE", twoddwave_4_clocking },
        { "USE", use_4_clocking },
        { "RES", res_4_clocking },
        { "BANCS", bancs_3_clocking },
        { "TOPOLINANO3", topolinano_3_clocking },
        { "TOPOLINANO", topolinano_3_clocking },
        { "TOPOLINANO4", topolinano_4_clocking }
    }};

    if (auto it = scheme_lookup.find(boost::to_upper_copy(name)); it != scheme_lookup.end())
    {
        return it->second;
    }
    else
    {
        return std::nullopt;
    }
}