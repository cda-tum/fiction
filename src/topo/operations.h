//
// Created by marcel on 26.04.18.
//

#ifndef FICTION_OPERATIONS_H
#define FICTION_OPERATIONS_H

#include <iostream>

/**
 * Available (elementary) operations. Its not scoped to access underlying type more sufficiently.
 * NONE : No operation.
 * AND  : Binary conjunction.
 * OR   : Binary disjunction.
 * NOT  : Unary negation.
 * MAJ  : Ternary majority.
 * F1O2 : 1-to-2 Fan-out.
 * F1O3 : 1-to-3 Fan-out.
 * W    : (Multi-)Wire.
 * PI   : Primary input node.
 * PO   : Primary output node.
 */
enum operation { NONE = 0u, AND, OR, NOT, MAJ, F1O2, F1O3, W, PI, PO };
/**
 * Number of different supported operations.
 */
constexpr const uint8_t OP_COUNT = 10ul;
/**
 * Writes the given operation o to the given ostream channel as a text symbol as it could be used
 * in grid representations.
 *
 * @param os An std::ostream object to write o into.
 * @param o Operation which should be written into os.
 * @return The given ostream channel extended by a representation of d.
 */
std::ostream& operator<<(std::ostream& os, const operation o) noexcept;
/**
 * Returns a string representation of the given operation o.
 *
 * @param o Operation whose string representation is desired.
 * @return String representation of o.
 */
std::string str(const operation o) noexcept;
/**
 * Returns a string representation of the given operation o suitable for printing names.
 *
 * @param o Operation whose string representation is desired.
 * @return String representation of o.
 */
std::string name_str(const operation o) noexcept;


#endif //FICTION_OPERATIONS_H
