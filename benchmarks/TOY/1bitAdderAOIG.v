module top(x, y, z, H, I);
input x, y, z;
output H, I;
wire A, B, D, E, F;

assign A = y | z ;
assign B = y & z ;
assign D = A & ~B ;
assign E = x | D ;
assign F = x & D ;
assign H = E & ~F ;
assign I = B | F ;

endmodule
