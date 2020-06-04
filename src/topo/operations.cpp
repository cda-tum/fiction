//
// Created by marcel on 15.06.18.
//

#include "operations.h"

std::ostream& operator<<(std::ostream& os, const operation o) noexcept
{
    os << str(o).c_str();
    return os;
}

std::string str(const operation o) noexcept
{
    switch (o)
    {
        case operation::NONE:
            return "▢";
        case operation::AND:
            return "&";
        case operation::OR:
            return "|";
//            return "∥";
        case operation::NOT:
            return "¬";
        case operation::MAJ:
            return "M";
        case operation::F1O2:
        case operation::F1O3:
            return "Y";
        case operation::W:
            return "=";
//            return "≡";
        case operation::PI:
            return "I";
        case operation::PO:
            return "O";
        default:
            return "X";
    }
}

std::string name_str(const operation o) noexcept
{
    switch (o)
    {
        case operation::NONE:
            return "NONE";
        case operation::AND:
            return "AND";
        case operation::OR:
            return "OR";
        case operation::NOT:
            return "NOT";
        case operation::MAJ:
            return "MAJ";
        case operation::F1O2:
            return "F1O2";
        case operation::F1O3:
            return "F1O3";
        case operation::W:
            return "WIRE";
        case operation::PI:
            return "PI";
        case operation::PO:
            return "PO";
        default:
            return "X";
    }
}
