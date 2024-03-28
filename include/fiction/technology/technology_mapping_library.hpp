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
inline constexpr const char* GATE_ZERO = "GATE  zero  0  O=CONST0;\n";
inline constexpr const char* GATE_ONE  = "GATE  one   0  O=CONST1;\n";

/**
 * 1-ary functions.
 */
inline constexpr const char* GATE_BUF = "GATE  buf   1 O=a;   PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_INV = "GATE  inv1  1 O=!a;  PIN * INV    1 999 1.0 1.0 1.0 1.0\n";

/**
 * 2-ary functions.
 */
inline constexpr const char* GATE_AND2  = "GATE  and2  1 O=a*b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_NAND2 = "GATE  nand2 1 O=!(a*b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_OR2   = "GATE  or2   1 O=a+b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_NOR2  = "GATE  nor2  1 O=!(a+b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_XOR2  = "GATE  xor2  1 O=a^b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_XNOR2 = "GATE  xnor2 1 O=!(a^b);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
/**
 * Non-standard 2-ary functions.
 */
inline constexpr const char* GATE_LT2 = "GATE  lt2  1 O=(!a*b);      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_GT2 = "GATE  gt2  1 O=(a*!b);      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_LE2 = "GATE  le2  1 O=!(a*!b);     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* GATE_GE2 = "GATE  ge2  1 O=!(!a*b);     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

/**
 * 3-ary functions.
 * Selection based on "Three-Input Gates for Logic Synthesis" by Dewmini S. Marakkalage et al. in TCAD 2020.
 */
inline constexpr const char* GATE_AND3  = "GATE  and3  1 O=a*b*c;  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_AND3 = "GATE  and3  1 O=a*b;    PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr const char* GATE_XOR_AND  = "GATE  xor_and  1 O=a*(b^c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_XOR_AND = "GATE  xor_and  1 O=a^b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                             "GATE  xor_and  1 O=a*b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                             "GATE  xor_and  1 O=a*!b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr const char* GATE_OR_AND  = "GATE  or_and   1 O=a*(b+c);   PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_OR_AND = "GATE  or_and   1 O=a+b;       PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                            "GATE  or_and   1 O=a*b;       PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                            "GATE  or_and   1 O=a;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr const char* GATE_ONEHOT =
    "GATE  onehot  1 O=(a*!b*!c)^(!a*b*!c)^(!a*!b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_ONEHOT =
    "GATE  onehot  1 O=a^b;                            PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
    "GATE  onehot  1 O=!(a+b);                         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr const char* GATE_MAJ3  = "GATE  maj3  1 O=a*b+a*c+b*c;  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_MAJ3 = "GATE  maj3  1 O=a*b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                          "GATE  maj3  1 O=a+b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr const char* GATE_GAMBLE =
    "GATE  gamble  1 O=(a*b*c)^(!a*!b*!c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_GAMBLE =
    "GATE  gamble  1 O=!(a+b);              PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
    "GATE  gamble  1 O=a*b;                 PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr const char* GATE_DOT  = "GATE  dot  1 O=a^(c+(a*b));  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_DOT = "GATE  dot  1 O=a;            PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  dot  1 O=!a*!b;        PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  dot  1 O=a^b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  dot  1 O=!a*b;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  dot  1 O=a*!b;         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  dot  1 O=!a;           PIN * INV    1 999 1.0 1.0 1.0 1.0\n";

inline constexpr const char* GATE_MUX  = "GATE  mux  1 O=(a*!c)+(b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_MUX = "GATE  mux  1 O=a*b;           PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  mux  1 O=a+!b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  mux  1 O=a*!b;          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  mux  1 O=a+b;           PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                         "GATE  mux  1 O=a;             PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

inline constexpr const char* GATE_AND_XOR  = "GATE  and_xor  1 O=a^(b*c);  PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
inline constexpr const char* DECAY_AND_XOR = "GATE  and_xor  1 O=a*b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                             "GATE  and_xor  1 O=!a*b;     PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                             "GATE  and_xor  1 O=a^b;      PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

}  // namespace fiction

#endif  // FICTION_TECHNOLOGY_MAPPING_LIBRARY_HPP
