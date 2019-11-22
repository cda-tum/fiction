# commands
alias "xibs(.*)" "exact -x -i -b -s{}"
alias "topo" "exact -xinobs topolinano3; cell -l 1; print -c"

# flows
alias "synth(.*)" "{}; cell; show -c"
alias "phys(.*)" "{}; cell; qca"

# load
alias "mux" "read ../benchmarks/TOY/mux21.v"
alias "rca" "read ../benchmarks/TOY/RCA2.v"
alias "c432" "read ../benchmarks/ISCAS85/c432.v"
