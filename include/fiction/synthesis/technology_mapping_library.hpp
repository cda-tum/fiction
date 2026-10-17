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
 * @brief Preset genlib gate definitions for mockturtle's technology mapper.
 *
 * Three-input gates follow "Three-Input Gates for Logic Synthesis" by Dewmini S. Marakkalage et al., TCAD 2020.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

namespace fiction::synthesis
{

/**
 * @brief GENLIB definition of the constant-zero function.
 */
inline constexpr const char* GATE_ZERO = "GATE  zero  0  O=CONST0;\n";
/**
 * @brief GENLIB definition of the constant-one function.
 */
inline constexpr const char* GATE_ONE = "GATE  one   0  O=CONST1;\n";

/**
 * @brief GENLIB definition of the buffer function `a`.
 */
inline constexpr const char* GATE_BUF = "GATE  buf   1 O=a;   PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the inverter function `!a`.
 */
inline constexpr const char* GATE_INV = "GATE  inv1  1 O=!a;  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the binary AND function `a*b`.
 */
inline constexpr const char* GATE_AND2 = "GATE  and2  1 O=a*b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the binary NAND function `!(a*b)`.
 */
inline constexpr const char* GATE_NAND2 = "GATE  nand2 1 O=!(a*b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the binary OR function `a+b`.
 */
inline constexpr const char* GATE_OR2 = "GATE  or2   1 O=a+b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the binary NOR function `!(a+b)`.
 */
inline constexpr const char* GATE_NOR2 = "GATE  nor2  1 O=!(a+b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the binary XOR function `a^b`.
 */
inline constexpr const char* GATE_XOR2 = "GATE  xor2  1 O=a^b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the binary XNOR function `!(a^b)`.
 */
inline constexpr const char* GATE_XNOR2 = "GATE  xnor2 1 O=!(a^b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the less-than function `!a*b`.
 */
inline constexpr const char* GATE_LT2 = "GATE  lt2  1 O=(!a*b);      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the greater-than function `a*!b`.
 */
inline constexpr const char* GATE_GT2 = "GATE  gt2  1 O=(a*!b);      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the less-than-or-equal function `!(a*!b)`.
 */
inline constexpr const char* GATE_LE2 = "GATE  le2  1 O=!(a*!b);     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB definition of the greater-than-or-equal function `!(!a*b)`.
 */
inline constexpr const char* GATE_GE2 = "GATE  ge2  1 O=!(!a*b);     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the three-input AND function `a*b*c`.
 */
inline constexpr const char* GATE_AND3 = "GATE  and3     1 O=a*b*c;  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_AND3`: binary AND gate.
 */
inline constexpr const char* DECAY_AND3 = "GATE  d1_and3  1 O=a*b;    PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the function `a*(b^c)`.
 */
inline constexpr const char* GATE_XOR_AND = "GATE  xor_and     1 O=a*(b^c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_XOR_AND`: XOR, AND, and `a*!b` gates.
 * @hideinitializer
 */
inline constexpr const char* DECAY_XOR_AND = "GATE  d1_xor_and  1 O=a^b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                             "GATE  d2_xor_and  1 O=a*b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                             "GATE  d3_xor_and  1 O=a*!b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the function `a*(b+c)`.
 */
inline constexpr const char* GATE_OR_AND = "GATE  or_and      1 O=a*(b+c);   PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_OR_AND`: OR, AND, and buffer gates.
 * @hideinitializer
 */
inline constexpr const char* DECAY_OR_AND = "GATE  d1_or_and   1 O=a+b;       PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                            "GATE  d2_or_and   1 O=a*b;       PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                            "GATE  d3_or_and   1 O=a;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the three-input one-hot function (exactly one input is true).
 */
inline constexpr const char* GATE_ONEHOT =
    "GATE  onehot     1 O=(a*!b*!c)^(!a*b*!c)^(!a*!b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_ONEHOT`: XOR and NOR gates.
 * @hideinitializer
 */
inline constexpr const char* DECAY_ONEHOT =
    "GATE  d1_onehot  1 O=a^b;                            PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
    "GATE  d2_onehot  1 O=!(a+b);                         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the three-input majority function `a*b+a*c+b*c`.
 */
inline constexpr const char* GATE_MAJ3 = "GATE  maj3     1 O=a*b+a*c+b*c;  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_MAJ3`: AND and OR gates.
 * @hideinitializer
 */
inline constexpr const char* DECAY_MAJ3 = "GATE  d1_maj3  1 O=a*b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                          "GATE  d2_maj3  1 O=a+b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the three-input equality function (all inputs are equal).
 */
inline constexpr const char* GATE_GAMBLE =
    "GATE  gamble     1 O=(a*b*c)^(!a*!b*!c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_GAMBLE`: NOR and AND gates.
 * @hideinitializer
 */
inline constexpr const char* DECAY_GAMBLE =
    "GATE  d1_gamble  1 O=!(a+b);              PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
    "GATE  d2_gamble  1 O=a*b;                 PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the function `a^(c+(a*b))`.
 */
inline constexpr const char* GATE_DOT = "GATE  dot     1 O=a^(c+(a*b));  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_DOT`: buffer, NOR, XOR, `!a*b`, `a*!b`, and inverter gates.
 * @hideinitializer
 */
inline constexpr const char* DECAY_DOT = "GATE  d1_dot  1 O=a;            PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d2_dot  1 O=!a*!b;        PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d3_dot  1 O=a^b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d4_dot  1 O=!a*b;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d5_dot  1 O=a*!b;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d6_dot  1 O=!a;           PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the multiplexer function `(a*!c)+(b*c)`.
 */
inline constexpr const char* GATE_MUX = "GATE  mux     1 O=(a*!c)+(b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_MUX`: AND, `a+!b`, `a*!b`, OR, and buffer gates.
 * @hideinitializer
 */
inline constexpr const char* DECAY_MUX = "GATE  d1_mux  1 O=a*b;           PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d2_mux  1 O=a+!b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d3_mux  1 O=a*!b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d4_mux  1 O=a+b;           PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  d5_mux  1 O=a;             PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * @brief GENLIB definition of the function `a^(b*c)`.
 */
inline constexpr const char* GATE_AND_XOR = "GATE  and_xor     1 O=a^(b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * @brief GENLIB decomposition library for `GATE_AND_XOR`: AND, `!a*b`, and XOR gates.
 * @hideinitializer
 */
inline constexpr const char* DECAY_AND_XOR = "GATE  d1_and_xor  1 O=a*b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                             "GATE  d2_and_xor  1 O=!a*b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                             "GATE  d3_and_xor  1 O=a^b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

}  // namespace fiction::synthesis
