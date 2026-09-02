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
 * @brief CMake probe that checks whether a usable Z3 installation is present.
 * @author Marcel Walter (marcelwa)
 */

#include <z3.h>

#include <iostream>

int main()
{
    std::cout << Z3_get_full_version();
    return 0;
}
