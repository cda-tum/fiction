# flows
alias "qcaone(.*)" "map -aoi; exact -x -b -s{}; cell -l qcaone"
alias "topolinano" "map -aoi; exact -x -n -b -d -s columnar --topolinano; cell -l topolinano"
alias "bestagon" "map -a -o -i --nand --nor --xor --xnor; exact -x -b -d -s row --hex even_row; cell -l bestagon"

# load
alias "mux" "read ../benchmarks/TOY/mux21.v"
alias "rca" "read ../benchmarks/TOY/RCA2.v"
alias "c432" "read ../benchmarks/ISCAS85/c432.v"

# command
alias "ccsim" "clustercomplete"