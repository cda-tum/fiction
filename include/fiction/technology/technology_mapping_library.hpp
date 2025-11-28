//
// Created by marcel on 08.06.21.
//

#ifndef FICTION_TECHNOLOGY_MAPPING_LIBRARY_HPP
#define FICTION_TECHNOLOGY_MAPPING_LIBRARY_HPP

namespace fiction
{

/**
 * This file provides preset gates for mockturtle's technology mapper that are parsable by lorina's genlib parser.
 */

/**
 * 0-ary functions.
 */
inline constexpr auto GATE_ZERO = "GATE  zero  0  O=CONST0;\n";
inline constexpr auto GATE_ONE  = "GATE  one   0  O=CONST1;\n";

/**
 * 1-ary functions.
 */
inline constexpr auto GATE_BUF = "GATE  buf   1 O=a;   PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_INV = "GATE  inv1  1 O=!a;  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * 2-ary functions.
 */
inline constexpr auto GATE_AND2  = "GATE  and2  1 O=a*b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_NAND2 = "GATE  nand2 1 O=!(a*b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_OR2   = "GATE  or2   1 O=a+b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_NOR2  = "GATE  nor2  1 O=!(a+b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_XOR2  = "GATE  xor2  1 O=a^b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_XNOR2 = "GATE  xnor2 1 O=!(a^b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * Non-standard 2-ary functions.
 */
inline constexpr auto GATE_LT2 = "GATE  lt2  1 O=(!a*b);      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_GT2 = "GATE  gt2  1 O=(a*!b);      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_LE2 = "GATE  le2  1 O=!(a*!b);     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto GATE_GE2 = "GATE  ge2  1 O=!(!a*b);     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * Asymmetrical 2-output 2-ary functions.
 */
inline constexpr auto GATE_HA = "GATE   ha  1 C=a*b;          PIN * INV 1 999 1.0 1.0 1.0 1.0\n"
                                "GATE   ha  1 S=!a*b+a*!b;    PIN * INV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * 3-ary functions.
 * Selection based on "Three-Input Gates for Logic Synthesis" by Dewmini S. Marakkalage et al. in TCAD 2020.
 */
inline constexpr auto GATE_AND3  = "GATE  and3     1 O=a*b*c;  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_AND3 = "GATE  d1_and3  1 O=a*b;    PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr auto GATE_XOR_AND  = "GATE  xor_and     1 O=a*(b^c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_XOR_AND = "GATE  d1_xor_and  1 O=a^b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                      "GATE  d2_xor_and  1 O=a*b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                      "GATE  d3_xor_and  1 O=a*!b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr auto GATE_OR_AND  = "GATE  or_and      1 O=a*(b+c);   PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_OR_AND = "GATE  d1_or_and   1 O=a+b;       PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                     "GATE  d2_or_and   1 O=a*b;       PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                     "GATE  d3_or_and   1 O=a;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr auto GATE_ONEHOT =
    "GATE  onehot     1 O=(a*!b*!c)^(!a*b*!c)^(!a*!b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_ONEHOT =
    "GATE  d1_onehot  1 O=a^b;                            PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
    "GATE  d2_onehot  1 O=!(a+b);                         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr auto GATE_MAJ3  = "GATE  maj3     1 O=a*b+a*c+b*c;  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_MAJ3 = "GATE  d1_maj3  1 O=a*b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                   "GATE  d2_maj3  1 O=a+b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr auto GATE_GAMBLE  = "GATE  gamble     1 O=(a*b*c)^(!a*!b*!c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_GAMBLE = "GATE  d1_gamble  1 O=!(a+b);              PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                     "GATE  d2_gamble  1 O=a*b;                 PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr auto GATE_DOT  = "GATE  dot     1 O=a^(c+(a*b));  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_DOT = "GATE  d1_dot  1 O=a;            PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d2_dot  1 O=!a*!b;        PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d3_dot  1 O=a^b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d4_dot  1 O=!a*b;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d5_dot  1 O=a*!b;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d6_dot  1 O=!a;           PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr auto GATE_MUX  = "GATE  mux     1 O=(a*!c)+(b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_MUX = "GATE  d1_mux  1 O=a*b;           PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d2_mux  1 O=a+!b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d3_mux  1 O=a*!b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d4_mux  1 O=a+b;           PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE  d5_mux  1 O=a;             PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr auto GATE_AND_XOR  = "GATE  and_xor     1 O=a^(b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr auto DECAY_AND_XOR = "GATE  d1_and_xor  1 O=a*b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                      "GATE  d2_and_xor  1 O=!a*b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                      "GATE  d3_and_xor  1 O=a^b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

}  // namespace fiction

#endif  // FICTION_TECHNOLOGY_MAPPING_LIBRARY_HPP
