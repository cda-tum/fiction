# commands
alias "xibs(.*)" "exact -x -i -b -s{}"

# flows
alias "synth(.*)" "{}; cell; show"
alias "phys(.*)" "{}; cell; qca"

# load
alias "mux" "read ../benchmarks/TOY/mux21.v"
alias "rca" "read ../benchmarks/TOY/RCA2.v"
