//
// Created by marcel on 15.06.18.
//

#include "directions.h"


layout::directions layout::opposite(const directions d) noexcept
{
    return d << 2 | d >> 2;
}
