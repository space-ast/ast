/* .\igrf13.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/


#if defined(__GNUC__) && !defined(__clang__)
#	pragma GCC diagnostic ignored "-Wmissing-braces"
#endif


#include "f2c.h"

/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__3 = 3;
static integer c__5 = 5;
static integer c__0 = 0;
static doublereal c_b433 = 1.1;

/* Main program */ int MAIN__(void)
{
    /* Initialized data */

    static doublereal dtmn = 1900.;
    static doublereal dtmx = 2030.;

    /* Format strings */
    static char fmt_991[] = "(a30)";
    static char fmt_928[] = "(\002 D is declination (+ve east)\002/\002 I is"
	    " inclination (+ve down)\002/\002 H is horizontal intensity\002"
	    "/\002 X is north component\002/\002 Y is east component\002/\002"
	    " Z is vertical component (+ve down)\002/\002 F is total intensity"
	    "\002)";
    static char fmt_929[] = "(/\002 SV is secular variation (annual rate of "
	    "change)\002)";
    static char fmt_930[] = "(1x,f8.3,\002 Lat\002,2i4,a11,\002 Long \002,2i"
	    "4,f10.3,\002 km \002,a20)";
    static char fmt_931[] = "(1x,f8.3,\002 Lat\002,f8.3,a11,\002 Long \002,f"
	    "8.3,f10.3,\002 km \002,a20)";
    static char fmt_937[] = "(15x,\002D =\002,i5,\002 deg\002,i4,\002 min"
	    "\002,4x,\002SV =\002,i8,\002 min/yr\002)";
    static char fmt_939[] = "(15x,\002I =\002,i5,\002 deg\002,i4,\002 min"
	    "\002,4x,\002SV =\002,i8,\002 min/yr\002)";
    static char fmt_941[] = "(15x,\002H =\002,i8,\002 nT     \002,5x,\002S"
	    "V =\002,i8,\002 nT/yr\002)";
    static char fmt_943[] = "(15x,\002X =\002,i8,\002 nT     \002,5x,\002S"
	    "V =\002,i8,\002 nT/yr\002)";
    static char fmt_945[] = "(15x,\002Y =\002,i8,\002 nT     \002,5x,\002S"
	    "V =\002,i8,\002 nT/yr\002)";
    static char fmt_947[] = "(15x,\002Z =\002,i8,\002 nT     \002,5x,\002S"
	    "V =\002,i8,\002 nT/yr\002)";
    static char fmt_949[] = "(15x,\002F =\002,i8,\002 nT     \002,5x,\002S"
	    "V =\002,i8,\002 nT/yr\002/)";
    static char fmt_932[] = "(\002Lat\002,2i4,a11,\002  Long \002,2i4,f10.3"
	    ",\002 km \002,a20)";
    static char fmt_933[] = "(\002Lat\002,f8.3,a11,\002  Long \002,f8.3,f10."
	    "3,\002 km \002,a20)";
    static char fmt_934[] = "(3x,\002DATE\002,7x,\002D\002,3x,\002SV\002,6x"
	    ",\002I\002,2x,\002SV\002,6x,\002H\002,4x,\002SV\002,7x,\002X\002"
	    ",4x,\002SV\002,7x,\002Y\002,4x,\002SV\002,7x,\002Z\002,4x,\002S"
	    "V\002,6x,\002F\002,4x,\002SV\002)";
    static char fmt_935[] = "(1x,f6.1,f8.2,i6,f7.2,i4,i7,i6,3(i8,i6),i7,i6)";
    static char fmt_958[] = "(\002 Date =\002,f9.3,5x,\002Altitude =\002,f10"
	    ".3,\002 km\002,5x,a11//\002      Lat     Long\002,7x,\002D\002,7"
	    "x,\002I\002,7x,\002H\002,7x,\002X\002,7x,\002Y\002,7x,\002Z\002,"
	    "7x,\002F\002)";
    static char fmt_959[] = "(2f9.3,2f8.2,5i8)";
    static char fmt_960[] = "(2f9.3,7i8)";
    static char fmt_961[] = "(14x,\002SV: \002,7i8)";
    static char fmt_962[] = "(/\002 D is declination in degrees (+ve east"
	    ")\002/\002 I is inclination in degrees (+ve down)\002/\002 H is "
	    "horizontal intensity in nT\002/\002 X is north component in n"
	    "T\002/\002 Y is east component in nT\002/\002 Z is vertical comp"
	    "onent in nT (+ve down)\002/\002 F is total intensity in nT\002)";
    static char fmt_963[] = "(\002 SV is secular variation (annual rate of c"
	    "hange)\002/\002 Units for SV: minutes/yr (D & I); nT/yr (H,X,Y,Z"
	    " & F)\002)";
    static char fmt_964[] = "(/\002 D is SV in declination in minutes/yr (+v"
	    "e east)\002/\002 I is SV in inclination in minutes/yr (+ve down"
	    ")\002/\002 H is SV in horizontal intensity in nT/yr\002/\002 X i"
	    "s SV in north component in nT/yr\002/\002 Y is SV in east compon"
	    "ent in nT/yr\002/\002 Z is SV in vertical component in nT/yr (+v"
	    "e down)\002/\002 F is SV in total intensity in nT/yr\002)";
    static char fmt_972[] = "(\002 ***** Error *****\002/\002 DATE =\002,f9."
	    "3,\002 - out of range\002)";
    static char fmt_973[] = "(\002 ***** Error *****\002/\002 A value of ALT"
	    " =\002,f10.3,\002 is not allowed when ITYPE =\002,i2)";
    static char fmt_966[] = "(\002 ***** Error *****\002/\002 XLT =\002,f9"
	    ".3,\002 - out of range\002)";
    static char fmt_967[] = "(\002 ***** Error *****\002/\002 XLN =\002,f10."
	    "3,\002 - out of range\002)";
    static char fmt_968[] = "(\002 ***** Error *****\002/\002 Latitude out o"
	    "f range\002,\002 - LTD =\002,i6,5x,\002LTM =\002,i4)";
    static char fmt_969[] = "(\002 ***** Error *****\002/\002 Longitude out "
	    "of range\002,\002 - LND =\002,i8,5x,\002LNM =\002,i4)";
    static char fmt_970[] = "(\002 ***** Error *****\002/\002 Latitude limit"
	    "s of table out of range - LTI =\002,i6,5x,\002 LTF =\002,i6)";
    static char fmt_971[] = "(\002 ***** Error *****\002/\002 Longitude limi"
	    "ts of table out of range - LNI =\002,i8,5x,\002 LNF =\002,i8)";

    /* System generated locals */
    integer i__1;
    doublereal d__1;
    olist o__1;

    /* Builtin functions */
    integer s_wsle(cilist *), e_wsle(void), do_lio(integer *, integer *, char 
	    *, ftnlen), s_rsfe(cilist *), do_fio(integer *, char *, ftnlen), 
	    e_rsfe(void), f_open(olist *), s_rsle(cilist *), e_rsle(void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), e_wsfe(void);
    /* Subroutine */ int s_stop(char *, ftnlen);
    double atan2(doublereal, doublereal), sqrt(doublereal);
    integer i_dnnt(doublereal *);

    /* Local variables */
    extern /* Subroutine */ int igrf13syn_(integer *, doublereal *, integer *,
	     doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *);
    static doublereal d__, f, h__;
    static integer i__;
    static doublereal s, x, y, z__, f1, dd;
    static char ia[1];
    static doublereal df, dh;
    static integer ih, nf;
    static doublereal ds;
    static integer ln;
    static doublereal dx, dy;
    static integer iu;
    static doublereal dz;
    static integer lt, ix, iy, iz, idd, idf, idh, idm, inc, ifl, lnd, ids;
    static char fnm[30];
    static doublereal alt, clt;
    static integer ltd, idx, lnm, idy, idz, lti, ltf, lni, ltm, imx, lnf;
    static doublereal xln, xlt;
    static integer idec;
    static doublereal fact, date;
    static char name__[20];
    static integer incm;
    static doublereal xlnd, xlnf, xlni;
    static integer iopt;
    static doublereal xltf, xltd, xlti;
    static char type__[11];
    static integer idecm, itype;
    extern /* Subroutine */ int dmddec_(integer *, integer *, doublereal *), 
	    ddecdm_(doublereal *, integer *, integer *);
    static integer ncount;

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, 0, 0 };
    static cilist io___4 = { 0, 6, 0, 0, 0 };
    static cilist io___5 = { 0, 6, 0, 0, 0 };
    static cilist io___6 = { 0, 6, 0, 0, 0 };
    static cilist io___7 = { 0, 6, 0, 0, 0 };
    static cilist io___8 = { 0, 6, 0, 0, 0 };
    static cilist io___9 = { 0, 6, 0, 0, 0 };
    static cilist io___10 = { 0, 6, 0, 0, 0 };
    static cilist io___11 = { 0, 6, 0, 0, 0 };
    static cilist io___12 = { 0, 6, 0, 0, 0 };
    static cilist io___13 = { 0, 6, 0, 0, 0 };
    static cilist io___14 = { 0, 6, 0, 0, 0 };
    static cilist io___15 = { 0, 6, 0, 0, 0 };
    static cilist io___16 = { 0, 6, 0, 0, 0 };
    static cilist io___17 = { 0, 6, 0, 0, 0 };
    static cilist io___18 = { 0, 6, 0, 0, 0 };
    static cilist io___19 = { 0, 6, 0, 0, 0 };
    static cilist io___20 = { 0, 6, 0, 0, 0 };
    static cilist io___21 = { 0, 6, 0, 0, 0 };
    static cilist io___22 = { 0, 6, 0, 0, 0 };
    static cilist io___23 = { 0, 6, 0, 0, 0 };
    static cilist io___24 = { 0, 6, 0, 0, 0 };
    static cilist io___25 = { 0, 5, 0, fmt_991, 0 };
    static cilist io___30 = { 0, 6, 0, 0, 0 };
    static cilist io___31 = { 0, 6, 0, 0, 0 };
    static cilist io___32 = { 0, 6, 0, 0, 0 };
    static cilist io___33 = { 0, 5, 0, 0, 0 };
    static cilist io___36 = { 0, 6, 0, 0, 0 };
    static cilist io___37 = { 0, 6, 0, 0, 0 };
    static cilist io___38 = { 0, 6, 0, 0, 0 };
    static cilist io___39 = { 0, 6, 0, 0, 0 };
    static cilist io___40 = { 0, 5, 0, 0, 0 };
    static cilist io___42 = { 0, 6, 0, 0, 0 };
    static cilist io___43 = { 0, 6, 0, 0, 0 };
    static cilist io___44 = { 0, 6, 0, 0, 0 };
    static cilist io___45 = { 0, 5, 0, 0, 0 };
    static cilist io___47 = { 0, 6, 0, 0, 0 };
    static cilist io___48 = { 0, 5, 0, "(A1)", 0 };
    static cilist io___50 = { 0, 0, 0, fmt_928, 0 };
    static cilist io___51 = { 0, 0, 0, fmt_929, 0 };
    static cilist io___52 = { 0, 0, 0, 0, 0 };
    static cilist io___53 = { 0, 0, 0, 0, 0 };
    static cilist io___54 = { 0, 6, 0, 0, 0 };
    static cilist io___55 = { 0, 5, 0, 0, 0 };
    static cilist io___57 = { 0, 6, 0, 0, 0 };
    static cilist io___58 = { 0, 6, 0, 0, 0 };
    static cilist io___59 = { 0, 5, 0, 0, 0 };
    static cilist io___61 = { 0, 6, 0, 0, 0 };
    static cilist io___62 = { 0, 6, 0, 0, 0 };
    static cilist io___63 = { 0, 6, 0, 0, 0 };
    static cilist io___64 = { 0, 6, 0, 0, 0 };
    static cilist io___65 = { 0, 5, 0, 0, 0 };
    static cilist io___72 = { 0, 6, 0, 0, 0 };
    static cilist io___73 = { 0, 5, 0, 0, 0 };
    static cilist io___74 = { 0, 6, 0, 0, 0 };
    static cilist io___75 = { 0, 5, 0, "(A)", 0 };
    static cilist io___97 = { 0, 0, 0, fmt_930, 0 };
    static cilist io___98 = { 0, 0, 0, fmt_931, 0 };
    static cilist io___100 = { 0, 0, 0, fmt_937, 0 };
    static cilist io___102 = { 0, 0, 0, fmt_939, 0 };
    static cilist io___105 = { 0, 0, 0, fmt_941, 0 };
    static cilist io___108 = { 0, 0, 0, fmt_943, 0 };
    static cilist io___111 = { 0, 0, 0, fmt_945, 0 };
    static cilist io___114 = { 0, 0, 0, fmt_947, 0 };
    static cilist io___117 = { 0, 0, 0, fmt_949, 0 };
    static cilist io___118 = { 0, 0, 0, fmt_932, 0 };
    static cilist io___119 = { 0, 0, 0, fmt_933, 0 };
    static cilist io___120 = { 0, 0, 0, fmt_934, 0 };
    static cilist io___123 = { 0, 0, 0, fmt_935, 0 };
    static cilist io___125 = { 0, 6, 0, 0, 0 };
    static cilist io___126 = { 0, 6, 0, 0, 0 };
    static cilist io___127 = { 0, 6, 0, 0, 0 };
    static cilist io___128 = { 0, 6, 0, 0, 0 };
    static cilist io___129 = { 0, 6, 0, 0, 0 };
    static cilist io___130 = { 0, 5, 0, 0, 0 };
    static cilist io___131 = { 0, 6, 0, 0, 0 };
    static cilist io___132 = { 0, 6, 0, 0, 0 };
    static cilist io___133 = { 0, 5, 0, 0, 0 };
    static cilist io___139 = { 0, 6, 0, 0, 0 };
    static cilist io___140 = { 0, 6, 0, 0, 0 };
    static cilist io___141 = { 0, 5, 0, 0, 0 };
    static cilist io___147 = { 0, 6, 0, 0, 0 };
    static cilist io___148 = { 0, 5, 0, 0, 0 };
    static cilist io___149 = { 0, 6, 0, 0, 0 };
    static cilist io___150 = { 0, 6, 0, 0, 0 };
    static cilist io___151 = { 0, 5, 0, 0, 0 };
    static cilist io___152 = { 0, 0, 0, fmt_958, 0 };
    static cilist io___155 = { 0, 0, 0, fmt_959, 0 };
    static cilist io___156 = { 0, 0, 0, fmt_960, 0 };
    static cilist io___157 = { 0, 0, 0, fmt_959, 0 };
    static cilist io___158 = { 0, 0, 0, fmt_961, 0 };
    static cilist io___159 = { 0, 0, 0, fmt_962, 0 };
    static cilist io___160 = { 0, 0, 0, fmt_963, 0 };
    static cilist io___161 = { 0, 0, 0, 0, 0 };
    static cilist io___162 = { 0, 0, 0, fmt_964, 0 };
    static cilist io___163 = { 0, 0, 0, 0, 0 };
    static cilist io___164 = { 0, 6, 0, fmt_972, 0 };
    static cilist io___165 = { 0, 6, 0, fmt_973, 0 };
    static cilist io___166 = { 0, 6, 0, fmt_966, 0 };
    static cilist io___167 = { 0, 6, 0, fmt_967, 0 };
    static cilist io___168 = { 0, 6, 0, fmt_968, 0 };
    static cilist io___169 = { 0, 6, 0, fmt_969, 0 };
    static cilist io___170 = { 0, 6, 0, fmt_970, 0 };
    static cilist io___171 = { 0, 6, 0, fmt_971, 0 };



