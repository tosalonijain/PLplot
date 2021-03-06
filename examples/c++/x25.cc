//  Test program for filling polygons and proper clipping
//
//  Copyright (C) 2005 Arjen Markus
//            (C) 2006 Andrew Ross
//
//
//  This file is part of PLplot.
//
//  PLplot is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Library General Public License as published
//  by the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  PLplot is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public License
//  along with PLplot; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "plc++demos.h"

class x25 {
public:
    x25( int, char ** );

private:
    plstream *pls;
};

x25::x25( int argc, char *argv[] )
{
    int   i, j, k;
    int   npts = 0;
    PLFLT xextreme[10][2];
    PLFLT yextreme[10][2];
    PLFLT x0[10];
    PLFLT y0[10];

    pls = new plstream();

// Parse and process command line arguments

    pls->parseopts( &argc, argv, PL_PARSE_FULL );

// Initialize plplot

    pls->ssub( 3, 3 );
    pls->init();

    xextreme[0][0] = -120.0; xextreme[0][1] = 120.0; yextreme[0][0] = -120.0; yextreme[0][1] = 120.0;
    xextreme[1][0] = -120.0; xextreme[1][1] = 120.0; yextreme[1][0] = 20.0; yextreme[1][1] = 120.0;
    xextreme[2][0] = -120.0; xextreme[2][1] = 120.0; yextreme[2][0] = -20.0; yextreme[2][1] = 120.0;
    xextreme[3][0] = -80.0; xextreme[3][1] = 80.0; yextreme[3][0] = -20.0; yextreme[3][1] = 120.0;
    xextreme[4][0] = -220.0; xextreme[4][1] = -120.0; yextreme[4][0] = -120.0; yextreme[4][1] = 120.0;
    xextreme[5][0] = -20.0; xextreme[5][1] = 20.0; yextreme[5][0] = -120.0; yextreme[5][1] = 120.0;
    xextreme[6][0] = -20.0; xextreme[6][1] = 20.0; yextreme[6][0] = -20.0; yextreme[6][1] = 20.0;
    xextreme[7][0] = -80.0; xextreme[7][1] = 80.0; yextreme[7][0] = -80.0; yextreme[7][1] = 80.0;
    xextreme[8][0] = 20.0; xextreme[8][1] = 120.0; yextreme[8][0] = -120.0; yextreme[8][1] = 120.0;

    for ( k = 0; k < 2; k++ )
    {
        for ( j = 0; j < 4; j++ )
        {
            if ( j == 0 )
            {
// Polygon 1: a diamond
                x0[0] = 0; y0[0] = -100;
                x0[1] = -100; y0[1] = 0;
                x0[2] = 0; y0[2] = 100;
                x0[3] = 100; y0[3] = 0;
                npts  = 4;
            }
            if ( j == 1 )
            {
// Polygon 1: a diamond - reverse direction
                x0[3] = 0; y0[3] = -100;
                x0[2] = -100; y0[2] = 0;
                x0[1] = 0; y0[1] = 100;
                x0[0] = 100; y0[0] = 0;
                npts  = 4;
            }
            if ( j == 2 )
            {
// Polygon 2: a square with punctures
                x0[0] = -100; y0[0] = -100;
                x0[1] = -100; y0[1] = -80;
                x0[2] = 80; y0[2] = 0;
                x0[3] = -100; y0[3] = 80;
                x0[4] = -100; y0[4] = 100;
                x0[5] = -80; y0[5] = 100;
                x0[6] = 0; y0[6] = 80;
                x0[7] = 80; y0[7] = 100;
                x0[8] = 100; y0[8] = 100;
                x0[9] = 100; y0[9] = -100;
                npts  = 10;
            }
            if ( j == 3 )
            {
// Polygon 2: a square with punctures - reversed direction
                x0[9] = -100; y0[9] = -100;
                x0[8] = -100; y0[8] = -80;
                x0[7] = 80; y0[7] = 0;
                x0[6] = -100; y0[6] = 80;
                x0[5] = -100; y0[5] = 100;
                x0[4] = -80; y0[4] = 100;
                x0[3] = 0; y0[3] = 80;
                x0[2] = 80; y0[2] = 100;
                x0[1] = 100; y0[1] = 100;
                x0[0] = 100; y0[0] = -100;
                npts  = 10;
            }

            for ( i = 0; i < 9; i++ )
            {
                pls->adv( 0 );
                pls->vsta();
                pls->wind( xextreme[i][0], xextreme[i][1], yextreme[i][0], yextreme[i][1] );

                pls->col0( 2 );
                pls->box( "bc", 1.0, 0, "bcnv", 10.0, 0 );
                pls->col0( 1 );
                pls->psty( 0 );
                if ( k == 0 )
                    pls->fill( npts, x0, y0 );
                else
                    pls->gradient( npts, x0, y0, 45. );
                pls->col0( 2 );
                pls->lsty( 1 );
                pls->line( npts, x0, y0 );
            }
        }
    }

// Don't forget to delete stream to finish off!

    delete pls;
}

int main( int argc, char **argv )
{
    x25 *x = new x25( argc, argv );

    delete x;
}
