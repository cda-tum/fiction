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
 * @brief Physical constants of the silicon surface the SiDB models are built on.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

namespace fiction::sidb::model
{

/**
 * The elementary charge \f$e\f$ in \f$C\f$.
 */
inline constexpr double ELEMENTARY_CHARGE = 1.6021766 * 1E-19;
/**
 * The Coulomb constant \f$k\f$ in \f$N \cdot m^{2} \cdot C^{-2}\f$.
 */
inline constexpr double K_E = 8.987552 * 1E9;
/**
 * The Boltzmann constant \f$k_B\f$ in \f$J \cdot K^{-1}\f$.
 */
inline constexpr double BOLTZMANN_CONSTANT = 1.38064852 * 1E-23;
/**
 * The conversion factor from electronvolts to joules.
 */
inline constexpr double EV_TO_JOULE = ELEMENTARY_CHARGE;

}  // namespace fiction::sidb::model
