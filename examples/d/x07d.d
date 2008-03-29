/* 

	Font demo.
*/

import plplot;
import std.string;

int[17] base = [    0,  200,  500,  600,  700,  800,  900,
                 2000, 2100, 2200, 2300, 2400, 2500, 2600,
                 2700, 2800, 2900];

/*--------------------------------------------------------------------------*\
 * main
 *
 * Displays the entire "plsym" symbol (font) set.
\*--------------------------------------------------------------------------*/
int main( char[][] args )
{
  char[] text;
  PLFLT x, y;

  /* Parse and process command line arguments */
  char*[] c_args = new char*[args.length];
  foreach( size_t i, char[] arg; args ) {
    c_args[i] = toStringz(arg);
  }
  int argc = c_args.length;
  plparseopts( &argc, cast(char**)c_args, PL_PARSE_FULL );

  /* Initialize plplot */
  plinit();

  plfontld( 1 );
  for( size_t l=0; l<17; l++) {
    pladv( 0 );

    /* Set up viewport and window */

    plcol0( 2 );
    plvpor( 0.15, 0.95, 0.1, 0.9 );
    plwind( 0.0, 1.0, 0.0, 1.0 );

    /* Draw the grid using plbox */
    plbox( "bcg", 0.1, 0, "bcg", 0.1, 0 );

    /* Write the digits below the frame */
    plcol0( 15 );
    for( size_t i=0; i<=9; i++ ) {
      text=format( "%d", i);
	    plmtex( "b", 1.5, (0.1*i+0.05), 0.5, toStringz(text) );
    }

    size_t k = 0;
    for( size_t i=0; i<=9; i++ ) {
      /* Write the digits to the left of the frame */
	    text=format( "%d", base[l] + 10 * i );
	    plmtex( "lv", 1.0, (0.95-0.1*i), 1.0, toStringz(text) );
	    for( size_t j=0; j<=9; j++ ) {
        x = 0.1*j+0.05;
        y = 0.95-0.1*i;

        /* Display the symbols */
        plsym( 1, &x, &y, base[l]+k );
        k = k+1;
	    }
    }

    plmtex( "t", 1.5, 0.5, 0.5, "PLplot Example 7 - PLSYM symbols" );
  }
  plend();
  return 0;
}
