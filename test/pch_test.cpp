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
 * @brief Translation unit that builds the test suite's pre-compiled header.
 * @author Marcel Walter (marcelwa)
 */

//
// Carrier translation unit for the test suite's shared pre-compiled header. The 118 test executables
// reuse the PCH built for this target instead of each building their own.
//

#include "pch_test.hpp"  // NOLINT(misc-include-cleaner): the point of this translation unit
