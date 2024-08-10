module main ( a , b , c , d , s , o ); 
input a ;
input b ;
input c ;
input d ;
input s ;
output o ;
wire aBAR ;
wire bBAR ;
wire cBAR ;
wire dBAR ;
wire wBAR ;
wire sBAR ;
wire w0 ;
wire w1 ;
wire w2 ;
wire w3 ;
wire f0 ;
wire f1 ;
wire f2 ;
wire g0 ;
wire g1 ;
wire g2 ;
wire g3 ;
wire g4 ;
wire g5 ;
wire g6 ;
wire g7 ;
wire g8 ;
wire g9 ;
wire g10 ;
wire o0 ;
wire o1 ;
wire w4 ;
wire w ;
wire f3 ;
wire f ;
wire g11 ;
wire g12 ;
wire g13 ;
wire g ;
nand #( 3 , 3 ) GaBAR ( aBAR , a , a );
nand #( 3 , 3 ) GbBAR ( bBAR , b , b );
nand #( 3 , 3 ) GcBAR ( cBAR , c , c );
nand #( 3 , 3 ) GdBAR ( dBAR , d , d );
nand #( 3 , 3 ) GwBAR ( wBAR , w , w );
nand #( 3 , 3 ) GsBAR ( sBAR , s , s );
and #( 5 , 5 ) Gw0 ( w0 , bBAR , cBAR );
and #( 5 , 5 ) Gw1 ( w1 , c , dBAR );
and #( 5 , 5 ) Gw2 ( w2 , aBAR , b );
and #( 5 , 5 ) Gw3 ( w3 , d , w2 );
and #( 5 , 5 ) Gf0 ( f0 , c , dBAR );
and #( 5 , 5 ) Gf1 ( f1 , w , bBAR );
and #( 5 , 5 ) Gf2 ( f2 , w , c );
and #( 5 , 5 ) Gg0 ( g0 , aBAR , bBAR );
and #( 5 , 5 ) Gg1 ( g1 , cBAR , dBAR );
and #( 5 , 5 ) Gg2 ( g2 , g0 , g1 );
and #( 5 , 5 ) Gg3 ( g3 , aBAR , b );
and #( 5 , 5 ) Gg4 ( g4 , c , g3 );
and #( 5 , 5 ) Gg5 ( g5 , a , bBAR );
and #( 5 , 5 ) Gg6 ( g6 , c , g5 );
and #( 5 , 5 ) Gg7 ( g7 , b , cBAR );
and #( 5 , 5 ) Gg8 ( g8 , d , g7 );
and #( 5 , 5 ) Gg9 ( g9 , aBAR , c );
and #( 5 , 5 ) Gg10 ( g10 , d , g9 );
and #( 5 , 5 ) Go0 ( o0 , s , g );
and #( 5 , 5 ) Go1 ( o1 , sBAR , f );
or #( 5 , 5 ) Gw4 ( w4 , w0 , w1 );
or #( 5 , 5 ) Gw ( w , w3 , w4 );
or #( 5 , 5 ) Gf3 ( f3 , f0 , f1 );
or #( 5 , 5 ) Gf ( f , f2 , f3 );
or #( 5 , 5 ) Gg11 ( g11 , g2 , g4 );
or #( 5 , 5 ) Gg12 ( g12 , g6 , g8 );
or #( 5 , 5 ) Gg13 ( g13 , g10 , g11 );
or #( 5 , 5 ) Gg ( g , g12 , g13 );
or #( 5 , 5 ) Go ( o , o0 , o1 );
endmodule
