/* .\trmfun.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Common Block Declarations */

struct {
    doublereal fistep;
} tra2_;

#define tra2_1 tra2_

/* TRMFUN.FOR	1987 */

/* ******************************************************************** */
/* *************** SUBROUTINES, FUNCTIONS ***************************** */
/* ******************************************************************** */
/* ******************* TRARA1, TRARA2 ********************************* */
/* ******************************************************************** */

/* Subroutine */ int trara1_(integer *descr, integer *map, doublereal *fl, 
	doublereal *bb0, doublereal *e, doublereal *f, integer *n)
{
    /* Initialized data */

    static doublereal f1 = 1.001;
    static doublereal f2 = 1.002;

    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static doublereal e0, e1, e2;
    static integer i1, i2, i3, i0;
    static doublereal f0;
    static integer l3;
    static logical s0, s1, s2;
    static integer ie, nb, nl;
    static doublereal xnl;
    extern doublereal trara2_(integer *, integer *, integer *);
    static doublereal escale, fscale;

/* *********************************************************************** */
/* *** TRARA1 FINDS PARTICLE FLUXES FOR GIVEN ENERGIES, MAGNETIC FIELD *** */
/* *** STRENGTH AND L-VALUE. FUNCTION TRARA2 IS USED TO INTERPOLATE IN *** */
/* *** B-L-SPACE.                                                      *** */
/* ***   INPUT: DESCR(8)   HEADER OF SPECIFIED TRAPPED RADITION MODEL  *** */
/* ***          MAP(...)   MAP OF TRAPPED RADITION MODEL               *** */
/* ***                     (DESCR AND MAP ARE EXPLAINED AT THE BEGIN   *** */
/* ***                     OF THE MAIN PROGRAM MODEL)                  *** */
/* ***          N          NUMBER OF ENERGIES                          *** */
/* ***          E(N)       ARRAY OF ENERGIES IN MEV                    *** */
/* ***          FL         L-VALUE                                     *** */
/* ***          BB0        =B/B0  MAGNETIC FIELD STRENGTH NORMALIZED   *** */
/* ***                     TO FIELD STRENGTH AT MAGNETIC EQUATOR       *** */
/* ***  OUTPUT: F(N)       DECADIC LOGARITHM OF INTEGRAL FLUXES IN     *** */
/* ***                     PARTICLES/(CM*CM*SEC)                       *** */
/* *********************************************************************** */
    /* Parameter adjustments */
    --descr;
    --map;
    --f;
    --e;

    /* Function Body */

    tra2_1.fistep = (doublereal) (descr[7] / descr[2]);
    escale = (doublereal) descr[4];
    fscale = (doublereal) descr[7];
/* Computing MIN */
    d__1 = 15.6, d__2 = abs(*fl);
    xnl = min(d__1,d__2);
    nl = (integer) (xnl * descr[5]);
    if (*bb0 < 1.) {
	*bb0 = 1.;
    }
    nb = (integer) ((*bb0 - 1.) * descr[6]);

/* I2 IS THE NUMBER OF ELEMENTS IN THE FLUX MAP FOR THE FIRST ENERGY. */
/* I3 IS THE INDEX OF THE LAST ELEMENT OF THE SECOND ENERGY MAP. */
/* L3 IS THE LENGTH OF THE MAP FOR THE THIRD ENERGY. */
/* E1 IS THE ENERGY OF THE FIRST ENERGY MAP (UNSCALED) */
/* E2 IS THE ENERGY OF THE SECOND ENERGY MAP (UNSCALED) */

    i1 = 0;
    i2 = map[1];
    i3 = i2 + map[i2 + 1];
    l3 = map[i3 + 1];
    e1 = map[i1 + 2] / escale;
    e2 = map[i2 + 2] / escale;

/* S0, S1, S2 ARE LOGICAL VARIABLES WHICH INDICATE WHETHER THE FLUX FOR */
/* A PARTICULAR E, B, L POINT HAS ALREADY BEEN FOUND IN A PREVIOUS CALL */
/* TO FUNCTION TRARA2. IF NOT, S.. =.TRUE. */

    s1 = TRUE_;
    s2 = TRUE_;

/* 			ENERGY LOOP */

    i__1 = *n;
    for (ie = 1; ie <= i__1; ++ie) {

/* FOR EACH ENERGY E(I) FIND THE SUCCESSIVE ENERGIES E0,E1,E2 IN */
/* MODEL MAP, WHICH OBEY  E0 < E1 < E(I) < E2 . */

L1:
	if (e[ie] <= e2 || l3 == 0) {
	    goto L2;
	}
	i0 = i1;
	i1 = i2;
	i2 = i3;
	i3 += l3;
	l3 = map[i3 + 1];
	e0 = e1;
	e1 = e2;
	e2 = map[i2 + 2] / escale;
	s0 = s1;
	s1 = s2;
	s2 = TRUE_;
	f0 = f1;
	f1 = f2;
	goto L1;

/* CALL TRARA2 TO INTERPOLATE THE FLUX-MAPS FOR E1,E2 IN L-B/B0- */
/* SPACE TO FIND FLUXES F1,F2 [IF THEY HAVE NOT ALREADY BEEN */
/* CALCULATED FOR A PREVIOUS E(I)]. */

L2:
	if (s1) {
	    f1 = trara2_(&map[i1 + 3], &nl, &nb) / fscale;
	}
	if (s2) {
	    f2 = trara2_(&map[i2 + 3], &nl, &nb) / fscale;
	}
	s1 = FALSE_;
	s2 = FALSE_;

/* FINALLY, INTERPOLATE IN ENERGY. */

	f[ie] = f1 + (f2 - f1) * (e[ie] - e1) / (e2 - e1);
	if (f2 > 0.) {
	    goto L3;
	}
	if (i1 == 0) {
	    goto L3;
	}

/* --------- SPECIAL INTERPOLATION --------------------------------- */
/* IF THE FLUX FOR THE SECOND ENERGY CANNOT BE FOUND (I.E. F2=0.0), */
/* AND THE ZEROTH ENERGY MAP HAS BEEN DEFINED (I.E. I1 NOT EQUAL 0), */
/* THEN INTERPOLATE USING THE FLUX MAPS FOR THE ZEROTH AND FIRST */
/* ENERGY AND CHOOSE THE MINIMUM OF THIS INTERPOLATIONS AND THE */
/* INTERPOLATION THAT WAS DONE WITH F2=0. */

	if (s0) {
	    f0 = trara2_(&map[i0 + 3], &nl, &nb) / fscale;
	}
	s0 = FALSE_;
/* Computing MIN */
	d__1 = f[ie], d__2 = f0 + (f1 - f0) * (e[ie] - e0) / (e1 - e0);
	f[ie] = min(d__1,d__2);

/* THE LOGARITHMIC FLUX IS ALWAYS KEPT GREATER OR EQUAL ZERO. */

L3:
/* Computing MAX */
	d__1 = f[ie];
	f[ie] = max(d__1,0.);
    }
    return 0;
} /* trara1_ */


