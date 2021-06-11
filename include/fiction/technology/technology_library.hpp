//
// Created by marcel on 08.06.21.
//

#ifndef FICTION_TECHNOLOGY_LIBRARY_HPP
#define FICTION_TECHNOLOGY_LIBRARY_HPP

namespace fiction
{

constexpr const char* GATE_INV  = "GATE   inv1    1 O=!a;           PIN * INV    1 999 1.0 1.0 1.0 1.0\n";

constexpr const char* GATE_AND2 = "GATE   and2    1 O=(ab);         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
constexpr const char* GATE_OR2  = "GATE   or2     1 O={ab};         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
constexpr const char* GATE_XOR2 = "GATE   xor2    1 O=[ab];         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

constexpr const char* GATE_MAJ3 = "GATE   maj3    1 O=<abc>;        PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE   maj3    1 O=(ab);         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                  "GATE   maj3    1 O={ab};         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";

constexpr const char* GATE_DOT = "GATE   dot     1 O=[a{c(ab)}];    PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                 "GATE   dot     1 O=a;             PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                 "GATE   dot     1 O=(!a!b);        PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                 "GATE   dot     1 O=[ab];          PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                 "GATE   dot     1 O=(!ab);         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                 "GATE   dot     1 O=(a!b);         PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
                                 "GATE   dot     1 O=!a;            PIN * INV    1 999 1.0 1.0 1.0 1.0\n";

constexpr const char* GATE_BUF  = "GATE   buf     1 O=a;            PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n";
constexpr const char* GATE_ZERO = "GATE   zero    0 O=0;\n";
constexpr const char* GATE_ONE  = "GATE   one     0 O=1;\n";

}  // namespace fiction

#endif  // FICTION_TECHNOLOGY_LIBRARY_HPP
