//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_COMMANDS_HPP
#define FICTION_COMMANDS_HPP

// general commands
#include "cmd/general/clear.hpp"
#include "cmd/general/version.hpp"

// input/output commands
#include "cmd/io/blif.hpp"
#include "cmd/io/fgl.hpp"
#include "cmd/io/fqca.hpp"
#include "cmd/io/qca.hpp"
#include "cmd/io/qcc.hpp"
#include "cmd/io/qll.hpp"
#include "cmd/io/read.hpp"
#include "cmd/io/sqd.hpp"
#include "cmd/io/tt.hpp"
#include "cmd/io/verilog.hpp"

// logic synthesis commands
#ifdef FICTION_LOGIC_SYNTHESIS_FLOW
#include "cmd/logic/abc.hpp"
#include "cmd/logic/akers.hpp"
#include "cmd/logic/balance.hpp"
#include "cmd/logic/fanouts.hpp"
#include "cmd/logic/gates.hpp"
#include "cmd/logic/map.hpp"
#include "cmd/logic/miginvopt.hpp"
#include "cmd/logic/miginvprop.hpp"
#include "cmd/logic/random.hpp"
#include "cmd/logic/simulate.hpp"
#include "cmd/verification/equiv.hpp"
#endif

// physical design and validation commands
#ifdef FICTION_PHYSICAL_DESIGN_FLOW
#include "cmd/physical_design/exact.hpp"
#include "cmd/physical_design/gold.hpp"
#include "cmd/physical_design/hex.hpp"
#include "cmd/physical_design/onepass.hpp"
#include "cmd/physical_design/optimize.hpp"
#include "cmd/physical_design/ortho.hpp"
#include "cmd/technology/area.hpp"
#include "cmd/technology/cell.hpp"
#include "cmd/technology/energy.hpp"
#include "cmd/verification/check.hpp"
#endif

// physical simulation commands
#ifdef FICTION_SIMULATION_FLOW
#include "cmd/simulation/clustercomplete.hpp"
#include "cmd/simulation/opdom.hpp"
#include "cmd/simulation/quickexact.hpp"
#include "cmd/simulation/quicksim.hpp"
#include "cmd/simulation/temp.hpp"
#endif

#endif  // FICTION_COMMANDS_HPP
