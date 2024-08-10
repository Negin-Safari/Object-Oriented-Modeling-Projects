module main(input a,b,c,d,s, output o);
  wire w,f,g;
  
  assign #10 w = (~a & ~b & ~c & ~d )| (~a & ~b & ~c & d )| (~a & ~b & c & ~d )| (~a & b & ~c & d )| (~a & b & c & ~d )| (~a & b & c & d )| (a & ~b & ~c & ~d )| (a & ~b & ~c & d )| (a & ~b & c & ~d )| (a & b & c & ~d );
  assign #12 f = (~w & ~b & c & ~d) | (~w & b & c & ~d) | (w & ~b & ~c & ~d) | (w & ~b & ~c & d) | (w & ~b & c & ~d) | (w & ~b & c & d) | (w & b & c & ~d) | (w & b & c & d);
  assign #14 g = (~a & ~b & ~c & ~d )| (~a & ~b & c & d )| (~a & b & ~c & d )| (~a & b & c & ~d )| (~a & b & c & d )| (a & ~b & c & ~d )| (a & ~b & c & d )| (a & b & ~c & d );
  assign #5 o = s ? g : f ;
  
endmodule

