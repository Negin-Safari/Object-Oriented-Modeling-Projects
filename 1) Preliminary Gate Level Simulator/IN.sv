module mux (a, b, s, y, w);
input a;
input b;
input s;
output y;
output w;
wire sbar;
wire aa;
wire bb;

nand #(3,5) U1 (sbar, s, s);
xor #(3,5) U2 (aa, a, sbar);
nor #(3,5) U3 (bb, b, s);
or #(3,5) U4 (y, aa, bb);
and #(3,5) U4 (w, aa, bb);




endmodule
