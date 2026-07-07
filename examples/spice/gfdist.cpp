/*
   Program gfdist_ex1
*/
#ifndef AST_NO_CSPICE
#include <stdio.h>
#include <clocale>
#include "SpiceUsr.h"
#include "ast/RunTime.hpp"
#include "ast/Environment.hpp"
#include "ast/TestConfig.hpp"


AST_USING_NAMESPACE

int main()
{
    setlocale(LC_ALL, ".UTF-8");
    if(aIsCI()) return 0;

    /*
    Constants
    */
    #define  TIMFMT  "YYYY MON DD HR:MN:SC.###"
    #define  MAXWIN  200
    #define  NINTVL  100
    #define  TIMLEN  41

    /*
    Local variables
    */
    SpiceChar               begstr [ TIMLEN ];
    SpiceChar               endstr [ TIMLEN ];

    SPICEDOUBLE_CELL      ( cnfine, MAXWIN );
    SPICEDOUBLE_CELL      ( result, MAXWIN );

    SpiceDouble             adjust;
    SpiceDouble             dist;
    SpiceDouble             et0;
    SpiceDouble             et1;
    SpiceDouble             lt;
    SpiceDouble             pos    [3];
    SpiceDouble             refval;
    SpiceDouble             start;
    SpiceDouble             step;
    SpiceDouble             stop;

    SpiceInt                i;

    /*
    Load kernels.
    */
    furnsh_c ( aTestGetConfigValue("SPK_FILE").toString().c_str() );
    furnsh_c ( aTestGetConfigValue("LSK_FILE").toString().c_str() );
    furnsh_c ( aTestGetConfigValue("PCK_FILE").toString().c_str() );

    /*
    Store the time bounds of our search interval in
    the confinement window.
    */
    str2et_c ( "2007 JAN 1", &et0 );
    str2et_c ( "2007 APR 1", &et1 );

    wninsd_c ( et0, et1, &cnfine );

    /*
    Search using a step size of 1 day (in units of
    seconds).  The reference value is 400000 km.
    We're not using the adjustment feature, so
    we set `adjust' to zero.
    */
    step   = spd_c();
    refval = 4.e5;
    adjust = 0.0;

    /*
    Perform the search. The set of times when the
    constraint is met will be stored in the SPICE
    window `result'.
    */
    gfdist_c ( "MOON", "NONE", "EARTH", ">",     refval,
            adjust, step,   NINTVL,  &cnfine, &result );

    /*
    Display the results.
    */
    if ( wncard_c(&result) == 0 )
    {
       printf ( "Result window is empty.\n\n" );
    }
    else
    {
        for ( i = 0;  i < wncard_c(&result);  i++ )
        {
            /*
            Fetch the endpoints of the Ith interval
            of the result window.
            */
            wnfetd_c ( &result, i, &start, &stop );

            /*
            Check the distance at the interval's
            start and stop times.
            */
            spkpos_c ( "MOON",  start, "J2000", "NONE",
                       "EARTH", pos,   &lt             );

            dist = vnorm_c(pos);

            timout_c ( start, TIMFMT, TIMLEN, begstr );

            printf ( "Start time, distance = %s %17.9f\n",
                    begstr, dist                          );

            spkpos_c ( "MOON",  stop, "J2000", "NONE",
                          "EARTH", pos,  &lt             );

            dist = vnorm_c(pos);

            timout_c ( stop, TIMFMT, TIMLEN, endstr );

            printf ("Stop time,  distance = %s %17.9f\n",
                    endstr, dist                          );
        }
    }

    return ( 0 );
}
#else
int main(){}
#endif