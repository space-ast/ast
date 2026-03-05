/*
   Program bltfrm_ex1
*/
#include <stdio.h>

#ifndef AST_NO_CSPICE

#include "SpiceUsr.h"

int main()
{
    /*
    Local parameters
    */
    #define FRNMLN          33
    #define NFRAME        ( SPICE_NFRAME_NINERT +  \
                            SPICE_NFRAME_NNINRT   )
    #define LNSIZE          81

    /*
    Local variables
    */
    SPICEINT_CELL           ( idset, NFRAME );

    SpiceChar               frname  [ FRNMLN ];
    SpiceChar               outlin  [ LNSIZE ];

    SpiceInt                i;
    SpiceInt                j;
    SpiceInt                nfrms;

    /*
    Get the Toolkit version number and display it.
    */
    printf( "Toolkit version: %s\n", tkvrsn_c( "TOOLKIT" ) );

    /*
    Fetch and display the frames of each class.
    */
    for ( i = 1;  i <= 7;  i++ )
    {

        if ( i < 7 )
        {
            /*
            Fetch the frames of class i.
            */
            bltfrm_c ( i, &idset );

            sprintf ( outlin,
                    "Number of frames of class %d: %d",
                    (int) i,
                    (int) card_c(&idset)                );

        }
        else
        {

            /*
            Fetch IDs of all built-in frames.
            */
            bltfrm_c ( SPICE_FRMTYP_ALL, &idset );

            sprintf( outlin,
                    "Number of built-in frames: %d",
                    (int) card_c(&idset)                );

        }

        /*
        Display the NAIF ID and name of a maximum of 5 frames
        per family.
        */
        printf ( "\n"
                "%s\n"
                "   Frame IDs and names\n",
                outlin                     );

        nfrms = mini_c ( 2, 21, card_c(&idset) );

        for ( j = 0;  j < nfrms;  j++ )
        {

            frmnam_c ( ((SpiceInt *)idset.data)[j], FRNMLN, frname );
            printf ( "%12ld   %s\n",
                    ( (long) ((SpiceInt *)idset.data)[j] ), frname );

        }

    }

    return ( 0 );
}
#else
int main(){}
#endif