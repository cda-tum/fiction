//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_COMMANDS_HPP
#define FICTION_COMMANDS_HPP

// general commands
#include "cmd/general/include/clear.hpp"
#include "cmd/general/include/version.hpp"

// input/output commands
#include "cmd/io/include/blif.hpp"
#include "cmd/io/include/fgl.hpp"
#include "cmd/io/include/fqca.hpp"
#include "cmd/io/include/qca.hpp"
#include "cmd/io/include/qcc.hpp"
#include "cmd/io/include/qll.hpp"
#include "cmd/io/include/read.hpp"
#include "cmd/io/include/sqd.hpp"
#include "cmd/io/include/tt.hpp"
#include "cmd/io/include/verilog.hpp"

// logic synthesis commands
#ifdef FICTION_LOGIC_SYNTHESIS_FLOW
#include "cmd/logic/include/abc.hpp"
#include "cmd/logic/include/akers.hpp"
#include "cmd/logic/include/balance.hpp"
#include "cmd/logic/include/fanouts.hpp"
#include "cmd/logic/include/gates.hpp"
#include "cmd/logic/include/map.hpp"
#include "cmd/logic/include/miginvopt.hpp"
#include "cmd/logic/include/miginvprop.hpp"
#include "cmd/logic/include/random.hpp"
#include "cmd/logic/include/simulate.hpp"
#include "cmd/verification/include/equiv.hpp"
#endif

// physical design and validation commands
#ifdef FICTION_PHYSICAL_DESIGN_FLOW
#include "cmd/physical_design/include/exact.hpp"
#include "cmd/physical_design/include/gold.hpp"
#include "cmd/physical_design/include/hex.hpp"
#include "cmd/physical_design/include/onepass.hpp"
#include "cmd/physical_design/include/optimize.hpp"
#include "cmd/physical_design/include/ortho.hpp"
#include "cmd/technology/include/area.hpp"
#include "cmd/technology/include/cell.hpp"
#include "cmd/technology/include/energy.hpp"
#include "cmd/verification/include/check.hpp"
#endif

// physical simulation commands
#ifdef FICTION_SIMULATION_FLOW
#include "cmd/simulation/include/clustercomplete.hpp"
#include "cmd/simulation/include/opdom.hpp"
#include "cmd/simulation/include/quickexact.hpp"
#include "cmd/simulation/include/quicksim.hpp"
#include "cmd/simulation/include/temp.hpp"
#endif

#endif  // FICTION_COMMANDS_HPP
