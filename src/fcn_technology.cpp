//
// Created by marcel on 09.10.18.
//

#include "fcn_technology.h"

namespace fcn
{
    std::ostream& operator<<(std::ostream& os, const technology tech)
    {
        os << to_string(tech);
        return os;
    }

    std::string to_string(const technology tech)
    {
        switch (tech)
        {
            case technology::QCA:
            {
                return "QCA";
            }
//            case technology::NML:
//            {
//                return "NML";
//            }
            default:
            {
                return "?";
            }
        }
    }
}