doublereal trara2_(integer *map, integer *il, integer *ib)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Local variables */
    static integer i1, i2, j2, l1, l2, j1, kt;
    static doublereal sl1, sl2, fkb, fnb, dfl, fnl, fkb1, fkb2, fll1, fll2, 
	    fkbm, flog, fkbj1, fkbj2, flog1, flog2, flogm;
    static integer itime;
    static doublereal fincr1, fincr2;

/* ***************************************************************** */
/* ***  TRARA2 INTERPOLATES LINEARLY IN L-B/B0-MAP TO OBTAIN     *** */
/* ***  THE LOGARITHM OF INTEGRAL FLUX AT GIVEN L AND B/B0.      *** */
/* ***    INPUT: MAP(..) IS SUB-MAP (FOR SPECIFIC ENERGY) OF     *** */
/* ***                   TRAPPED RADIATION MODEL MAP             *** */
/* ***           IL      SCALED L-VALUE                          *** */
/* ***           IB      SCALED B/B0-1                           *** */
/* ***   OUTPUT: TRARA2  SCALED LOGARITHM OF PARTICLE FLUX       *** */
/* ***************************************************************** */
/* ***  SEE MAIN PROGRAM 'MODEL' FOR EXPLANATION OF MAP FORMAT   *** */
/* ***  SCALING FACTORS.                                         *** */
/* ***  THE STEPSIZE FOR THE PARAMETERIZATION OF THE LOGARITHM   *** */
/* ***  OF FLUX IS OBTAINED FROM 'COMMON/TRA2/'.                 *** */
/* ***************************************************************** */
    /* Parameter adjustments */
    --map;

    /* Function Body */
    fnl = (doublereal) (*il);
    fnb = (doublereal) (*ib);
    itime = 0;
    i2 = 0;

/* FIND CONSECUTIVE SUB-SUB-MAPS FOR SCALED L-VALUES LS1,LS2, */
/* WITH IL LESS OR EQUAL LS2.  L1,L2 ARE LENGTHS OF SUB-SUB-MAPS. */
/* I1,I2 ARE INDECES OF FIRST ELEMENTS MINUS 1. */

L1:
    l2 = map[i2 + 1];
    if (map[i2 + 2] > *il) {
	goto L2;
    }
    i1 = i2;
    l1 = l2;
    i2 += l2;
    goto L1;
L2:

/* IF SUB-SUB-MAPS ARE EMPTY, I. E. LENGTH LESS 4, THAN TRARA2=0 */

    if (l1 < 4 && l2 < 4) {
	goto L50;
    }

/* IF FLOG2 LESS FLOG1, THAN LS2 FIRST MAP AND LS1 SECOND MAP */

    if (map[i2 + 3] > map[i1 + 3]) {
	goto L10;
    }
