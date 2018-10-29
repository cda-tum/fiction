//
// Created by marcel on 15.06.18.
//

#include "operations.h"

std::ostream& operator<<(std::ostream& os, const operation o) noexcept
{
    os << str(o);
    return os;
}

std::string str(const operation o) noexcept
{
    switch(o)
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
            return "Y";
        case operation::W:
            return "=";
//            return "≡";
        case operation::PI:
            return "I";
        case operation::PO:
            return "O";
        case operation::ONE:
            return "1";
        case operation::ZERO:
            return "0";
        case operation::BUF:
            return "B";
        default:
            return "X";
    }
}
