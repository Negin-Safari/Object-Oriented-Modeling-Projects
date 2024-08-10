module TEST (a, b, c, d, y);

input a;
input b;
input d;
output y;

input c;

wire x;

wire xbar;
wire aa;
wire bb;

nand U1 (y,aa, bb);

nand #(3,5) U2 (aa, xbar, a);

nor #(3,5) U3 (bb, x, c);
not #(3,5) U4 (xbar, x);

nor #(3,5) U5 (x, b, d);


endmodule