L5:
    kt = i1;
    i1 = i2;
    i2 = kt;
    kt = l1;
    l1 = l2;
    l2 = kt;

/* DETERMINE INTERPOLATE IN SCALED L-VALUE */

L10:
    fll1 = (doublereal) map[i1 + 2];
    fll2 = (doublereal) map[i2 + 2];
    dfl = (fnl - fll1) / (fll2 - fll1);
    flog1 = (doublereal) map[i1 + 3];
    flog2 = (doublereal) map[i2 + 3];
    fkb1 = 0.;
    fkb2 = 0.;
    if (l1 < 4) {
	goto L32;
    }

/* B/B0 LOOP */

    i__1 = l2;
    for (j2 = 4; j2 <= i__1; ++j2) {
	fincr2 = (doublereal) map[i2 + j2];
	if (fkb2 + fincr2 > fnb) {
	    goto L23;
	}
	fkb2 += fincr2;
/* L17: */
	flog2 -= tra2_1.fistep;
    }
    ++itime;
    if (itime == 1) {
	goto L5;
    }
    goto L50;
L23:
    if (itime == 1) {
	goto L30;
    }
    if (j2 == 4) {
	goto L28;
    }
    sl2 = flog2 / fkb2;
    i__1 = l1;
    for (j1 = 4; j1 <= i__1; ++j1) {
	fincr1 = (doublereal) map[i1 + j1];
	fkb1 += fincr1;
	flog1 -= tra2_1.fistep;
	fkbj1 = (flog1 / tra2_1.fistep * fincr1 + fkb1) / (fincr1 / 
		tra2_1.fistep * sl2 + 1.);
	if (fkbj1 <= fkb1) {
	    goto L31;
	}
/* L27: */
    }
    if (fkbj1 <= fkb2) {
	goto L50;
    }
L31:
    if (fkbj1 <= fkb2) {
	goto L29;
    }
    fkb1 = 0.;
L30:
    fkb2 = 0.;
L32:
    j2 = 4;
    fincr2 = (doublereal) map[i2 + j2];
    flog2 = (doublereal) map[i2 + 3];
    flog1 = (doublereal) map[i1 + 3];
L28:
    flogm = flog1 + (flog2 - flog1) * dfl;
    fkbm = 0.;
    fkb2 += fincr2;
    flog2 -= tra2_1.fistep;
    sl2 = flog2 / fkb2;
    if (l1 < 4) {
	goto L35;
    }
    j1 = 4;
    fincr1 = (doublereal) map[i1 + j1];
    fkb1 += fincr1;
    flog1 -= tra2_1.fistep;
    sl1 = flog1 / fkb1;
    goto L15;
L29:
    fkbm = fkbj1 + (fkb2 - fkbj1) * dfl;
    flogm = fkbm * sl2;
    flog2 -= tra2_1.fistep;
    fkb2 += fincr2;
    sl1 = flog1 / fkb1;
    sl2 = flog2 / fkb2;
L15:
    if (sl1 < sl2) {
	goto L20;
    }
    fkbj2 = (flog2 / tra2_1.fistep * fincr2 + fkb2) / (fincr2 / tra2_1.fistep 
	    * sl1 + 1.);
    fkb = fkb1 + (fkbj2 - fkb1) * dfl;
    flog = fkb * sl1;
    if (fkb >= fnb) {
	goto L60;
    }
    fkbm = fkb;
    flogm = flog;
    if (j1 >= l1) {
	goto L50;
    }
    ++j1;
    fincr1 = (doublereal) map[i1 + j1];
    flog1 -= tra2_1.fistep;
    fkb1 += fincr1;
    sl1 = flog1 / fkb1;
    goto L15;
L20:
    fkbj1 = (flog1 / tra2_1.fistep * fincr1 + fkb1) / (fincr1 / tra2_1.fistep 
	    * sl2 + 1.);
    fkb = fkbj1 + (fkb2 - fkbj1) * dfl;
    flog = fkb * sl2;
    if (fkb >= fnb) {
	goto L60;
    }
    fkbm = fkb;
    flogm = flog;
    if (j2 >= l2) {
	goto L50;
    }
    ++j2;
    fincr2 = (doublereal) map[i2 + j2];
    flog2 -= tra2_1.fistep;
    fkb2 += fincr2;
    sl2 = flog2 / fkb2;
    goto L15;
L35:
    fincr1 = 0.;
    sl1 = -9e5;
    goto L20;
L60:
    if (fkb < fkbm + 1e-10) {
	goto L50;
    }
    ret_val = flogm + (flog - flogm) * ((fnb - fkbm) / (fkb - fkbm));
    ret_val = max(ret_val,0.);
    return ret_val;
L50:
    ret_val = 0.;
    return ret_val;
} /* trara2_ */

