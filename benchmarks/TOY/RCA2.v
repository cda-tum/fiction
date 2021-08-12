module top(a0, a1, b0, b1, cin, sum0, sum1, cout);
input a0, a1, b0, b1, cin;
output sum0, sum1, cout;

wire w1, w2, w3, w4, w5, w6, w7, w8, w9, n1;
wire w11, w12, w13, w14, w15, w16, w17, w18, w19, n2, cint;

assign w1 = a0 | b0;
assign w2 = a0 & b0;
assign w3 = ~w2;
assign n1 = w1 & w3;

assign w5 = cin | n1;
assign w6 = cin & n1;
assign w7 = ~w6;
assign sum0 = w5 & w7;

assign w8 = a1 & b1;
assign w9 = n1 & cin;
assign cint = w8 | w9;

assign w11 = a1 | b1;
assign w12 = a1 & b1;
assign w13 = ~w12;
assign n2 = w11 & w13;

assign w15 = cint | n2;
assign w16 = cint & n2;
assign w17 = ~w16;
assign sum1 = w15 & w17;

assign w18 = a1 & b1;
assign w19 = n2 & cin;
assign cout = w18 | w19;

endmodule 
