module fa(in0, in1, cin, sum, cout);
input in0, in1, cin;
output sum, cout;

wire w1, w2, w3, w4, w5, w6, w7, w8, w9, n1;

assign w1 = in0 | in1;
assign w2 = in0 & in1;
assign w3 = ~w2;
assign n1 = w1 & w3;

assign w5 = cin | n1;
assign w6 = cin | n1;
assign w7 = ~w6;
assign sum = w5 & w7;

assign w8 = in0 & in1;
assign w9 = n1 & cin;
assign cout = w8 & w9;

endmodule
