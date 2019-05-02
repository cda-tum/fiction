//
// Created by marcel on 26.04.18.
//

#include "fcn_clocking_scheme.h"


fcn_clocking_scheme::fcn_clocking_scheme(const std::string& name, const fcn_clock::cutout& c, const fcn_clock::number n, const bool r)
        :
        name(name),
        scheme(c),
        num_clocks(n),
        regular(r),
        cutout_y(scheme.size()),
        cutout_x(cutout_y ? scheme[0].size() : 0)
{}

fcn_clocking_scheme::fcn_clocking_scheme(std::string&& name, fcn_clock::cutout&& c, fcn_clock::number&& n, bool&& r)
        :
        name(std::move(name)),
        scheme(std::move(c)),
        num_clocks(std::move(n)),
        regular(std::move(r)),
        cutout_y(scheme.size()),
        cutout_x(cutout_y ? scheme[0].size() : 0)
{}

boost::optional<fcn_clocking_scheme> get_clocking_scheme(const std::string& name)
{
    static const std::unordered_map<std::string, fcn_clocking_scheme> scheme_lookup
    {{
        { "OPEN3", open_3_clocking },
        { "OPEN4", open_4_clocking },
        { "2DDWAVE3", twoddwave_3_clocking },
        { "DIAG3", twoddwave_3_clocking },
        { "2DDWAVE4", twoddwave_4_clocking },
        { "DIAG4", twoddwave_4_clocking },
        { "USE", use_4_clocking },
        { "RES", res_4_clocking },
        { "BANCS", bancs_3_clocking },
    }};

    try
    {
        return scheme_lookup.at(boost::to_upper_copy(name));
    }
    catch (...)
    {
        return boost::none;
    }
}