/*     This is a program for synthesising geomagnetic field values from the */
/*     International Geomagnetic Reference Field series of models as agreed */
/*     in December 2019 by IAGA Working Group V-MOD. */
/*     It is the 13th generation IGRF, ie the 12th revision. */
/*     The main-field models for 1900.0, 1905.0,..1940.0 and 2020.0 are */
/*     non-definitive, those for 1945.0, 1950.0,...2015.0 are definitive and */
/*     the secular-variation model for 2020.0 to 2025.0 is non-definitive. */

/*     Main-field models are to degree and order 10 (ie 120 coefficients) */
/*     for 1900.0-1995.0 and to 13 (ie 195 coefficients) for 2000.0 onwards. */
/*     The predictive secular-variation model is to degree and order 8 (ie 80 */
/*     coefficients). */

/*     Options include values at different locations at different */
/*     times (spot), values at same location at one year intervals */
/*     (time series), grid of values at one time (grid); geodetic or */
/*     geocentric coordinates, latitude & longitude entered as decimal */
/*     degrees or degrees & minutes (not in grid), choice of main field */
/*     or secular variation or both (grid only). */
/* Recent history of code: */
/*     Aug 2003: */
/*     Adapted from 8th generation version to include new maximum degree for */
/*     main-field models for 2000.0 and onwards and use WGS84 spheroid instead */
/*     of International Astronomical Union 1966 spheroid as recommended by IAGA */
/*     in July 2003. Reference radius remains as 6371.2 km - it is NOT the mean */
/*     radius (= 6371.0 km) but 6371.2 km is what is used in determining the */
/*     coefficients. */
/*     Dec 2004: */
/*     Adapted for 10th generation */
/*     Jul 2005: */
/*     1995.0 coefficients as published in igrf9coeffs.xls and igrf10coeffs.xls */
/*     now used in code - (Kimmo Korhonen spotted 1 nT difference in 11 coefficients) */
/*     Dec 2009: */
/*     Adapted for 11th generation */
/*     Dec 2014: */
/*     Adapted for 12th generation */
/*     Dec 2019 (W. Brown, BGS): */
/*     Adapted for 13th generation */
/*     Feb 2020 (W. Brown, BGS): */
/*     Correction of coefficient rounding for 2020 and 2020 SV values */



    s_wsle(&io___3);
    e_wsle();
    s_wsle(&io___4);
    do_lio(&c__9, &c__1, "**************************************************"
	    "****", (ftnlen)54);
    e_wsle();
    s_wsle(&io___5);
    do_lio(&c__9, &c__1, "*              IGRF SYNTHESIS PROGRAM             "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___6);
    do_lio(&c__9, &c__1, "*                                                 "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___7);
    do_lio(&c__9, &c__1, "* A program for the computation of geomagnetic    "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___8);
    do_lio(&c__9, &c__1, "* field elements from the International Geomagneti"
	    "c  *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___9);
    do_lio(&c__9, &c__1, "* Reference Field (13th generation) as revised in "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___10);
    do_lio(&c__9, &c__1, "* December 2019 by the IAGA Working Group V-MOD.  "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___11);
    do_lio(&c__9, &c__1, "*                                                 "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___12);
    do_lio(&c__9, &c__1, "* It is valid for dates from 1900.0 to 2025.0,    "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___13);
    do_lio(&c__9, &c__1, "* values up to 2030.0 will be computed but with   "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___14);
    do_lio(&c__9, &c__1, "* reduced accuracy. Values for dates before 1945.0"
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___15);
    do_lio(&c__9, &c__1, "* and after 2015.0 are non-definitive, otherwise t"
	    "he *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___16);
    do_lio(&c__9, &c__1, "* values are definitive.                          "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___17);
    do_lio(&c__9, &c__1, "*                                                 "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___18);
    do_lio(&c__9, &c__1, "* Susan Macmillan, William Brown                  "
	    "   *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___19);
    do_lio(&c__9, &c__1, "*                          British Geological Surv"
	    "ey *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___20);
    do_lio(&c__9, &c__1, "*                           IAGA Working Group V-M"
	    "OD *", (ftnlen)54);
    e_wsle();
    s_wsle(&io___21);
    do_lio(&c__9, &c__1, "**************************************************"
	    "****", (ftnlen)54);
    e_wsle();
    s_wsle(&io___22);
    e_wsle();
    s_wsle(&io___23);
    do_lio(&c__9, &c__1, "Enter name of output file (30 characters maximum)", 
	    (ftnlen)49);
    e_wsle();
    s_wsle(&io___24);
    do_lio(&c__9, &c__1, "or press \"Return\" for output to screen", (ftnlen)
	    38);
    e_wsle();
    s_rsfe(&io___25);
    do_fio(&c__1, fnm, (ftnlen)30);
    e_rsfe();
    if (*(unsigned char *)fnm == 32) {
	iu = 6;
    } else {
	iu = 2;
	o__1.oerr = 0;
	o__1.ounit = iu;
	o__1.ofnmlen = 30;
	o__1.ofnm = fnm;
	o__1.orl = 0;
	o__1.osta = 0;
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
    }
    fact = 57.295779505601047f;
    ncount = 0;

L10:
    s_wsle(&io___30);
    do_lio(&c__9, &c__1, "Enter value for coordinate system:", (ftnlen)34);
    e_wsle();
    s_wsle(&io___31);
    do_lio(&c__9, &c__1, "1 - geodetic (shape of Earth is approximated by a "
	    "spheroid)", (ftnlen)59);
    e_wsle();
    s_wsle(&io___32);
    do_lio(&c__9, &c__1, "2 - geocentric (shape of Earth is approximated by "
	    "a sphere)", (ftnlen)59);
    e_wsle();
    s_rsle(&io___33);
    do_lio(&c__3, &c__1, (char *)&itype, (ftnlen)sizeof(integer));
    e_rsle();
    if (itype < 1 || itype > 2) {
	goto L10;
    }
    if (itype == 1) {
	s_copy(type__, " geodetic  ", (ftnlen)11, (ftnlen)11);
    }
    if (itype == 2) {
	s_copy(type__, " geocentric", (ftnlen)11, (ftnlen)11);
    }

L20:
    s_wsle(&io___36);
    do_lio(&c__9, &c__1, "Choose an option:", (ftnlen)17);
    e_wsle();
    s_wsle(&io___37);
    do_lio(&c__9, &c__1, "1 - values at one or more locations & dates", (
	    ftnlen)43);
    e_wsle();
    s_wsle(&io___38);
    do_lio(&c__9, &c__1, "2 - values at yearly intervals at one location", (
	    ftnlen)46);
    e_wsle();
    s_wsle(&io___39);
    do_lio(&c__9, &c__1, "3 - values on a latitude/longitude grid at one date"
	    , (ftnlen)51);
    e_wsle();
    s_rsle(&io___40);
    do_lio(&c__3, &c__1, (char *)&iopt, (ftnlen)sizeof(integer));
    e_rsle();
    if (iopt < 1 || iopt > 3) {
	goto L20;
    }
    if (iopt == 3) {
	goto L150;
    }

L30:
    s_wsle(&io___42);
    do_lio(&c__9, &c__1, "Enter value for format of latitudes and longitudes:"
	    , (ftnlen)51);
    e_wsle();
    s_wsle(&io___43);
    do_lio(&c__9, &c__1, "1 - in degrees & minutes", (ftnlen)24);
    e_wsle();
    s_wsle(&io___44);
    do_lio(&c__9, &c__1, "2 - in decimal degrees", (ftnlen)22);
    e_wsle();
    s_rsle(&io___45);
    do_lio(&c__3, &c__1, (char *)&idm, (ftnlen)sizeof(integer));
    e_rsle();
    if (idm < 1 || idm > 2) {
	goto L30;
    }
    if (ncount == 0) {
	goto L50;
    }

L40:
    s_wsle(&io___47);
    do_lio(&c__9, &c__1, "Do you want values for another date & position? (y"
	    "/n)", (ftnlen)53);
    e_wsle();
    s_rsfe(&io___48);
    do_fio(&c__1, ia, (ftnlen)1);
    e_rsfe();
    if (*(unsigned char *)ia != 'Y' && *(unsigned char *)ia != 'y' && *(
	    unsigned char *)ia != 'N' && *(unsigned char *)ia != 'n') {
	goto L40;
    }
    if (*(unsigned char *)ia == 'N' || *(unsigned char *)ia == 'n') {
	io___50.ciunit = iu;
	s_wsfe(&io___50);
	e_wsfe();
	io___51.ciunit = iu;
	s_wsfe(&io___51);
	e_wsfe();
	if (itype == 2) {
	    io___52.ciunit = iu;
	    s_wsle(&io___52);
	    do_lio(&c__9, &c__1, "These elements are relative to the geocent"
		    "ric coordinate system", (ftnlen)63);
	    e_wsle();
	} else {
	    io___53.ciunit = iu;
	    s_wsle(&io___53);
	    e_wsle();
	}
	s_stop("", (ftnlen)0);
    }

L50:
    ncount = 1;
    if (iopt != 2) {
	s_wsle(&io___54);
	do_lio(&c__9, &c__1, "Enter date in years A.D.", (ftnlen)24);
	e_wsle();
	s_rsle(&io___55);
	do_lio(&c__5, &c__1, (char *)&date, (ftnlen)sizeof(doublereal));
	e_rsle();
	if (date < dtmn || date > dtmx) {
	    goto L209;
	}
    }
    if (itype == 1) {
	s_wsle(&io___57);
	do_lio(&c__9, &c__1, "Enter altitude in km", (ftnlen)20);
	e_wsle();
    } else {
	s_wsle(&io___58);
	do_lio(&c__9, &c__1, "Enter radial distance in km (>3485 km)", (
		ftnlen)38);
	e_wsle();
    }
    s_rsle(&io___59);
    do_lio(&c__5, &c__1, (char *)&alt, (ftnlen)sizeof(doublereal));
    e_rsle();
    if (itype == 2 && alt <= 3485.f) {
	goto L210;
    }

    if (idm == 1) {
	s_wsle(&io___61);
	do_lio(&c__9, &c__1, "Enter latitude & longitude in degrees & minutes"
		, (ftnlen)47);
	e_wsle();
	s_wsle(&io___62);
	do_lio(&c__9, &c__1, "(if either latitude or longitude is between -1",
		 (ftnlen)46);
	e_wsle();
	s_wsle(&io___63);
	do_lio(&c__9, &c__1, "and 0 degrees, enter the minutes as negative).",
		 (ftnlen)46);
	e_wsle();
	s_wsle(&io___64);
	do_lio(&c__9, &c__1, "Enter 4 integers", (ftnlen)16);
	e_wsle();
	s_rsle(&io___65);
	do_lio(&c__3, &c__1, (char *)&ltd, (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&ltm, (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&lnd, (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&lnm, (ftnlen)sizeof(integer));
	e_rsle();
	if (ltd < -90 || ltd > 90 || ltm <= -60 || ltm >= 60) {
	    goto L204;
	}
	if (lnd < -360 || lnd > 360 || lnm <= -60 || lnm >= 60) {
	    goto L205;
	}
	if (ltm < 0 && ltd != 0) {
	    goto L204;
	}
	if (lnm < 0 && lnd != 0) {
	    goto L205;
	}
	dmddec_(&ltd, &ltm, &xlt);
	dmddec_(&lnd, &lnm, &xln);
    } else {
	s_wsle(&io___72);
	do_lio(&c__9, &c__1, "Enter latitude & longitude in decimal degrees", 
		(ftnlen)45);
	e_wsle();
	s_rsle(&io___73);
	do_lio(&c__5, &c__1, (char *)&xlt, (ftnlen)sizeof(doublereal));
	do_lio(&c__5, &c__1, (char *)&xln, (ftnlen)sizeof(doublereal));
	e_rsle();
	if (xlt < -90.f || xlt > 90.f) {
	    goto L202;
	}
	if (xln < -360.f || xln > 360.f) {
	    goto L203;
	}
    }

    s_wsle(&io___74);
    do_lio(&c__9, &c__1, "Enter place name (20 characters maximum)", (ftnlen)
	    40);
    e_wsle();
    s_rsfe(&io___75);
    do_fio(&c__1, name__, (ftnlen)20);
    e_rsfe();
    clt = 90.f - xlt;
    if (clt < 0.f || clt > 180.f) {
	goto L204;
    }
    if (xln <= -360.f || xln >= 360.f) {
	goto L205;
    }
    if (iopt == 2) {
	goto L60;
    }

    igrf13syn_(&c__0, &date, &itype, &alt, &clt, &xln, &x, &y, &z__, &f);
    d__ = fact * atan2(y, x);
    h__ = sqrt(x * x + y * y);
    s = fact * atan2(z__, h__);
    ddecdm_(&d__, &idec, &idecm);
    ddecdm_(&s, &inc, &incm);

    igrf13syn_(&c__1, &date, &itype, &alt, &clt, &xln, &dx, &dy, &dz, &f1);
    dd = fact * 60.f * (x * dy - y * dx) / (h__ * h__);
    dh = (x * dx + y * dy) / h__;
    ds = fact * 60.f * (h__ * dz - z__ * dh) / (f * f);
    df = (h__ * dh + z__ * dz) / f;

    if (idm == 1) {
	io___97.ciunit = iu;
	s_wsfe(&io___97);
	do_fio(&c__1, (char *)&date, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&ltd, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ltm, (ftnlen)sizeof(integer));
	do_fio(&c__1, type__, (ftnlen)11);
	do_fio(&c__1, (char *)&lnd, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&lnm, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&alt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, name__, (ftnlen)20);
	e_wsfe();
    } else {
	io___98.ciunit = iu;
	s_wsfe(&io___98);
	do_fio(&c__1, (char *)&date, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&xlt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, type__, (ftnlen)11);
	do_fio(&c__1, (char *)&xln, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&alt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, name__, (ftnlen)20);
	e_wsfe();
    }

    idd = i_dnnt(&dd);
    io___100.ciunit = iu;
    s_wsfe(&io___100);
    do_fio(&c__1, (char *)&idec, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&idecm, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&idd, (ftnlen)sizeof(integer));
    e_wsfe();

    ids = i_dnnt(&ds);
    io___102.ciunit = iu;
    s_wsfe(&io___102);
    do_fio(&c__1, (char *)&inc, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&incm, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&ids, (ftnlen)sizeof(integer));
    e_wsfe();

    ih = i_dnnt(&h__);
    idh = i_dnnt(&dh);
    io___105.ciunit = iu;
    s_wsfe(&io___105);
    do_fio(&c__1, (char *)&ih, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&idh, (ftnlen)sizeof(integer));
    e_wsfe();

    ix = i_dnnt(&x);
    idx = i_dnnt(&dx);
    io___108.ciunit = iu;
    s_wsfe(&io___108);
    do_fio(&c__1, (char *)&ix, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&idx, (ftnlen)sizeof(integer));
    e_wsfe();

    iy = i_dnnt(&y);
    idy = i_dnnt(&dy);
    io___111.ciunit = iu;
    s_wsfe(&io___111);
    do_fio(&c__1, (char *)&iy, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&idy, (ftnlen)sizeof(integer));
    e_wsfe();

    iz = i_dnnt(&z__);
    idz = i_dnnt(&dz);
    io___114.ciunit = iu;
    s_wsfe(&io___114);
    do_fio(&c__1, (char *)&iz, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&idz, (ftnlen)sizeof(integer));
    e_wsfe();

    nf = i_dnnt(&f);
    idf = i_dnnt(&df);
    io___117.ciunit = iu;
    s_wsfe(&io___117);
    do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&idf, (ftnlen)sizeof(integer));
    e_wsfe();

    goto L40;

L60:

/*     SERIES OF VALUES AT ONE LOCATION... */

    if (idm == 1) {
	io___118.ciunit = iu;
	s_wsfe(&io___118);
	do_fio(&c__1, (char *)&ltd, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ltm, (ftnlen)sizeof(integer));
	do_fio(&c__1, type__, (ftnlen)11);
	do_fio(&c__1, (char *)&lnd, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&lnm, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&alt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, name__, (ftnlen)20);
	e_wsfe();
    } else {
	io___119.ciunit = iu;
	s_wsfe(&io___119);
	do_fio(&c__1, (char *)&xlt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, type__, (ftnlen)11);
	do_fio(&c__1, (char *)&xln, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&alt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, name__, (ftnlen)20);
	e_wsfe();
    }
    io___120.ciunit = iu;
    s_wsfe(&io___120);
    e_wsfe();
    imx = (integer) (dtmx - dtmn - 5);
    i__1 = imx;
    for (i__ = 1; i__ <= i__1; ++i__) {
	date = dtmn - .5f + i__;
	igrf13syn_(&c__0, &date, &itype, &alt, &clt, &xln, &x, &y, &z__, &f);
	d__ = fact * atan2(y, x);
	h__ = sqrt(x * x + y * y);
	s = fact * atan2(z__, h__);
	ih = i_dnnt(&h__);
	ix = i_dnnt(&x);
	iy = i_dnnt(&y);
	iz = i_dnnt(&z__);
	nf = i_dnnt(&f);

	igrf13syn_(&c__1, &date, &itype, &alt, &clt, &xln, &dx, &dy, &dz, &f1)
		;
	dd = fact * 60.f * (x * dy - y * dx) / (h__ * h__);
	dh = (x * dx + y * dy) / h__;
	ds = fact * 60.f * (h__ * dz - z__ * dh) / (f * f);
	df = (h__ * dh + z__ * dz) / f;
	idd = i_dnnt(&dd);
	idh = i_dnnt(&dh);
	ids = i_dnnt(&ds);
	idx = i_dnnt(&dx);
	idy = i_dnnt(&dy);
	idz = i_dnnt(&dz);
	idf = i_dnnt(&df);

	io___123.ciunit = iu;
	s_wsfe(&io___123);
	do_fio(&c__1, (char *)&date, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&d__, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&idd, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&s, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&ids, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ih, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idh, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ix, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idx, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iy, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idy, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iz, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idz, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idf, (ftnlen)sizeof(integer));
	e_wsfe();
/* L70: */
    }
    ifl = 2;
    goto L158;

/*     GRID OF VALUES... */

L150:
    s_wsle(&io___125);
    do_lio(&c__9, &c__1, "Enter value for MF/SV flag:", (ftnlen)27);
    e_wsle();
    s_wsle(&io___126);
    do_lio(&c__9, &c__1, "0 for main field (MF)", (ftnlen)21);
    e_wsle();
    s_wsle(&io___127);
    do_lio(&c__9, &c__1, "1 for secular variation (SV)", (ftnlen)28);
    e_wsle();
    s_wsle(&io___128);
    do_lio(&c__9, &c__1, "2 for both", (ftnlen)10);
    e_wsle();
    s_wsle(&io___129);
    do_lio(&c__9, &c__1, "9 to quit", (ftnlen)9);
    e_wsle();
    s_rsle(&io___130);
    do_lio(&c__3, &c__1, (char *)&ifl, (ftnlen)sizeof(integer));
    e_rsle();
    if (ifl == 9) {
	s_stop("", (ftnlen)0);
    }
    if (ifl != 0 && ifl != 1 && ifl != 2) {
	goto L150;
    }

    s_wsle(&io___131);
    do_lio(&c__9, &c__1, "Enter initial value, final value & increment or", (
	    ftnlen)47);
    e_wsle();
    s_wsle(&io___132);
    do_lio(&c__9, &c__1, "decrement of latitude, in degrees & decimals", (
	    ftnlen)44);
    e_wsle();
    s_rsle(&io___133);
    do_lio(&c__5, &c__1, (char *)&xlti, (ftnlen)sizeof(doublereal));
    do_lio(&c__5, &c__1, (char *)&xltf, (ftnlen)sizeof(doublereal));
    do_lio(&c__5, &c__1, (char *)&xltd, (ftnlen)sizeof(doublereal));
    e_rsle();
    d__1 = xlti * 1e3f;
    lti = i_dnnt(&d__1);
    d__1 = xltf * 1e3f;
    ltf = i_dnnt(&d__1);
    d__1 = xltd * 1e3f;
    ltd = i_dnnt(&d__1);
    s_wsle(&io___139);
    do_lio(&c__9, &c__1, "Enter initial value, final value & increment or", (
	    ftnlen)47);
    e_wsle();
    s_wsle(&io___140);
    do_lio(&c__9, &c__1, "decrement of longitude, in degrees & decimals", (
	    ftnlen)45);
    e_wsle();
    s_rsle(&io___141);
    do_lio(&c__5, &c__1, (char *)&xlni, (ftnlen)sizeof(doublereal));
    do_lio(&c__5, &c__1, (char *)&xlnf, (ftnlen)sizeof(doublereal));
    do_lio(&c__5, &c__1, (char *)&xlnd, (ftnlen)sizeof(doublereal));
    e_rsle();
    d__1 = xlni * 1e3f;
    lni = i_dnnt(&d__1);
    d__1 = xlnf * 1e3f;
    lnf = i_dnnt(&d__1);
    d__1 = xlnd * 1e3f;
    lnd = i_dnnt(&d__1);
    if (lti < -90000 || lti > 90000) {
	goto L206;
    }
    if (ltf < -90000 || ltf > 90000) {
	goto L206;
    }
    if (lni < -360000 || lni > 360000) {
	goto L207;
    }
    if (lnf < -360000 || lnf > 360000) {
	goto L207;
    }
/* L98: */
    s_wsle(&io___147);
    do_lio(&c__9, &c__1, "Enter date in years A.D.", (ftnlen)24);
    e_wsle();
    s_rsle(&io___148);
    do_lio(&c__5, &c__1, (char *)&date, (ftnlen)sizeof(doublereal));
    e_rsle();
    if (date < dtmn || date > dtmx) {
	goto L209;
    }
    if (itype == 1) {
	s_wsle(&io___149);
	do_lio(&c__9, &c__1, "Enter altitude in km", (ftnlen)20);
	e_wsle();
    } else {
	s_wsle(&io___150);
	do_lio(&c__9, &c__1, "Enter radial distance in km (>3485 km)", (
		ftnlen)38);
	e_wsle();
    }
    s_rsle(&io___151);
    do_lio(&c__5, &c__1, (char *)&alt, (ftnlen)sizeof(doublereal));
    e_rsle();
    if (itype == 2 && alt <= 3485.f) {
	goto L210;
    }
    io___152.ciunit = iu;
    s_wsfe(&io___152);
    do_fio(&c__1, (char *)&date, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&alt, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, type__, (ftnlen)11);
    e_wsfe();

    lt = lti;
L151:
    xlt = (doublereal) lt;
    xlt *= .001f;
    clt = 90.f - xlt;
    if (clt < -.001f || clt > 180.001f) {
	goto L202;
    }
    ln = lni;
L152:
    xln = (doublereal) ln;
    xln *= .001f;
    if (xln <= -360.f) {
	xln += 360.f;
    }
    if (xln >= 360.f) {
	xln += -360.f;
    }
    igrf13syn_(&c__0, &date, &itype, &alt, &clt, &xln, &x, &y, &z__, &f);
    d__ = fact * atan2(y, x);
    h__ = sqrt(x * x + y * y);
    s = fact * atan2(z__, h__);
    ih = i_dnnt(&h__);
    ix = i_dnnt(&x);
    iy = i_dnnt(&y);
    iz = i_dnnt(&z__);
    nf = i_dnnt(&f);
    if (ifl == 0) {
	goto L153;
    }
    igrf13syn_(&c__1, &date, &itype, &alt, &clt, &xln, &dx, &dy, &dz, &f1);
    idx = i_dnnt(&dx);
    idy = i_dnnt(&dy);
    idz = i_dnnt(&dz);
    dd = fact * 60.f * (x * dy - y * dx) / (h__ * h__);
    idd = i_dnnt(&dd);
    dh = (x * dx + y * dy) / h__;
    idh = i_dnnt(&dh);
    ds = fact * 60.f * (h__ * dz - z__ * dh) / (f * f);
    ids = i_dnnt(&ds);
    df = (h__ * dh + z__ * dz) / f;
    idf = i_dnnt(&df);

L153:
    if (ifl == 0) {
	io___155.ciunit = iu;
	s_wsfe(&io___155);
	do_fio(&c__1, (char *)&xlt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&xln, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&d__, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&s, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&ih, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ix, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iy, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iz, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
	e_wsfe();
    }
    if (ifl == 1) {
	io___156.ciunit = iu;
	s_wsfe(&io___156);
	do_fio(&c__1, (char *)&xlt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&xln, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&idd, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ids, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idh, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idx, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idy, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idz, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idf, (ftnlen)sizeof(integer));
	e_wsfe();
    }
    if (ifl == 2) {
	io___157.ciunit = iu;
	s_wsfe(&io___157);
	do_fio(&c__1, (char *)&xlt, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&xln, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&d__, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&s, (ftnlen)sizeof(doublereal));
	do_fio(&c__1, (char *)&ih, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ix, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iy, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&iz, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&nf, (ftnlen)sizeof(integer));
	e_wsfe();
	io___158.ciunit = iu;
	s_wsfe(&io___158);
	do_fio(&c__1, (char *)&idd, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ids, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idh, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idx, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idy, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idz, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&idf, (ftnlen)sizeof(integer));
	e_wsfe();
    }

/* L154: */
    ln += lnd;
    if (lnd < 0) {
	goto L156;
    }
    if (ln <= lnf) {
	goto L152;
    }
L155:
    lt += ltd;
    if (ltd < 0) {
	goto L157;
    }
    if (lt - ltf <= 0) {
	goto L151;
    } else {
	goto L158;
    }
L156:
    if (ln - lnf >= 0) {
	goto L152;
    } else {
	goto L155;
    }
L157:
    if (lt >= ltf) {
	goto L151;
    }
L158:
    if (ifl == 0 || ifl == 2) {
	io___159.ciunit = iu;
	s_wsfe(&io___159);
	e_wsfe();
	if (ifl != 0) {
	    io___160.ciunit = iu;
	    s_wsfe(&io___160);
	    e_wsfe();
	}
	if (itype == 2) {
	    io___161.ciunit = iu;
	    s_wsle(&io___161);
	    do_lio(&c__9, &c__1, "These elements are relative to the geocent"
		    "ric coordinate system", (ftnlen)63);
	    e_wsle();
	}
    } else {
	io___162.ciunit = iu;
	s_wsfe(&io___162);
	e_wsfe();
	if (itype == 2) {
	    io___163.ciunit = iu;
	    s_wsle(&io___163);
	    do_lio(&c__9, &c__1, "These elements are relative to the geocent"
		    "ric coordinate system", (ftnlen)63);
	    e_wsle();
	}
    }
/* L159: */
    s_stop("", (ftnlen)0);

L209:
    s_wsfe(&io___164);
    do_fio(&c__1, (char *)&date, (ftnlen)sizeof(doublereal));
    e_wsfe();
    s_stop("", (ftnlen)0);

L210:
    s_wsfe(&io___165);
    do_fio(&c__1, (char *)&alt, (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&itype, (ftnlen)sizeof(integer));
    e_wsfe();
    s_stop("", (ftnlen)0);

L202:
    s_wsfe(&io___166);
    do_fio(&c__1, (char *)&xlt, (ftnlen)sizeof(doublereal));
    e_wsfe();
    s_stop("", (ftnlen)0);

L203:
    s_wsfe(&io___167);
    do_fio(&c__1, (char *)&xln, (ftnlen)sizeof(doublereal));
    e_wsfe();
    s_stop("", (ftnlen)0);

L204:
    s_wsfe(&io___168);
    do_fio(&c__1, (char *)&ltd, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&ltm, (ftnlen)sizeof(integer));
    e_wsfe();
    s_stop("", (ftnlen)0);

L205:
    s_wsfe(&io___169);
    do_fio(&c__1, (char *)&lnd, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&lnm, (ftnlen)sizeof(integer));
    e_wsfe();
    s_stop("", (ftnlen)0);

L206:
    s_wsfe(&io___170);
    do_fio(&c__1, (char *)&lti, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&ltf, (ftnlen)sizeof(integer));
    e_wsfe();
    s_stop("", (ftnlen)0);

L207:
    s_wsfe(&io___171);
    do_fio(&c__1, (char *)&lni, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&lnf, (ftnlen)sizeof(integer));
    e_wsfe();
    s_stop("", (ftnlen)0);

    return 0;
} /* MAIN__ */


/* Subroutine */ int dmddec_(integer *i__, integer *m, doublereal *x)
{
    static doublereal de, em;

    de = (doublereal) (*i__);
    em = (doublereal) (*m);
    if (*i__ < 0) {
	em = -em;
    }
    *x = de + em / 60.f;
    return 0;
} /* dmddec_ */


/* Subroutine */ int ddecdm_(doublereal *x, integer *i__, integer *m)
{
    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *);
    integer i_dnnt(doublereal *);

    /* Local variables */
    static doublereal t, dr, sig;
    static integer isig;

    sig = d_sign(&c_b433, x);
    dr = abs(*x);
    *i__ = (integer) dr;
    t = (doublereal) (*i__);
    d__1 = (dr - t) * 60.f;
    *m = i_dnnt(&d__1);
    if (*m == 60) {
	*m = 0;
	++(*i__);
    }
    isig = (integer) sig;
    if (*i__ != 0) {
	*i__ *= isig;
    } else {
	if (*m != 0) {
	    *m *= isig;
	}
    }
    return 0;
} /* ddecdm_ */

/* Subroutine */ int igrf13syn_(integer *isv, doublereal *date, integer *
	itype, doublereal *alt, doublereal *colat, doublereal *elong, 
	doublereal *x, doublereal *y, doublereal *z__, doublereal *f)
{
    /* Initialized data */

    static struct {
	doublereal e_1[3645];
	} equiv_25 = { -31543., -2298., 5922., -677., 2905., -1061., 924., 
		1121., 1022., -1469., -330., 1256., 3., 572., 523., 876., 
		628., 195., 660., -69., -361., -210., 134., -75., -184., 328.,
		 -210., 264., 53., 5., -33., -86., -124., -16., 3., 63., 61., 
		-9., -11., 83., -217., 2., -58., -35., 59., 36., -90., -69., 
		70., -55., -45., 0., -13., 34., -10., -41., -1., -21., 28., 
		18., -12., 6., -22., 11., 8., 8., -4., -14., -9., 7., 1., 
		-13., 2., 5., -9., 16., 5., -5., 8., -18., 8., 10., -20., 1., 
		14., -11., 5., 12., -3., 1., -2., -2., 8., 2., 10., -1., -2., 
		-1., 2., -3., -4., 2., 2., 1., -5., 2., -2., 6., 6., -4., 4., 
		0., 0., -2., 2., 4., 2., 0., 0., -6., -31464., -2298., 5909., 
		-728., 2928., -1086., 1041., 1065., 1037., -1494., -357., 
		1239., 34., 635., 480., 880., 643., 203., 653., -77., -380., 
		-201., 146., -65., -192., 328., -193., 259., 56., -1., -32., 
		-93., -125., -26., 11., 62., 60., -7., -11., 86., -221., 4., 
		-57., -32., 57., 32., -92., -67., 70., -54., -46., 0., -14., 
		33., -11., -41., 0., -20., 28., 18., -12., 6., -22., 11., 8., 
		8., -4., -15., -9., 7., 1., -13., 2., 5., -8., 16., 5., -5., 
		8., -18., 8., 10., -20., 1., 14., -11., 5., 12., -3., 1., -2.,
		 -2., 8., 2., 10., 0., -2., -1., 2., -3., -4., 2., 2., 1., 
		-5., 2., -2., 6., 6., -4., 4., 0., 0., -2., 2., 4., 2., 0., 
		0., -6., -31354., -2297., 5898., -769., 2948., -1128., 1176., 
		1e3, 1058., -1524., -389., 1223., 62., 705., 425., 884., 660.,
		 211., 644., -90., -400., -189., 160., -55., -201., 327., 
		-172., 253., 57., -9., -33., -102., -126., -38., 21., 62., 
		58., -5., -11., 89., -224., 5., -54., -29., 54., 28., -95., 
		-65., 71., -54., -47., 1., -14., 32., -12., -40., 1., -19., 
		28., 18., -13., 6., -22., 11., 8., 8., -4., -15., -9., 6., 1.,
		 -13., 2., 5., -8., 16., 5., -5., 8., -18., 8., 10., -20., 1.,
		 14., -11., 5., 12., -3., 1., -2., -2., 8., 2., 10., 0., -2., 
		-1., 2., -3., -4., 2., 2., 1., -5., 2., -2., 6., 6., -4., 4., 
		0., 0., -2., 2., 4., 2., 0., 0., -6., -31212., -2306., 5875., 
		-802., 2956., -1191., 1309., 917., 1084., -1559., -421., 
		1212., 84., 778., 360., 887., 678., 218., 631., -109., -416., 
		-173., 178., -51., -211., 327., -148., 245., 58., -16., -34., 
		-111., -126., -51., 32., 61., 57., -2., -10., 93., -228., 8., 
		-51., -26., 49., 23., -98., -62., 72., -54., -48., 2., -14., 
		31., -12., -38., 2., -18., 28., 19., -15., 6., -22., 11., 8., 
		8., -4., -15., -9., 6., 2., -13., 3., 5., -8., 16., 6., -5., 
		8., -18., 8., 10., -20., 1., 14., -11., 5., 12., -3., 1., -2.,
		 -2., 8., 2., 10., 0., -2., -1., 2., -3., -4., 2., 2., 1., 
		-5., 2., -2., 6., 6., -4., 4., 0., 0., -2., 1., 4., 2., 0., 
		0., -6., -31060., -2317., 5845., -839., 2959., -1259., 1407., 
		823., 1111., -1600., -445., 1205., 103., 839., 293., 889., 
		695., 220., 616., -134., -424., -153., 199., -57., -221., 
		326., -122., 236., 58., -23., -38., -119., -125., -62., 43., 
		61., 55., 0., -10., 96., -233., 11., -46., -22., 44., 18., 
		-101., -57., 73., -54., -49., 2., -14., 29., -13., -37., 4., 
		-16., 28., 19., -16., 6., -22., 11., 7., 8., -3., -15., -9., 
		6., 2., -14., 4., 5., -7., 17., 6., -5., 8., -19., 8., 10., 
		-20., 1., 14., -11., 5., 12., -3., 1., -2., -2., 9., 2., 10., 
		0., -2., -1., 2., -3., -4., 2., 2., 1., -5., 2., -2., 6., 6., 
		-4., 4., 0., 0., -2., 1., 4., 3., 0., 0., -6., -30926., 
		-2318., 5817., -893., 2969., -1334., 1471., 728., 1140., 
		-1645., -462., 1202., 119., 881., 229., 891., 711., 216., 
		601., -163., -426., -130., 217., -70., -230., 326., -96., 
		226., 58., -28., -44., -125., -122., -69., 51., 61., 54., 3., 
		-9., 99., -238., 14., -40., -18., 39., 13., -103., -52., 73., 
		-54., -50., 3., -14., 27., -14., -35., 5., -14., 29., 19., 
		-17., 6., -21., 11., 7., 8., -3., -15., -9., 6., 2., -14., 4.,
		 5., -7., 17., 7., -5., 8., -19., 8., 10., -20., 1., 14., 
		-11., 5., 12., -3., 1., -2., -2., 9., 2., 10., 0., -2., -1., 
		2., -3., -4., 2., 2., 1., -5., 2., -2., 6., 6., -4., 4., 0., 
		0., -2., 1., 4., 3., 0., 0., -6., -30805., -2316., 5808., 
		-951., 2980., -1424., 1517., 644., 1172., -1692., -480., 
		1205., 133., 907., 166., 896., 727., 205., 584., -195., -422.,
		 -109., 234., -90., -237., 327., -72., 218., 60., -32., -53., 
		-131., -118., -74., 58., 60., 53., 4., -9., 102., -242., 19., 
		-32., -16., 32., 8., -104., -46., 74., -54., -51., 4., -15., 
		25., -14., -34., 6., -12., 29., 18., -18., 6., -20., 11., 7., 
		8., -3., -15., -9., 5., 2., -14., 5., 5., -6., 18., 8., -5., 
		8., -19., 8., 10., -20., 1., 14., -12., 5., 12., -3., 1., -2.,
		 -2., 9., 3., 10., 0., -2., -2., 2., -3., -4., 2., 2., 1., 
		-5., 2., -2., 6., 6., -4., 4., 0., 0., -2., 1., 4., 3., 0., 
		0., -6., -30715., -2306., 5812., -1018., 2984., -1520., 1550.,
		 586., 1206., -1740., -494., 1215., 146., 918., 101., 903., 
		744., 188., 565., -226., -415., -90., 249., -114., -241., 
		329., -51., 211., 64., -33., -64., -136., -115., -76., 64., 
		59., 53., 4., -8., 104., -246., 25., -25., -15., 25., 4., 
		-106., -40., 74., -53., -52., 4., -17., 23., -14., -33., 7., 
		-11., 29., 18., -19., 6., -19., 11., 7., 8., -3., -15., -9., 
		5., 1., -15., 6., 5., -6., 18., 8., -5., 7., -19., 8., 10., 
		-20., 1., 15., -12., 5., 11., -3., 1., -3., -2., 9., 3., 11., 
		0., -2., -2., 2., -3., -4., 2., 2., 1., -5., 2., -2., 6., 6., 
		-4., 4., 0., 0., -1., 2., 4., 3., 0., 0., -6., -30654., 
		-2292., 5821., -1106., 2981., -1614., 1566., 528., 1240., 
		-1790., -499., 1232., 163., 916., 43., 914., 762., 169., 550.,
		 -252., -405., -72., 265., -141., -241., 334., -33., 208., 
		71., -33., -75., -141., -113., -76., 69., 57., 54., 4., -7., 
		105., -249., 33., -18., -15., 18., 0., -107., -33., 74., -53.,
		 -52., 4., -18., 20., -14., -31., 7., -9., 29., 17., -20., 5.,
		 -19., 11., 7., 8., -3., -14., -10., 5., 1., -15., 6., 5., 
		-5., 19., 9., -5., 7., -19., 8., 10., -21., 1., 15., -12., 5.,
		 11., -3., 1., -3., -2., 9., 3., 11., 1., -2., -2., 2., -3., 
		-4., 2., 2., 1., -5., 2., -2., 6., 6., -4., 4., 0., 0., -1., 
		2., 4., 3., 0., 0., -6., -30594., -2285., 5810., -1244., 
		2990., -1702., 1578., 477., 1282., -1834., -499., 1255., 186.,
		 913., -11., 944., 776., 144., 544., -276., -421., -55., 304.,
		 -178., -253., 346., -12., 194., 95., -20., -67., -142., 
		-119., -82., 82., 59., 57., 6., 6., 100., -246., 16., -25., 
		-9., 21., -16., -104., -39., 70., -40., -45., 0., -18., 0., 
		2., -29., 6., -10., 28., 15., -17., 29., -22., 13., 7., 12., 
		-8., -21., -5., -12., 9., -7., 7., 2., -10., 18., 7., 3., 2., 
		-11., 5., -21., -27., 1., 17., -11., 29., 3., -9., 16., 4., 
		-3., 9., -4., 6., -3., 1., -4., 8., -3., 11., 5., 1., 1., 2., 
		-20., -5., -1., -1., -6., 8., 6., -1., -4., -3., -2., 5., 0., 
		-2., -2., -30554., -2250., 5815., -1341., 2998., -1810., 
		1576., 381., 1297., -1889., -476., 1274., 206., 896., -46., 
		954., 792., 136., 528., -278., -408., -37., 303., -210., 
		-240., 349., 3., 211., 103., -20., -87., -147., -122., -76., 
		80., 54., 57., -1., 4., 99., -247., 33., -16., -12., 12., 
		-12., -105., -30., 65., -55., -35., 2., -17., 1., 0., -40., 
		10., -7., 36., 5., -18., 19., -16., 22., 15., 5., -4., -22., 
		-1., 0., 11., -21., 15., -8., -13., 17., 5., -4., -1., -17., 
		3., -7., -24., -1., 19., -25., 12., 10., 2., 5., 2., -5., 8., 
		-2., 8., 3., -11., 8., -7., -8., 4., 13., -1., -2., 13., -10.,
		 -4., 2., 4., -3., 12., 6., 3., -3., 2., 6., 10., 11., 3., 8.,
		 -30500., -2215., 5820., -1440., 3003., -1898., 1581., 291., 
		1302., -1944., -462., 1288., 216., 882., -83., 958., 796., 
		133., 510., -274., -397., -23., 290., -230., -229., 360., 15.,
		 230., 110., -23., -98., -152., -121., -69., 78., 47., 57., 
		-9., 3., 96., -247., 48., -8., -16., 7., -12., -107., -24., 
		65., -56., -50., 2., -24., 10., -4., -32., 8., -11., 28., 9., 
		-20., 18., -18., 11., 9., 10., -6., -15., -14., 5., 6., -23., 
		10., 3., -7., 23., 6., -4., 9., -13., 4., 9., -11., -4., 12., 
		-5., 7., 2., 6., 4., -2., 1., 10., 2., 7., 2., -6., 5., 5., 
		-3., -5., -4., -1., 0., 2., -8., -3., -2., 7., -4., 4., 1., 
		-2., -3., 6., 7., -2., -1., 0., -3., -30421., -2169., 5791., 
		-1555., 3002., -1967., 1590., 206., 1302., -1992., -414., 
		1289., 224., 878., -130., 957., 800., 135., 504., -278., 
		-394., 3., 269., -255., -222., 362., 16., 242., 125., -26., 
		-117., -156., -114., -63., 81., 46., 58., -10., 1., 99., 
		-237., 60., -1., -20., -2., -11., -113., -17., 67., -56., 
		-55., 5., -28., 15., -6., -32., 7., -7., 23., 17., -18., 8., 
		-17., 15., 6., 11., -4., -14., -11., 7., 2., -18., 10., 4., 
		-5., 23., 10., 1., 8., -20., 4., 6., -18., 0., 12., -9., 2., 
		1., 0., 4., -3., -1., 9., -2., 8., 3., 0., -1., 5., 1., -3., 
		4., 4., 1., 0., 0., -1., 2., 4., -5., 6., 1., 1., -1., -1., 
		6., 2., 0., 0., -7., -30334., -2119., 5776., -1662., 2997., 
		-2016., 1594., 114., 1297., -2038., -404., 1292., 240., 856., 
		-165., 957., 804., 148., 479., -269., -390., 13., 252., -269.,
		 -219., 358., 19., 254., 128., -31., -126., -157., -97., -62.,
		 81., 45., 61., -11., 8., 100., -228., 68., 4., -32., 1., -8.,
		 -111., -7., 75., -57., -61., 4., -27., 13., -2., -26., 6., 
		-6., 26., 13., -23., 1., -12., 13., 5., 7., -4., -12., -14., 
		9., 0., -16., 8., 4., -1., 24., 11., -3., 4., -17., 8., 10., 
		-22., 2., 15., -13., 7., 10., -4., -1., -5., -1., 10., 5., 
		10., 1., -4., -2., 1., -2., -3., 2., 2., 1., -5., 2., -2., 6.,
		 4., -4., 4., 0., 0., -2., 2., 3., 2., 0., 0., -6., -30220., 
		-2068., 5737., -1781., 3e3, -2047., 1611., 25., 1287., -2091.,
		 -366., 1278., 251., 838., -196., 952., 800., 167., 461., 
		-266., -395., 26., 234., -279., -216., 359., 26., 262., 139., 
		-42., -139., -160., -91., -56., 83., 43., 64., -12., 15., 
		100., -212., 72., 2., -37., 3., -6., -112., 1., 72., -57., 
		-70., 1., -27., 14., -4., -22., 8., -2., 23., 13., -23., -2., 
		-11., 14., 6., 7., -2., -15., -13., 6., -3., -17., 5., 6., 0.,
		 21., 11., -6., 3., -16., 8., 10., -21., 2., 16., -12., 6., 
		10., -4., -1., -5., 0., 10., 3., 11., 1., -2., -1., 1., -3., 
		-3., 1., 2., 1., -5., 3., -1., 4., 6., -4., 4., 0., 1., -1., 
		0., 3., 3., 1., -1., -4., -30100., -2013., 5675., -1902., 
		3010., -2067., 1632., -68., 1276., -2144., -333., 1260., 262.,
		 830., -223., 946., 791., 191., 438., -265., -405., 39., 216.,
		 -288., -218., 356., 31., 264., 148., -59., -152., -159., 
		-83., -49., 88., 45., 66., -13., 28., 99., -198., 75., 1., 
		-41., 6., -4., -111., 11., 71., -56., -77., 1., -26., 16., 
		-5., -14., 10., 0., 22., 12., -23., -5., -12., 14., 6., 6., 
		-1., -16., -12., 4., -8., -19., 4., 6., 0., 18., 10., -10., 
		1., -17., 7., 10., -21., 2., 16., -12., 7., 10., -4., -1., 
		-5., -1., 10., 4., 11., 1., -3., -2., 1., -3., -3., 1., 2., 
		1., -5., 3., -2., 4., 5., -4., 4., -1., 1., -1., 0., 3., 3., 
		1., -1., -5., -29992., -1956., 5604., -1997., 3027., -2129., 
		1663., -200., 1281., -2180., -336., 1251., 271., 833., -252., 
		938., 782., 212., 398., -257., -419., 53., 199., -297., -218.,
		 357., 46., 261., 150., -74., -151., -162., -78., -48., 92., 
		48., 66., -15., 42., 93., -192., 71., 4., -43., 14., -2., 
		-108., 17., 72., -59., -82., 2., -27., 21., -5., -12., 16., 
		1., 18., 11., -23., -2., -10., 18., 6., 7., 0., -18., -11., 
		4., -7., -22., 4., 9., 3., 16., 6., -13., -1., -15., 5., 10., 
		-21., 1., 16., -12., 9., 9., -5., -3., -6., -1., 9., 7., 10., 
		2., -6., -5., 2., -4., -4., 1., 2., 0., -5., 3., -2., 6., 5., 
		-4., 3., 0., 1., -1., 2., 4., 3., 0., 0., -6., -29873., 
		-1905., 5500., -2072., 3044., -2197., 1687., -306., 1296., 
		-2208., -310., 1247., 284., 829., -297., 936., 780., 232., 
		361., -249., -424., 69., 170., -297., -214., 355., 47., 253., 
		150., -93., -154., -164., -75., -46., 95., 53., 65., -16., 
		51., 88., -185., 69., 4., -48., 16., -1., -102., 21., 74., 
		-62., -83., 3., -27., 24., -2., -6., 20., 4., 17., 10., -23., 
		0., -7., 21., 6., 8., 0., -19., -11., 5., -9., -23., 4., 11., 
		4., 14., 4., -15., -4., -11., 5., 10., -21., 1., 15., -12., 
		9., 9., -6., -3., -6., -1., 9., 7., 9., 1., -7., -5., 2., -4.,
		 -4., 1., 3., 0., -5., 3., -2., 6., 5., -4., 3., 0., 1., -1., 
		2., 4., 3., 0., 0., -6., -29775., -1848., 5406., -2131., 
		3059., -2279., 1686., -373., 1314., -2239., -284., 1248., 
		293., 802., -352., 939., 780., 247., 325., -240., -423., 84., 
		141., -299., -214., 353., 46., 245., 154., -109., -153., 
		-165., -69., -36., 97., 61., 65., -16., 59., 82., -178., 69., 
		3., -52., 18., 1., -96., 24., 77., -64., -80., 2., -26., 26., 
		0., -1., 21., 5., 17., 9., -23., 0., -4., 23., 5., 10., -1., 
		-19., -10., 6., -12., -22., 3., 12., 4., 12., 2., -16., -6., 
		-10., 4., 9., -20., 1., 15., -12., 11., 9., -7., -4., -7., 
		-2., 9., 7., 8., 1., -7., -6., 2., -3., -4., 2., 2., 1., -5., 
		3., -2., 6., 4., -4., 3., 0., 1., -2., 3., 3., 3., -1., 0., 
		-6., -29692., -1784., 5306., -2200., 3070., -2366., 1681., 
		-413., 1335., -2267., -262., 1249., 302., 759., -427., 940., 
		780., 262., 290., -236., -418., 97., 122., -306., -214., 352.,
		 46., 235., 165., -118., -143., -166., -55., -17., 107., 68., 
		67., -17., 68., 72., -170., 67., -1., -58., 19., 1., -93., 
		36., 77., -72., -69., 1., -25., 28., 4., 5., 24., 4., 17., 8.,
		 -24., -2., -6., 25., 6., 11., -6., -21., -9., 8., -14., -23.,
		 9., 15., 6., 11., -5., -16., -7., -4., 4., 9., -20., 3., 15.,
		 -10., 12., 8., -6., -8., -8., -1., 8., 10., 5., -2., -8., 
		-8., 3., -3., -6., 1., 2., 0., -4., 4., -1., 5., 4., -5., 2., 
		-1., 2., -2., 5., 1., 1., -2., 0., -7., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., -29619.4, -1728.2, 
		5186.1, -2267.7, 3068.4, -2481.6, 1670.9, -458., 1339.6, 
		-2288., -227.6, 1252.1, 293.4, 714.5, -491.1, 932.3, 786.8, 
		272.6, 250., -231.9, -403., 119.8, 111.3, -303.8, -218.8, 
		351.4, 43.8, 222.3, 171.9, -130.4, -133.1, -168.6, -39.3, 
		-12.9, 106.3, 72.3, 68.2, -17.4, 74.2, 63.7, -160.9, 65.1, 
		-5.9, -61.2, 16.9, .7, -90.4, 43.8, 79., -74., -64.6, 0., 
		-24.2, 33.3, 6.2, 9.1, 24., 6.9, 14.8, 7.3, -25.4, -1.2, -5.8,
		 24.4, 6.6, 11.9, -9.2, -21.5, -7.9, 8.5, -16.6, -21.5, 9.1, 
		15.5, 7., 8.9, -7.9, -14.9, -7., -2.1, 5., 9.4, -19.7, 3., 
		13.4, -8.4, 12.5, 6.3, -6.2, -8.9, -8.4, -1.5, 8.4, 9.3, 3.8, 
		-4.3, -8.2, -8.2, 4.8, -2.6, -6., 1.7, 1.7, 0., -3.1, 4., -.5,
		 4.9, 3.7, -5.9, 1., -1.2, 2., -2.9, 4.2, .2, .3, -2.2, -1.1, 
		-7.4, 2.7, -1.7, .1, -1.9, 1.3, 1.5, -.9, -.1, -2.6, .1, .9, 
		-.7, -.7, .7, -2.8, 1.7, -.9, .1, -1.2, 1.2, -1.9, 4., -.9, 
		-2.2, -.3, -.4, .2, .3, .9, 2.5, -.2, -2.6, .9, .7, -.5, .3, 
		.3, 0., -.3, 0., -.4, .3, -.1, -.9, -.2, -.4, -.4, .8, -.2, 
		-.9, -.9, .3, .2, .1, 1.8, -.4, -.4, 1.3, -1., -.4, -.1, .7, 
		.7, -.4, .3, .3, .6, -.1, .3, .4, -.2, 0., -.5, .1, -.9, 
		-29554.63, -1669.05, 5077.99, -2337.24, 3047.69, -2594.5, 
		1657.76, -515.43, 1336.3, -2305.83, -198.86, 1246.39, 269.72, 
		672.51, -524.72, 920.55, 797.96, 282.07, 210.65, -225.23, 
		-379.86, 145.15, 100., -305.36, -227., 354.41, 42.72, 208.95, 
		180.25, -136.54, -123.45, -168.05, -19.57, -13.55, 103.85, 
		73.6, 69.56, -20.33, 76.74, 54.75, -151.34, 63.63, -14.58, 
		-63.53, 14.58, .24, -86.36, 50.94, 79.88, -74.46, -61.14, 
		-1.65, -22.57, 38.73, 6.82, 12.3, 25.35, 9.37, 10.93, 5.42, 
		-26.32, 1.94, -4.64, 24.8, 7.62, 11.2, -11.73, -20.88, -6.88, 
		9.83, -18.11, -19.71, 10.17, 16.22, 9.36, 7.61, -11.25, 
		-12.76, -4.87, -.06, 5.58, 9.76, -20.11, 3.58, 12.69, -6.94, 
		12.67, 5.01, -6.72, -10.76, -8.16, -1.25, 8.1, 8.76, 2.92, 
		-6.66, -7.73, -9.22, 6.01, -2.17, -6.12, 2.19, 1.42, .1, 
		-2.35, 4.46, -.15, 4.76, 3.06, -6.58, .29, -1.01, 2.06, -3.47,
		 3.77, -.86, -.21, -2.31, -2.09, -7.93, 2.95, -1.6, .26, 
		-1.88, 1.44, 1.44, -.77, -.31, -2.27, .29, .9, -.79, -.58, 
		.53, -2.69, 1.8, -1.08, .16, -1.58, .96, -1.9, 3.99, -1.39, 
		-2.15, -.29, -.55, .21, .23, .89, 2.38, -.38, -2.63, .96, .61,
		 -.3, .4, .46, .01, -.35, .02, -.36, .28, .08, -.87, -.49, 
		-.34, -.08, .88, -.16, -.88, -.76, .3, .33, .28, 1.72, -.43, 
		-.54, 1.18, -1.07, -.37, -.04, .75, .63, -.26, .21, .35, .53, 
		-.05, .38, .41, -.22, -.1, -.57, -.18, -.82, -29496.57, 
		-1586.42, 4944.26, -2396.06, 3026.34, -2708.54, 1668.17, 
		-575.73, 1339.85, -2326.54, -160.4, 1232.1, 251.75, 633.73, 
		-537.03, 912.66, 808.97, 286.48, 166.58, -211.03, -356.83, 
		164.46, 89.4, -309.72, -230.87, 357.29, 44.58, 200.26, 189.01,
		 -141.05, -118.06, -163.17, -.01, -8.03, 101.04, 72.78, 68.69,
		 -20.9, 75.92, 44.18, -141.4, 61.54, -22.83, -66.26, 13.1, 
		3.02, -78.09, 55.4, 80.44, -75., -57.8, -4.55, -21.2, 45.24, 
		6.54, 14., 24.96, 10.46, 7.03, 1.64, -27.61, 4.92, -3.28, 
		24.41, 8.21, 10.84, -14.5, -20.03, -5.59, 11.83, -19.34, 
		-17.41, 11.61, 16.71, 10.85, 6.96, -14.05, -10.74, -3.54, 
		1.64, 5.5, 9.45, -20.54, 3.45, 11.51, -5.27, 12.75, 3.13, 
		-7.14, -12.38, -7.42, -.76, 7.97, 8.43, 2.14, -8.42, -6.08, 
		-10.08, 7.01, -1.94, -6.24, 2.73, .89, -.1, -1.07, 4.71, -.16,
		 4.44, 2.45, -7.22, -.33, -.96, 2.13, -3.95, 3.09, -1.99, 
		-1.03, -1.97, -2.8, -8.31, 3.05, -1.48, .13, -2.03, 1.67, 
		1.65, -.66, -.51, -1.76, .54, .85, -.79, -.39, .37, -2.51, 
		1.79, -1.27, .12, -2.11, .75, -1.94, 3.75, -1.86, -2.12, -.21,
		 -.87, .3, .27, 1.04, 2.13, -.63, -2.49, .95, .49, -.11, .59, 
		.52, 0., -.39, .13, -.37, .27, .21, -.86, -.77, -.23, .04, 
		.87, -.09, -.89, -.87, .31, .3, .42, 1.66, -.45, -.59, 1.08, 
		-1.14, -.31, -.07, .78, .54, -.18, .1, .38, .49, .02, .44, 
		.42, -.25, -.26, -.53, -.26, -.79, -29441.46, -1501.77, 
		4795.99, -2445.88, 3012.2, -2845.41, 1676.35, -642.17, 
		1350.33, -2352.26, -115.29, 1225.85, 245.04, 581.69, -538.7, 
		907.42, 813.68, 283.54, 120.49, -188.43, -334.85, 180.95, 
		70.38, -329.23, -232.91, 360.14, 46.98, 192.35, 196.98, 
		-140.94, -119.14, -157.4, 15.98, 4.3, 100.12, 69.55, 67.57, 
		-20.61, 72.79, 33.3, -129.85, 58.74, -28.93, -66.64, 13.14, 
		7.35, -70.85, 62.41, 81.29, -75.99, -54.27, -6.79, -19.53, 
		51.82, 5.59, 15.07, 24.45, 9.32, 3.27, -2.88, -27.5, 6.61, 
		-2.32, 23.98, 8.89, 10.04, -16.78, -18.26, -3.16, 13.18, 
		-20.56, -14.6, 13.33, 16.16, 11.76, 5.69, -15.98, -9.1, -2.02,
		 2.26, 5.33, 8.83, -21.77, 3.02, 10.76, -3.22, 11.74, .67, 
		-6.74, -13.2, -6.88, -.1, 7.79, 8.68, 1.04, -9.06, -3.89, 
		-10.54, 8.44, -2.01, -6.26, 3.28, .17, -.4, .55, 4.55, -.55, 
		4.4, 1.7, -7.92, -.67, -.61, 2.13, -4.16, 2.33, -2.85, -1.8, 
		-1.12, -3.59, -8.72, 3., -1.4, 0., -2.3, 2.11, 2.08, -.6, 
		-.79, -1.05, .58, .76, -.7, -.2, .14, -2.12, 1.7, -1.44, -.22,
		 -2.57, .44, -2.01, 3.49, -2.34, -2.09, -.16, -1.08, .46, .37,
		 1.23, 1.75, -.89, -2.19, .85, .27, .1, .72, .54, -.09, -.37, 
		.29, -.43, .23, .22, -.89, -.94, -.16, -.03, .72, -.02, -.92, 
		-.88, .42, .49, .63, 1.56, -.42, -.5, .96, -1.24, -.19, -.1, 
		.81, .42, -.13, -.04, .38, .48, .08, .48, .46, -.3, -.35, 
		-.43, -.36, -.71, -29404.8, -1450.9, 4652.5, -2499.6, 2982., 
		-2991.6, 1677., -734.6, 1363.2, -2381.2, -82.1, 1236.2, 241.9,
		 525.7, -543.4, 903., 809.5, 281.9, 86.3, -158.4, -309.4, 
		199.7, 48., -349.7, -234.3, 363.2, 47.7, 187.8, 208.3, -140.7,
		 -121.2, -151.2, 32.3, 13.5, 98.9, 66., 65.5, -19.1, 72.9, 
		25.1, -121.5, 52.8, -36.2, -64.5, 13.5, 8.9, -64.7, 68.1, 
		80.6, -76.7, -51.5, -8.2, -16.9, 56.5, 2.2, 15.8, 23.5, 6.4, 
		-2.2, -7.2, -27.2, 9.8, -1.8, 23.7, 9.7, 8.4, -17.6, -15.3, 
		-.5, 12.8, -21.1, -11.7, 15.3, 14.9, 13.7, 3.6, -16.5, -6.9, 
		-.3, 2.8, 5., 8.4, -23.4, 2.9, 11., -1.5, 9.8, -1.1, -5.1, 
		-13.2, -6.3, 1.1, 7.8, 8.8, .4, -9.3, -1.4, -11.9, 9.6, -1.9, 
		-6.2, 3.4, -.1, -.2, 1.7, 3.6, -.9, 4.8, .7, -8.6, -.9, -.1, 
		1.9, -4.3, 1.4, -3.4, -2.4, -.1, -3.8, -8.8, 3., -1.4, 0., 
		-2.5, 2.5, 2.3, -.6, -.9, -.4, .3, .6, -.7, -.2, -.1, -1.7, 
		1.4, -1.6, -.6, -3., .2, -2., 3.1, -2.6, -2., -.1, -1.2, .5, 
		.5, 1.3, 1.4, -1.2, -1.8, .7, .1, .3, .8, .5, -.2, -.3, .6, 
		-.5, .2, .1, -.9, -1.1, 0., -.3, .5, .1, -.9, -.9, .5, .6, .7,
		 1.4, -.3, -.4, .8, -1.3, 0., -.1, .8, .3, 0., -.1, .4, .5, 
		.1, .5, .5, -.4, -.5, -.4, -.4, -.6, 5.7, 7.4, -25.9, -11., 
		-7., -30.2, -2.1, -22.4, 2.2, -5.9, 6., 3.1, -1.1, -12., .5, 
		-1.2, -1.6, -.1, -5.9, 6.5, 5.2, 3.6, -5.1, -5., -.3, .5, 0., 
		-.6, 2.5, .2, -.6, 1.3, 3., .9, .3, -.5, -.3, 0., .4, -1.6, 
		1.3, -1.3, -1.4, .8, 0., 0., .9, 1., -.1, -.2, .6, 0., .6, .7,
		 -.8, .1, -.2, -.5, -1.1, -.8, .1, .8, .3, 0., .1, -.2, -.1, 
		.6, .4, -.2, -.1, .5, .4, -.3, .3, -.4, -.1, .5, .4, 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
		0., 0., 0., 0., 0., 0., 0., 0., 0. };


    /* Format strings */
    static char fmt_960[] = "(/\002 This version of the IGRF is intended for"
	    " use up\002,\002 to 2025.0.\002/\002 values for\002,f9.3,\002 wi"
	    "ll be computed\002,\002 but may be of reduced accuracy\002/)";
    static char fmt_961[] = "(/\002 This subroutine will not work with a dat"
	    "e of\002,f9.3,\002.  Date must be in the range 1900.0.ge.date"
	    "\002,\002.le.2030.0. On return f = 1.0d8., x = y = z = 0.\002)";

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double cos(doublereal), sin(doublereal), sqrt(doublereal);

    /* Local variables */
    static integer i__, j, k, l, m, n;
    static doublereal p[105], q[105], r__, t, a2, b2;
#define g0 ((doublereal *)&equiv_25)
#define g1 ((doublereal *)&equiv_25 + 120)
#define g2 ((doublereal *)&equiv_25 + 240)
#define g3 ((doublereal *)&equiv_25 + 360)
#define g4 ((doublereal *)&equiv_25 + 480)
#define g5 ((doublereal *)&equiv_25 + 600)
#define g6 ((doublereal *)&equiv_25 + 720)
#define g7 ((doublereal *)&equiv_25 + 840)
#define g8 ((doublereal *)&equiv_25 + 960)
#define g9 ((doublereal *)&equiv_25 + 1080)
    static doublereal cd;
#define ga ((doublereal *)&equiv_25 + 1200)
#define gb ((doublereal *)&equiv_25 + 1320)
#define gc ((doublereal *)&equiv_25 + 1440)
#define gd ((doublereal *)&equiv_25 + 1560)
#define ge ((doublereal *)&equiv_25 + 1680)
#define gf ((doublereal *)&equiv_25 + 1800)
#define gg ((doublereal *)&equiv_25 + 1920)
#define gh ((doublereal *)&equiv_25)
#define gi ((doublereal *)&equiv_25 + 2040)
#define gj ((doublereal *)&equiv_25 + 2160)
#define gk ((doublereal *)&equiv_25 + 2280)
#define gl ((doublereal *)&equiv_25 + 2475)
#define gm ((doublereal *)&equiv_25 + 2670)
    static doublereal cl[13];
    static integer nc;
#define gp ((doublereal *)&equiv_25 + 2865)
#define gq ((doublereal *)&equiv_25 + 3060)
#define gr ((doublereal *)&equiv_25 + 3255)
#define gs ((doublereal *)&equiv_25 + 3450)
    static integer ll;
    static doublereal tc, ct, sd, sl[13], fn, gn, fm;
    static integer lm;
    static doublereal rr, st, one, gmm, rho;
    static integer kmx, nmx;
    static doublereal two, three, ratio;

    /* Fortran I/O blocks */
    static cilist io___205 = { 0, 6, 0, fmt_960, 0 };
    static cilist io___240 = { 0, 6, 0, fmt_961, 0 };



/*     This is a synthesis routine for the 13th generation IGRF as agreed */
/*     in December 2019 by IAGA Working Group V-MOD. It is valid 1900.0 to */
/*     2025.0 inclusive. Values for dates from 1945.0 to 2015.0 inclusive are */
/*     definitive, otherwise they are non-definitive. */
/*   INPUT */
/*     isv   = 0 if main-field values are required */
/*     isv   = 1 if secular variation values are required */
/*     date  = year A.D. Must be greater than or equal to 1900.0 and */
/*             less than or equal to 2030.0. Warning message is given */
/*             for dates greater than 2025.0. Must be double precision. */
/*     itype = 1 if geodetic (spheroid) */
/*     itype = 2 if geocentric (sphere) */
/*     alt   = height in km above sea level if itype = 1 */
/*           = distance from centre of Earth in km if itype = 2 (>3485 km) */
/*     colat = colatitude (0-180) */
/*     elong = east-longitude (0-360) */
/*     alt, colat and elong must be double precision. */
/*   OUTPUT */
/*     x     = north component (nT) if isv = 0, nT/year if isv = 1 */
/*     y     = east component (nT) if isv = 0, nT/year if isv = 1 */
/*     z     = vertical component (nT) if isv = 0, nT/year if isv = 1 */
/*     f     = total intensity (nT) if isv = 0, rubbish if isv = 1 */

/*     To get the other geomagnetic elements (D, I, H and secular */
/*     variations dD, dH, dI and dF) use routines ptoc and ptocsv. */

/*     Adapted from 8th generation version to include new maximum degree for */
/*     main-field models for 2000.0 and onwards and use WGS84 spheroid instead */
/*     of International Astronomical Union 1966 spheroid as recommended by IAGA */
/*     in July 2003. Reference radius remains as 6371.2 km - it is NOT the mean */
/*     radius (= 6371.0 km) but 6371.2 km is what is used in determining the */
/*     coefficients. Adaptation by Susan Macmillan, August 2003 (for */
/*     9th generation), December 2004, December 2009 & December 2014; */
/*     by William Brown, December 2019, February 2020. */

/*     Coefficients at 1995.0 incorrectly rounded (rounded up instead of */
/*     to even) included as these are the coefficients published in Excel */
/*     spreadsheet July 2005. */



/*     set initial values */

    *x = 0.f;
    *y = 0.f;
    *z__ = 0.f;
    if (*date < 1900.f || *date > 2030.f) {
	goto L11;
    }
    if (*date > 2025.f) {
	s_wsfe(&io___205);
	do_fio(&c__1, (char *)&(*date), (ftnlen)sizeof(doublereal));
	e_wsfe();
    }
    if (*date >= 2020.f) {
	goto L1;
    }
    t = (*date - 1900.f) * .2f;
    ll = (integer) t;
    one = (doublereal) ll;
    t -= one;

/*     SH models before 1995.0 are only to degree 10 */

    if (*date < 1995.f) {
	nmx = 10;
	nc = nmx * (nmx + 2);
	ll = nc * ll;
	kmx = (nmx + 1) * (nmx + 2) / 2;
    } else {
	nmx = 13;
	nc = nmx * (nmx + 2);
	ll = (integer) ((*date - 1995.f) * .2f);

/*     19 is the number of SH models that extend to degree 10 */

	ll = nc * ll + 2280;
	kmx = (nmx + 1) * (nmx + 2) / 2;
    }
    tc = 1.f - t;
    if (*isv == 1) {
	tc = -.2f;
	t = .2f;
    }
    goto L2;

L1:
    t = *date - 2020.f;
    tc = 1.f;
    if (*isv == 1) {
	t = 1.f;
	tc = 0.f;
    }

/*     pointer for last coefficient in pen-ultimate set of MF coefficients... */

    ll = 3255;
    nmx = 13;
    nc = nmx * (nmx + 2);
    kmx = (nmx + 1) * (nmx + 2) / 2;
L2:
    r__ = *alt;
    one = *colat * .017453292f;
    ct = cos(one);
    st = sin(one);
    one = *elong * .017453292f;
    cl[0] = cos(one);
    sl[0] = sin(one);
    cd = 1.f;
    sd = 0.f;
    l = 1;
    m = 1;
    n = 0;
    if (*itype == 2) {
	goto L3;
    }

/*     conversion from geodetic to geocentric coordinates */
/*     (using the WGS84 spheroid) */

    a2 = 40680631.6f;
    b2 = 40408296.f;
    one = a2 * st * st;
    two = b2 * ct * ct;
    three = one + two;
    rho = sqrt(three);
    r__ = sqrt(*alt * (*alt + rho * 2.f) + (a2 * one + b2 * two) / three);
    cd = (*alt + rho) / r__;
    sd = (a2 - b2) / rho * ct * st / r__;
    one = ct;
    ct = ct * cd - st * sd;
    st = st * cd + one * sd;

L3:
    ratio = 6371.2f / r__;
    rr = ratio * ratio;

/*     computation of Schmidt quasi-normal coefficients p and x(=q) */

    p[0] = 1.f;
    p[2] = st;
    q[0] = 0.f;
    q[2] = ct;
    i__1 = kmx;
    for (k = 2; k <= i__1; ++k) {
	if (n >= m) {
	    goto L4;
	}
	m = 0;
	++n;
	rr *= ratio;
	fn = (doublereal) n;
	gn = (doublereal) (n - 1);
L4:
	fm = (doublereal) m;
	if (m != n) {
	    goto L5;
	}
	if (k == 3) {
	    goto L6;
	}
	one = sqrt(1.f - .5f / fm);
	j = k - n - 1;
	p[k - 1] = one * st * p[j - 1];
	q[k - 1] = one * (st * q[j - 1] + ct * p[j - 1]);
	cl[m - 1] = cl[m - 2] * cl[0] - sl[m - 2] * sl[0];
	sl[m - 1] = sl[m - 2] * cl[0] + cl[m - 2] * sl[0];
	goto L6;
L5:
	gmm = (doublereal) (m * m);
	one = sqrt(fn * fn - gmm);
	two = sqrt(gn * gn - gmm) / one;
	three = (fn + gn) / one;
	i__ = k - n;
	j = i__ - n + 1;
	p[k - 1] = three * ct * p[i__ - 1] - two * p[j - 1];
	q[k - 1] = three * (ct * q[i__ - 1] - st * p[i__ - 1]) - two * q[j - 
		1];

/*     synthesis of x, y and z in geocentric coordinates */

L6:
	lm = ll + l;
	one = (tc * gh[lm - 1] + t * gh[lm + nc - 1]) * rr;
	if (m == 0) {
	    goto L9;
	}
	two = (tc * gh[lm] + t * gh[lm + nc]) * rr;
	three = one * cl[m - 1] + two * sl[m - 1];
	*x += three * q[k - 1];
	*z__ -= (fn + 1.f) * three * p[k - 1];
	if (st == 0.f) {
	    goto L7;
	}
	*y += (one * sl[m - 1] - two * cl[m - 1]) * fm * p[k - 1] / st;
	goto L8;
L7:
	*y += (one * sl[m - 1] - two * cl[m - 1]) * q[k - 1] * ct;
L8:
	l += 2;
	goto L10;
L9:
	*x += one * q[k - 1];
	*z__ -= (fn + 1.f) * one * p[k - 1];
	++l;
L10:
	++m;
    }

/*     conversion to coordinate system specified by itype */

    one = *x;
    *x = *x * cd + *z__ * sd;
    *z__ = *z__ * cd - one * sd;
    *f = sqrt(*x * *x + *y * *y + *z__ * *z__);

    return 0;

/*     error return if date out of bounds */

L11:
    *f = 1e8;
    s_wsfe(&io___240);
    do_fio(&c__1, (char *)&(*date), (ftnlen)sizeof(doublereal));
    e_wsfe();
    return 0;
} /* igrf13syn_ */

#undef gs
#undef gr
#undef gq
#undef gp
#undef gm
#undef gl
#undef gk
#undef gj
#undef gi
#undef gh
#undef gg
#undef gf
#undef ge
#undef gd
#undef gc
#undef gb
#undef ga
#undef g9
#undef g8
#undef g7
#undef g6
#undef g5
#undef g4
#undef g3
#undef g2
#undef g1
#undef g0


/* Main program alias */ int igrf13_ () { MAIN__ (); return 0; }
