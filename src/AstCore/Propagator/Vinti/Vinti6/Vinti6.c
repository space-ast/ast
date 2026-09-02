/******************************************************************************
 *  Copyright (C) 1996-97 Gim J. Der.  All rights reserved. 
 *  ------------------------------------------------------------------------  *
 *                                                                            *
 *  Vinti6()...Vinti Solution with Universal Variables.                       *
 *                                                                            *
 *  Originators:                Albert T. Monuki and Gim J. Der, 1996         *
 *  Fortran to C Conversion:    Herb Reynolds, August 5, 1997                 *
 *                                                                            *
 *  ------------------------------------------------------------------------  *
 *                                                                            *
 *  This function computes a Vinti analytic solution to the equations of      *
 *  motion for drag-free circular, elliptic, parabolic and hyperbolic         *
 *  satellite orbits and ballistic trajectories.                              *
 *                                                                            *
 *  ------------------------------------------------------------------------  *
 *                                                                            *
 *  The Keplerian solution is found by the routine Kepler1().                 *
 *                                                                            *
 *  The implementaion of Vinti theory is based on Getchell's method           *
 *  of mechanization (J. of Spacecraft, Vol. 7, N0. 4, April 1970).           *
 *  Improvements are made to give a fool-proof Vinti solution free of         *
 *  singularities by Der and Monuki.                                          *
 *                                                                            *
 *  The J2 and J3 vaules are those of the wgs84 Earth Gravity Model.          *
 *  Metric derived units (km, km/s) are used only for input / output,         *
 *  while Astronomical units (normalized units) are used internally           *
 *  to reduce numerical round-off errors.                                     *
 *                                                                            *
 *  Limitations: None, for trajectory at least 1,000,000 km from Earth.       *
 *               (mean radius of Moon's orbit is 384,000 km)                  *
 *               The limit is artificial since an unbounded trajectory        *
 *               passes the sphere of influence of the Earth.                 *
 *                                                                            *
 *  Input variables:                                                          *
 *    vt0   = initial time (seconds)                                          *
 *    x0[6] = state vector, position and velocity, (km, km/s)                 *
 *    vt1   = final time (seconds)                                            *
 *                                                                            *
 *  Output variables:                                                         *
 *    x1[6] = state vector, position and velocity, (km, km/s)                 *
 *    oe[6] = Vinti mean elements                                             *
 *                                                                            *
 *****************************************************************************/

#include "Vinti6.h"

A_SUPPRESS_WARNINGS_BEGIN

void Vinti6 (double planet[4], double vt0, double x0[6], double vt1, double x1[6], double oe[6])
{
   // ast修改：使用高经度的 pi 值
   // double pi = 3.141592653589793238;
   double pi = 3.141592653589793238462643383279502884197169399375105;

   double ae;
   double gm;
   double xj2;
   double xj3;

   int i, icf, icg, ick;   /* Loop indices */

   double pin[3], vin[3], pf[3], vf[3];
   double twopi;
   double xhat0;
   double r1, r2;
   double time_factor, ve, t0, tf;

   /* Steps 1 - 3 variables */
   double delta, csq, d0, alph0 ;
   double r02, zpdelta, rhotemp, rho0, rho02, sigma0, rrd, delsig0, csqsig0, rcs, v, v0;    
   double alph3, alph32, alph1, sqrf, sqrg, alph22, alph2;
   double gamma0, csgam0, p0, s0, pcsgam0, csqs0p0;
   double a1p, b1, a1;
   double gam1, pgam1, gamma, p, dela1;
   double deltax, cx, c0x, p0x, e02, a_vinti, b_vinti;
   double smgam;
   double s1p, px, s1;
   double p0s1, q1, p1;
   double dels1;
   double h0[3], h02, r0mag, v0mag, alp0, a0, e0;

   /* Step 4 variables */
   double gams3, s, pxs, q;   
   double xinc, xincx;
   double q2, q4, betad, beta, betasq, g, a, b; 
   double g2, asq, bsq, ab, d5, xm, xk1, d1, ecc2, ecc, rho1;
   double a1sq, b1sq;

   double a2, a3, a4, a5, a6;
   double e3, e4, e5, e6;
   double psq;
   double p3, p4, p5, p6;

   double x21, x22;
   double x33, x32, x31;
   double x44, x43, x42, x41;
   double x55, x54, x53, x52, x51;
   double x66, x65, x64, x63, x62, x61;
   double x77, x76, x75, x74, x73, x72, x71;
   double gg1si, gg1psi;
   double cr11, cr12, cr13, cr14, cr15, cr16, cr17;
   double cr21, cr22, cr23, cr24, cr25, cr26, cr27;
   double cr31, cr32, cr33, cr34, cr35;
   double bmg, bpg, d1ma, d1pa, beta1, beta2, b12, b13, b22, b23, xmm1;
   double xmm2;
   double d2, d3, d4;
   double d1md3, bmag;
   double d10, d20, dd2, dd3, dd4, dd5, dd6;
   double c15, c14, c13, c12, c11, c10, c25, c24, c23, c22, c21, c20;
   double b1q, b1q2, b1q4, b2q, b2q2, b2q4;
   double xk12, xk13;
   double sq, sq2, sq3, ucf1, ucf2, ucf3, denystt, denyst;
   double d1a2, cn11, cn12, cn13, cn14, cn15, cn16, cn17;
   double cn22, cn24, cn26;
   double d41ma, d41pa;
   double cn31, cn32, cn33, cn34, cn35, cn36;
   double u, d5sq, ut_n, ut_d;
   double csu, snu, snu2, snu4;
   double t1, t2, t3, t4, t5, t6;
   double xhat, shat, cacs, acs, chat, zz;  

   /* Step 5 variables */
   double eca;
   double sneca, cneca, s1mes, temp3, tra;
   double snw, cnw;
   double ecccnw;
   double dwdx, dw1dx, dw2dx, dw3dx, dw4dx;
   double v3, v4, v5, v6, v7; 
   double w1, w2, w3, w4, w5, w6;
   double uhat;
   double sstar, cstar, cb1qs, psi1;
   double cb2qs, psi2;
   double r3;
   double en3, en1, en2;
   double somega, somegax, capt, deltat, comega, comegax;

   /* Step 6 variables */
   double cn1r1, psixhat, dpsx1, dpsx, delx;
   double ystar;

   /* Step 7 variables */
   double qsnu, alphaf, rhof, sigmaf;
   double rhof2, sigf2;
   double rhocsq, df, snalp, csalp;
   double rhoqd, drhofp, drhof, temp12, dsigf;
   double temp20, temp21, dd, dalphf;
   double degs;
   double dt;                                /* 8/20/97 */

   dt = vt1 - vt0;                           /* 8/20/97 */
   if(fabs(dt) < 1.0e-15)                    /* 8/20/97 */
   {                                         /* 8/20/97 */
      for(i = 0; i < 6; i++) x1[i] = x0[i];  /* 8/20/97 */
                                             /* 8/20/97 */
      return;                                /* 8/20/97 */
   }                                         /* 8/20/97 */

   twopi = 2*pi;
   degs = 180/pi;

   ae  = planet[0];  // Equatorial radius of planet
   gm  = planet[1];  // Gravitational mass constant
   xj2 = planet[2];  // J2
   xj3 = planet[3];  // J3

   /*
    * Compute 
    *    a. The initial guess of the universal variable xhat at tf.
    *    b. The Kepler solution as a default for the focal circle case. 
    */
   Kepler1(planet, vt0, x0, vt1, x1, &xhat0);

   xhat0 = xhat0/sqrt(ae);    /* Change to Astronomical units */

   /*
    *  Check Vinti's forbidden zone 
    */
   h0[0] = x0[1]*x0[5] - x0[2]*x0[4];
   h0[1] = x0[2]*x0[3] - x0[0]*x0[5];
   h0[2] = x0[0]*x0[4] - x0[1]*x0[3];
   h02   = h0[0]*h0[0] + h0[1]*h0[1] + h0[2]*h0[2];

   r0mag = sqrt(x0[0]*x0[0] + x0[1]*x0[1] + x0[2]*x0[2]);
   v0mag = sqrt(x0[3]*x0[3] + x0[4]*x0[4] + x0[5]*x0[5]);

   alp0  = 2.0/r0mag - v0mag*v0mag/gm;

   if (fabs(alp0) < 1.0e-15)
   {
     r1 = h02/(2.0*gm);	  				  /* Parabolic */
   }
   else
   {
     a0 = 1.0/alp0;                   /* Others */
     e02 = 1 - alp0*h02/gm;

     //if(e02 < 0) e0 = 0;    /* For most machines  this test is unnececessary */
     //else        e0 = sqrt(1.0 - alp0*h02/gm);
     
     // Above est replaced 9/5/97
     if(e02 <= 0) e0 = 0;    /* For most machines  this test is unnececessary */
     else         e0 = sqrt(e02);

     r1 = a0*(1.0 - e0);
   }

	if (r1 < 210) return;

   /*
    *  Change from SI (derived) units to astronomical units
    */ 
   time_factor = sqrt(ae*ae*ae/gm);
   ve = ae /time_factor;
   t0 = vt0/time_factor;
   tf = vt1/time_factor;
 
   for(i = 0; i < 3; i++)
   {
      pin[i] = x0[i]/ae;
      vin[i] = x0[i+3]/ve;
   }

   /*   
    * Step 1. Initial coordinate transformation
    */   
   delta = -xj3/(2*xj2);
   csq   =  xj2*(1 - delta*delta/xj2);
   d0    =  sqrt( pin[0]*pin[0] + pin[1]*pin[1] ) ;
   alph0 =  atan2(pin[1], pin[0]);

   if(alph0 < 0)
   {
      alph0 = twopi + alph0;
   }

   r02     =  d0*d0 + pin[2]*pin[2];
   zpdelta =  pin[2] + delta;
   rhotemp =  r02 - csq + delta*(zpdelta + pin[2]);
   rho0    =  sqrt(rhotemp + sqrt(rhotemp*rhotemp + 4*csq*(zpdelta*zpdelta)))/sqrt(2.0);
   rho02   =  rho0*rho0;
   sigma0  =  zpdelta/rho0;
   rrd     =  pin[0]*vin[0] + pin[1]*vin[1] + pin[2]*vin[2];
   delsig0 =  delta*sigma0;
   csqsig0 =  csq*sigma0;
   rcs     =  rho02 + csqsig0*sigma0;
   v       = -(rho0 + delsig0)/rcs;
   v0      =  sqrt( vin[0]*vin[0] + vin[1]*vin[1] + vin[2]*vin[2] );

   /*
    * Step 2. The first three Jacobi constants, alph1, alph2, alph3
    */ 
   alph3   = pin[0]*vin[1] - pin[1]*vin[0];
   alph32  = alph3*alph3;
   alph1   = 0.5*v0*v0 + v;
   sqrf    = rho0*rrd + (csqsig0 + delta*rho0)*vin[2];
   sqrg    =-sigma0*rrd - (delsig0 - rho0)*vin[2];
   alph22  = 2*rho0 + 2*alph1*rho02 + (csq*alph32 - sqrf*sqrf)/(rho02 + csq);
   alph2   = sqrt(alph22);
   gamma0  = 2*alph1;
   csgam0  = csq*gamma0;
   p0      = alph22;
   s0      = 1 - alph32/alph22;
   pcsgam0 = p0 - csgam0;
   csqs0p0 = csq*s0*p0;

   /*
    *  Step 3. Factorizing the F and G quartics
    * 
    *  Vinti(1961) Equations (3.19) to (3.22)
    *  Getchell(1970) Equations (6) to (9)
    *  Factorization by numerical method is more accurate than
    *  analytic approximation.
    *
    *  alph1 < 0, gamma0 = 2*alph1 < 0  for ellipse and circle
    *  alph1 = 0, gamma0 = 0            for parabola
    *  alph1 > 0, gamma0 = 2*alph1 > 0  for hyperbola
    *  Note that even if the input state is a parabola,
    *  alph1 may not be zero.
    *
    *  Factorizing the F(rho) quartic
    */
   a1p = 0;
   b1 = csqs0p0/pcsgam0;
   a1 =(csq - b1)/pcsgam0;
   
   for(icf = 1; icf <= 5; icf++)
   {
      gam1  = 1 + gamma0*a1;
      pgam1 = pcsgam0 + b1*gamma0 - 4*a1*gam1;
      gamma = gamma0/gam1;
      p     = pgam1/gam1;
      b1    = csqs0p0/pgam1;
      a1    =(csq - gam1*b1)/pgam1;
      dela1 = a1 - a1p;
      
      if(fabs(dela1) < 1.0e-15)  break;
      a1p = a1;
   }

   oe[0] = p;     // Vinti mean element (conic parameter)

   /*   
    * Check on some critical parameters in SI derived units
    */   
	deltax  = delta*ae;

	cx  = sqrt(csq)*ae;
	c0x = sqrt(xj2)*ae;
	p0x = p0*ae;
   e02 = 1 + 2*alph1*alph22;

	a_vinti = -2*a1;
	b_vinti = b1;

   /*	r1pr2_vinti = -2.d0/gamma	            // gamma=0 for parabola 
    *	r1xr2_vinti = -p/gamma
    *	rho1 = p/2.d0		 ! parabola
    *	ax = -ae/gamma
    * e0 = dsqrt(1.d0 + 2.d0*alph1*alph22)   // problem sometimes 
    */

   if (gamma < 0) smgam = sqrt(-gamma);
	else           smgam = sqrt(gamma);

   /*
    * Factorizing the G(sigma) quartic
    */
   s1p = 0;
   px = 0;
   s1 = 1;

   for(icg = 1; icg <= 5; icg++)
   {
      p0s1 = p0*s1;
      q1 = -csgam0/p0s1;
      p1 = 2*delta/p0s1 - 2*q1*px;
      px = delta/p0s1 - s0*p1/(2*s1);
      s1 = (pcsgam0 - s0*p0*q1)/((1 - 2*px*p1)*p0);
      dels1 = s1 - s1p;
      
      if (fabs(dels1) < 1.0e-15)  break;
      s1p = s1;
   }

  /*
   * Step 4. Initialize the coefficients of the R and N integrals.
   * First, determine the constants after the factorization, inclination, 
   * eccentricity,rho1, etc.
   */
   gams3 = gamma*smgam;
   s = s0/s1;
	pxs = px*px + s;     oe[2] = pxs;   // Vinti mean element ( sin^2 (I) )

	if (pxs < 0) q = 0;
	else         q = sqrt(pxs);
 
   xinc = asin(q);

   if( alph3*cos(xinc) < 0 )  xinc = pi - xinc;

   xincx = xinc*degs;

   q2 = q*q;
   q4 = q2*q2;
   betad = 1 + p1*px - q1*px*px - q1*q2;
   beta  =(p1 - 2*q1*px)/betad;
   betasq = beta*beta;
   g = -beta/(1 + sqrt(1 - betasq*q2));
   a =  px + g*q2;
   b =  1 + g*px;

   g2  = g*g;
   asq = a*a;
   bsq = b*b;
   ab  = a*b;
   d5  = 1 + p1*a - q1*asq;
   xm  = (q1*bsq - p1*b*g - g2)/d5;
   xk1 = xm*q2;
   d1  = sqrt((1 - g2*q2)/(s1*d5));
   ecc2 = 1 + p*gamma;
   ecc  = sqrt(ecc2);      oe[1] = ecc;   // Vinti mean element (eccentricity)
   rho1 = p/(1 + ecc);

  /*
   * Coefficients for the R - integrals
   * A0 = 1, A1 from factorization, A2 to A6
   */
   a1sq = a1*a1;
   b1sq = b1*b1;

   a2 =(3*a1sq - b1)/2;
   a3 =(2.5*a1sq*a1 - 1.50*a1*b1);
   a4 = 0.375*(b1sq - 10*a1sq*b1);  
   a5 = 1.875*a1*b1sq;
   a6 = -0.3125*b1sq*b1;

  /*
   *  W1, W2 ... W6 for the R - integrals
   */
   e3 = ecc2*ecc;
   e4 = ecc2*ecc2;
   e5 = e3*ecc2;
   e6 = e3*e3;

   psq = p*p;

   p3 = psq*p;
   p4 = psq*psq;
   p5 = p3*psq;
   p6 = p4*psq;

   x21 = 1/p;
   x22 = ecc/p;

   x33 = 0.5*ecc2/psq;
   x32 = 2*ecc/psq;
   x31 = 1/psq + x33;

   x44 = e3/(3*p3);
   x43 = 1.5*ecc2/p3;
   x42 = 3*ecc/p3 + 2*x44;
   x41 = 1/p3 + x43;

   x55 = x33*x33;
   x54 = 4*x44*x21;
   x53 = 3*ecc2/p4 + 1.5*x55;
   x52 = 4*ecc/p4 + 2*x54;
   x51 = 1/p4 + x53;

   x66 = 0.2*e5/p5;
   x65 = 1.25*e4/p5;
   x64 = 10*e3/(3*p5) + 4*x66/3;
   x63 = 5*ecc2/p5 + 1.5*x65;
   x62 = 5*ecc/p5 + 2*x64;
   x61 = 1/p5 + x63;

   x77 = e6/(6*p6);
   x76 = 1.2*e5/p6;
   x75 = 3.75*e4/p6 + 1.25*x77;
   x74 = 20*e3/(3*p6) + x76/0.75;
   x73 = 7.5*ecc2/p6 + 1.5*x75;
   x72 = 6*ecc/p6 + 2*x74;
   x71 = 1/p6 + x73;	         
   /* Coefficients for the W's completed */


   gg1si = 1.0/sqrt(gam1);	   /* gam1 > 0 always */
   gg1psi = gg1si/sqrt(p);	   /* p > 0 always */

   /* R1 coefficients */
   cr11 = (rho1 + a1)*gg1si;
   cr12 = ecc*gg1si;
   cr13 = a2*gg1psi;
   cr14 = a3*gg1psi;
   cr15 = a4*gg1psi;
   cr16 = a5*gg1psi;
   cr17 = a6*gg1psi;

   /* R2 coefficients */
   cr21 = sqrt(p0/pgam1);
   cr22 = a1*cr21;
   cr23 = a2*cr21;
   cr24 = a3*cr21;
   cr25 = a4*cr21;
   cr26 = a5*cr21;
   cr27 = a6*cr21;

   /* R3 coefficients */
   cr31 = alph3*gg1psi;
   cr32 = a1*cr31;
   cr33 = (a2 - csq)*cr31;
   cr34 = (a3 - a1*csq)*cr31;
   cr35 = a4*cr31 - csq*cr33;
   /* R coefficients completed */


   /* Coefficients for the N - integrals */

   /*
    *  N3 coefficients
    */
   bmg   = b - g;
   bpg   = b + g;
   d1ma  = 1 - a;
   d1pa  = 1 + a;
   beta1 = bmg/d1ma;
   beta2 =-bpg/d1pa;
   b12   = beta1*beta1;
   b13   = b12*beta1;
   b22   = beta2*beta2;
   b23   = b22*beta2;
   xmm1  = sqrt(1 - b12*q2);
   
   if(xmm1*alph3 < 0) xmm1 = -xmm1;

   xmm2 = sqrt(1 - b22*q2);

   if(xmm2*alph3 < 0) xmm2 = -xmm2;

   d2 = delta/(p0*(s1 - s0*q1));
   d3 = q1 + 2*p1*d2;
   d4 = d1*alph3/(2.*alph2);

   d1md3 = 1 - d3;
   bmag  = b - a*g;

   d10 = bmag/bmg*sqrt(d1md3/(d5*(1 - xm/b12)*(1 - 2*d2)));
   d20 = bmag/bpg*sqrt(d1md3/(d5*(1 - xm/b22)*(1 + 2*d2)));

   dd2 = xm/2;
   dd3 = dd2*g;
   dd4 = 1.5*dd2*dd2;
   dd5 = dd4*g;
   dd6 = dd2*dd4/0.6;

   c15 = dd6/(b13*b13);
   c14 = c15 + dd5/(b12*b13);
   c13 = c14 + dd4/(b12*b12);
   c12 = c13 + dd3/b13;
   c11 = c12 + dd2/b12;
   c10 = c11 + g/beta1;

   c25 = dd6/(b23*b23);
   c24 = c25 + dd5/(b22*b23);
   c23 = c24 + dd4/(b22*b22);
   c22 = c23 + dd3/b23;
   c21 = c22 + dd2/b22;
   c20 = c21 + g/beta2;

   b1q  = beta1*q;
   b1q2 = b1q*b1q;
   b1q4 = b1q2*b1q2;
   b2q  = beta2*q;
   b2q2 = b2q*b2q;
   b2q4 = b2q2*b2q2;     /* N3 coefficients completed */

   /*
    *  N1 and N2 coefficients
    */
   xk12 = xk1*xk1;
   xk13 = xk12*xk1;

   /* Byrd and Friedman formula for elliptic integral */
   sq  = xk1/16 + xk12/32 + 21*xk13/1024;
   sq2 = sq*sq;
   sq3 = sq2*sq;

   ucf1 = 2*sq/(1 + sq2);
   ucf2 = sq2/(1 + sq2*sq2);
   ucf3 = 2.0*sq3/(3.0*(1 + sq3*sq3));

   denystt = 1 + sq + sq;
   denyst  = denystt*denystt*d1;

   /* N1 coefficients */
   d1a2 = d1/alph2;
   cn11 = d1a2*asq;
   cn12 = d1a2*2*ab*q;
   cn13 = d1a2*(bsq - 4*ab*g)*q2;
   cn14 = d1a2*(xm*ab - 2*bsq*g)*q2*q;
   cn15 = d1a2*(3*bsq*g2 + xm*bsq/2)*q4;
   cn16 =-d1a2*xm*bsq*g*q4*q;
   cn17 = 0.375*d1a2*xm*xm*bsq*q4*q2;

   /* N2 coefficients */
   cn22 = 0.5*xk1*d1;
   cn24 = 0.375*xk12*d1;
   cn26 = 0.3125*xk13*d1;

   /* N3 coefficients */
   d41ma = d4/d1ma;
   d41pa = d4/d1pa;

   cn31 = -d41ma*c10 - d41pa*c20;
   cn32 = -d41ma*c11*b1q - d41pa*c21*b2q;
   cn33 = -d41ma*c12*b1q2 - d41pa*c22*b2q2;
   cn34 = -d41ma*c13*b1q2*b1q - d41pa*c23*b2q2*b2q;
   cn35 = -d41ma*c14*b1q4 - d41pa*c24*b2q4;
   cn36 = -d41ma*c15*b1q4*b1q - d41pa*c25*b2q4*b2q;

   /*
    * This is to avoid the singularity at zero inclination
    * u must be determined exactly
    */
   if (q == 0.0)
   {
	   u = 0;
   }
	else
   {
      d5sq = sqrt( s1*p0*(1 + p1*sigma0 - q1*sigma0*sigma0) );
      ut_n = (sigma0 - a)*d5sq;
      ut_d = sqrg*sqrt(1 - g2*q2);

      u = atan2(ut_n, ut_d);
   }

   /*
    * Calculate the Tk = tk, k = 0, T0 = t0 = u.  Here k = 1, ... ,6
    */
   csu  = cos(u);
   snu  = sin(u);
   snu2 = snu*snu;
   snu4 = snu2*snu2;

   t1 = 1 - csu;
   t2 = (u - csu*snu)/2;
   t3 = (2*t1 - csu*snu2)/3;
   t4 = (3*t2 - csu*snu2*snu)/4;
   t5 = (4*t3 - csu*snu4)/5;
   t6 = (5*t4 - csu*snu4*snu)/6;

   /*
    * Compute xhat at the initial time which is used only once to get
    * the Jacobi's constants (beta1= - capt, beta2=somega, beta3=comega)
    * Note that xhat should not be computed from the initial osculating
    * position and velocity vectors which are Kepler's solutions.
    */
   if (gamma < -1.0e-14)      /* Ellipse */
   {
      //if (fabs(ecc) < 1.0e-10 || fabs(sqrf) < 1.0e-10)
      // Replaced 9/5/97
      if (fabs(ecc) < 1.0e-10 || fabs(sqrf) < 1.0e-10 || fabs(rho0 -rho1) < 1e-5 )
      {
         xhat = 0;
      }
      else
      {
         //shat = sign(1, sqrf)*sqrt(gamma*rho02 + 2*rho0 - p)/ecc;
         shat = ( fabs(sqrf)/sqrf )*sqrt(gamma*rho02 + 2*rho0 - p)/ecc;

         cacs =(rho0 - rho1)*gamma/ecc + 1;
         acs  = atan2(shat*smgam, cacs);
      
         if(acs <  0) acs = twopi + acs;

         xhat = acs/smgam;
      }   
   }
	else if (gamma > 1.0e-14)  /* Hyperbola */
   {
      if (fabs(ecc) < 1.0e-10 || fabs(sqrf) < 1.0e-10)
      {   
          xhat = 0.0;
      }
      else
      {
        chat = (rho0 - rho1)/ecc;		   /* chat > 0, always */
	     zz = gamma*chat + 1;
        xhat = (  log( zz + sqrt(zz*zz - 1) )  )/sqrt(gamma);

        if (sqrf < 0)   xhat = -xhat;
      }
   }
	else								/* parabola */
   {
	    xhat = rrd;		      /* r*rdot */
   }

   /* .............. End initialization ...................*/


   /*
    * Step 5. The last three Jacobi constants, -capt, somega, comega
    *
    * Determine the true anomaly, tra, from xhat of ti
    */
   eca = smgam*xhat;

   if (gamma < -1.0e-14)      /* Ellipse */
   {
      sneca = sin(eca);
      cneca = cos(eca);
      s1mes = sqrt(1 - ecc2);
      temp3 = 2*atan( (ecc*sneca)/(1 + s1mes - ecc*cneca) );
      tra   = eca + temp3;
   }
   else if (gamma > 1.0e-14)  /* Hyperbola */
   {
      sneca = sinh(eca);
      cneca = cosh(eca);

      chat = (cneca - 1)/gamma;
      shat = sneca/smgam;
      tra = atan2( sqrt(p)*shat,( rho1 - chat) );
   }
   else								/* Parabola */
   {
      tra = 2*atan( xhat/sqrt(p) );
   }

   snw = sin(tra);
   cnw = cos(tra);
   ecccnw = ecc*cnw;

   /*
    *  Is there such a thing call parabolic trajectory in the OSC system? 
    */
   dwdx  = (1 + ecccnw)/sqrt(p); 
   dw1dx = (1 + ecccnw)/p;

   dw2dx = dw1dx*dw1dx;
   dw3dx = dw1dx*dw2dx;
   dw4dx = dw2dx*dw2dx;

   v3 = snw*cnw;
   v4 = v3*cnw;
   v5 = v4*cnw;
   v6 = v5*cnw;
   v7 = v6*cnw;

   w1 = x21*tra + x22*snw;
   w2 = x31*tra + x32*snw + x33*v3;
   w3 = x41*tra + x42*snw + x43*v3 + x44*v4;
   w4 = x51*tra + x52*snw + x53*v3 + x54*v4 + x55*v5;
   w5 = x61*tra + x62*snw + x63*v3 + x64*v4 + x65*v5 + x66*v6;
   w6 = x71*tra + x72*snw + x73*v3 + x74*v4 + x75*v5 + x76*v6 + x77*v7;

   if (fabs(gamma) >= 1.0e-14)	/* Circle, ellipse, hyperbloa */
   {  
      chat = (cneca - 1)/gamma;
      uhat = (sneca - eca)/gams3;
   }
	else								   /*  Parabola */
   {
      chat = xhat*xhat/2;
      uhat = xhat*xhat*xhat/6;
   }

   r1 = cr11*xhat + cr12*uhat + cr13*tra + cr14*w1 + cr15*w2 + cr16*w3 + cr17*w4;
   r2 = cr21*tra  + cr22*w1   + cr23*w2  + cr24*w3 + cr25*w4 + cr26*w5 + cr27*w6;

   sstar = sin(0.5*u);
   cstar = cos(0.5*u);
   cb1qs = cstar - b1q*sstar;
   psi1  = atan(xmm1*sstar/cb1qs);

   if(cb1qs*cos(psi1) < 0.0) psi1 = pi + psi1;

   cb2qs = cstar - b2q*sstar;
   psi2  = atan(xmm2*sstar/cb2qs);

   if(cb2qs*cos(psi2) < 0.0) psi2 = pi + psi2;

   r3 = cr31*w2 + cr32*w3 + cr33*w4 + cr34*w5 + cr35*w6;

   en3 = d10*psi1 + d20*psi2 + cn31*u + cn32*t1 + cn33*t2 + cn34*t3 + cn35*t4 + cn36*t5;
   en1 = cn11*u + cn12*t1 + cn13*t2 + cn14*t3 + cn15*t4 + cn16*t5 + cn17*t6;
   en2 = d1*u + cn22*t2 + cn24*t4 + cn26*t6;
   
   somega  = en2 - r2;		            /* beta2 */
   somegax = (somega)*degs;
   capt    = t0 - r1 - csq*en1;		   /* -beta1 */
   deltat  = tf - capt;			  
   comega  = alph0 + csq*r3 - en3;	   /* beta3 */
   comegax = comega*degs;

   oe[3] = -capt*time_factor;       /* Vinti mean element  "beta1" */
   oe[4] = somega;                  /* Vinti mean element  "beta2" */
   oe[5] = comega;                  /* Vinti mean element  "beta3" */

   /*
    * Step 6. At tf, solve the kinematical equations for the oblate
    * spheroidal coordinates (rho, sigma, phi)
    *
    * The generalized Kepler equation is solved by iteration
    * initial guess of xhat is that of the Kepler's solution at tf 
    */
	//xhat = xhat0;
	xhat += xhat0;  // replaced 9/5/97

   for(ick = 1; ick <= 10; ick++)
   {      
      eca = smgam*xhat;
      
      if (gamma < -1.0e-14)         /* Ellipse */
      {
         sneca = sin(eca);
         cneca = cos(eca);
         s1mes = sqrt(1 - ecc2);
         temp3 = 2*atan( (ecc*sneca)/(1 + s1mes - ecc*cneca) );
         tra   = eca + temp3;
      }
		else if (gamma > 1.0e-14)     /* Hyperbola */
      {
         sneca = sinh(eca);
         cneca = cosh(eca);
         chat  = (cneca - 1)/gamma;
         shat  = sneca/smgam;
         tra   = atan2( sqrt(p)*shat, (rho1 - chat) );
      }
		else									/* Parabola */
      {
         tra = 2*atan(xhat/sqrt(p));
      }
      
      snw = sin(tra);
      cnw = cos(tra);
      ecccnw = ecc*cnw;
      
      dwdx  = (1 + ecccnw)/sqrt(p);
      dw1dx = (1 + ecccnw)/p;
      dw2dx = dw1dx*dw1dx;
      dw3dx = dw1dx*dw2dx;
      dw4dx = dw2dx*dw2dx;
      
      v3 = snw*cnw;
      v4 = v3*cnw;
      v5 = v4*cnw;
      v6 = v5*cnw;
      v7 = v6*cnw;
      
      w1 = x21*tra + x22*snw;
      w2 = x31*tra + x32*snw + x33*v3;
      w3 = x41*tra + x42*snw + x43*v3 + x44*v4;
      w4 = x51*tra + x52*snw + x53*v3 + x54*v4 + x55*v5;
      w5 = x61*tra + x62*snw + x63*v3 + x64*v4 + x65*v5 + x66*v6;
      w6 = x71*tra + x72*snw + x73*v3 + x74*v4 + x75*v5 + x76*v6 + x77*v7;
      
      if(fabs(gamma) >= 1.0e-14)
      {
         
         chat =(cneca - 1)/gamma;
         uhat =(sneca - eca)/gams3;
      }
		else		/* Parabola */
      {
         chat = xhat*xhat/2;
         uhat = xhat*xhat*xhat/6;
      }
      
      r1 = cr11*xhat + cr12*uhat + cr13*tra + cr14*w1 + cr15*w2 + cr16*w3 + cr17*w4;
      r2 = cr21*tra  + cr22*w1   + cr23*w2  + cr24*w3 + cr25*w4 + cr26*w5 + cr27*w6;
      
      //if (iflag == 1) goto LABLE_60; 	/* solution converged */
      
      ystar = (r2 + somega)/denyst;
      u = ystar + ucf1*sin(2*ystar) + ucf2*sin(4*ystar) + ucf3*sin(6*ystar);
        
      csu  = cos(u);
      snu  = sin(u);
      snu2 = snu*snu;
      snu4 = snu2*snu2;
      
      t1 = 1 - csu;
      t2 = (u - csu*snu)/2;
      t3 = (2*t1 - csu*snu2)/3;
      t4 = (3*t2 - csu*snu2*snu)/4;
      t5 = (4*t3 - csu*snu4)/5;
      t6 = (5*t4 - csu*snu4*snu)/6;
      
      en1 = cn11*u + cn12*t1 + cn13*t2 + cn14*t3 + cn15*t4 + cn16*t5 + cn17*t6;
      
      cn1r1  = csq*en1 + r1;
      psixhat = cn1r1 - deltat;		   	    /* Function */
      
      dpsx1 = cr13 + cr14*dw1dx + cr15*dw2dx + cr16*dw3dx + cr17*dw4dx;
      dpsx = cr11 + cr12*chat + dpsx1*dwdx; 	 /* 1st derivative */
      delx = psixhat/dpsx;
      
      xhat = xhat - delx;
      
      if(fabs(delx) < 1.0e-14 )  break;
   }
      
   /*
    * Step 7. Final coordinate transformation, after solution converged
    * OSC state vector is (rhof, sigmaf, alphaf, drhof, dsigf, dalphf)
    * ECI state vector is (pf, vf)
    */
   sstar = sin(0.5*u);
   cstar = cos(0.5*u);
   cb1qs = cstar - b1q*sstar;
   psi1  = atan(xmm1*sstar/cb1qs);

   if(cb1qs*cos(psi1) < 0) psi1 = pi + psi1;

   cb2qs = cstar - b2q*sstar;
   psi2  = atan(xmm2*sstar/cb2qs);

   if(cb2qs*cos(psi2) < 0) psi2 = pi + psi2;

   r3 = cr31*w2 + cr32*w3 + cr33*w4 + cr34*w5 + cr35*w6;
   en3 = d10*psi1 + d20*psi2 + cn31*u + cn32*t1 + cn33*t2 + cn34*t3 + cn35*t4 + cn36*t5;
   qsnu = q*snu;
   alphaf = comega - csq*r3 + en3;
   rhof   = rho1 + ecc*chat;
   sigmaf = (a + b*qsnu)/(1 + g*qsnu);

   pf[2] = rhof*sigmaf - delta;

   rhof2 = rhof*rhof;
   sigf2 = sigmaf*sigmaf;

   rhocsq = rhof2 + csq;
   df = sqrt(rhocsq*(1 - sigf2));
   snalp = sin(alphaf);
   csalp = cos(alphaf);

   pf[0] = df*csalp;
   pf[1] = df*snalp;

   shat = xhat + gamma*uhat;
   rcs  = rhof2 + csq*sigf2;

   rhoqd  = rhof2 - 2*a1*rhof + b1;
   drhofp = sqrt(gam1*rhoqd)/rcs;
   drhof  = ecc*shat*drhofp;
   temp12 =(1 + p1*sigmaf - q1*sigf2)*(1 - g2*q2)*s1;
   dsigf  = alph2*q*sqrt(temp12)/rcs*csu/(1 + g*qsnu);
   temp20 =(1 - sigf2)*(rhof*drhof);
   temp21 = rhocsq*sigmaf*dsigf;
   dd =(temp20 - temp21)/df;
   dalphf = alph3/(df*df);

   vf[0] = dd*csalp - pf[1]*dalphf;
   vf[1] = dd*snalp + pf[0]*dalphf;
   vf[2] = sigmaf*drhof + rhof*dsigf;

   /*
    * Change from astronomical units to SI (derived) units
    */
   for(i = 0; i < 3; i++)
   {
      x1[i] = pf[i]*ae;      /* Change back to km */
      x1[i+3] = vf[i]*ve;    /* Change back to km/s */
   }
}


/******************************************************************************
 *  Orbital and Celestial Mechanics		                                       *
 *  ------------------------------------------------------------------------  *
 *                                                                            *
 *  Kepler1()...Kepler Solution with Universal Variables.                     *
 *                                                                            *
 *  Originator:                 Gim J. Der, 1995                              *
 *  Fortran to C Conversion:    Herbert B. Reynolds, 1997                     *
 *  ------------------------------------------------------------------------  *
 *                                                                            *
 *  This function computes the kepler solution (state vector, x1, at          *
 *  any time, t1) from a given initial state vector, x0, at an arbitrary      *
 *  time t0, using the universal variable formulation.                        *
 *                                                                            *
 *  It accommodates all orbits (circular, elliptic, parabolic,                *
 *  hyperbolic) with no discontinuity or singularity.  No problem of          *
 *  convergence was encountered, and it always converges in less than:        *
 *                                                                            *
 *      a. five iterations for circle and ellipse                             *
 *      b. seven iterations for parabola and hyperbola                        *
 *                                                                            *
 *  The "if(dx2 > 0)" loop is not for efficiency but to guarantee a good      *
 *  solution for parabolic and hyperbolic orbits up to 1,000,000 km from      *
 *  Earth.                                                                    *
 *                                                                            *
 *  Input variables:                                                          *
 *          x0[6] = initial state vector at t0, (ECI),     (km,km/s)          *
 *          t0    = time with respect to x0, initial time, (sec)              *
 *          t1    = time with respect to x1, final time,   (sec)              *
 *                                                                            *
 *          Note: unit (km, km/s) depends only on that of the data gmx, rex   *
 *                                                                            *
 *  Output variables:                                                         *
 *          x1[6] = state vector at t1:(t0+dt),   (km,km/s)                   *
 *          xxx   = universal variable, xhat in Getchell at t1  (Unitless)    *
 *                                                                            *
 *****************************************************************************/

#include <math.h>

void Kepler1 (double planet[4], double t0, double x0[6], double t1, double x1[6], double *xxx)
{
   double rex, gmx;
   double muqr = 1;

//   double gmx = 398600.5;
//   double rex = 6378.137;

   double tlimit = 1.0e-10;
   int kn = 10;

   int i, k;
   double x;
   double r0[3], v0[3];
   double dfx, u1, u2, u3;
   double dt;
   double time_factor, vex;
   double r0mag, v0mag, d0;
   double sigma0, alp0, a0;
   double y, yqr, cy, sy;
   double fx, dfx2, sdfx;
   double dx0, dx1, dx2;
   double dx;
   double rmag, f, g, df, dg;

   rex = planet[0];
   gmx = planet[1];

   dt = t1 - t0;

   if(fabs(dt) < tlimit)
   {
      for(i = 0; i < 6; i++)
      {
          x1[i] = x0[i];
      }

      *xxx = 0;   /* Better initialize it to something */

      return;
   }

   /*  
    *  Change from SI (derived) units to astronomical units
    */  
   time_factor = sqrt(rex*rex*rex/gmx);

   vex = rex/time_factor;

   dt = dt/time_factor;

   for(i = 0; i < 3; i++)
   {
      r0[i] = x0[i]/rex;
      v0[i] = x0[i+3]/vex;
   }

   r0mag = sqrt( r0[0]*r0[0] + r0[1]*r0[1] + r0[2]*r0[2] );
   v0mag = sqrt( v0[0]*v0[0] + v0[1]*v0[1] + v0[2]*v0[2] );

   d0 = r0[0]*v0[0] +r0[1]*v0[1] +r0[2]*v0[2];

   sigma0 = d0 / muqr;
   alp0 = 2.0 / r0mag - v0mag*v0mag;

   if(alp0 == 0)
   {
      a0 = 1.0e30;
   }
   else
   {
      a0 = 1.0/alp0;
   }

   /*
    * x = Flight angle (theta) computed by kepler's solution
    *     first guessed of:
    *
    *     x = alp0*muqr*(t - t0)          For circle and ellipse
    *     x = muqr*(t - t0)/(10*r0mag)    For parabola and hyperbola
    */
   
   x = alp0*dt;

   if (alp0 <= 0)
   {
      //x = 0.1*dt/r0mag;
      x = 0.5*dt/r0mag; // replace 9/5/97
   }

   for(k = 1; k <= kn; k++)
   {
      if(alp0 < 0)
      {
         y = alp0*x*x;
         yqr = sqrt(-y);
         cy = (1 - cosh(yqr))/y;
         sy = (sinh(yqr) - yqr)/(yqr*yqr*yqr);
      }
      else if(alp0 == 0)
      {
         y = 0;
         cy = 0.5;
         sy = 1.0/6.0;
      }
      else if(alp0 > 0)
      {
         y = alp0*x*x;
         yqr = sqrt(y);
         cy = ( 1 - cos(yqr) )/y;
         sy = (yqr - sin(yqr))/(yqr*yqr*yqr);
      }

      u1 = x*(1 - y*sy);
      u2 = x*x*cy;
      u3 = x*x*x*sy;

      fx = r0mag*u1 + sigma0*u2 + u3 - dt*muqr;
      dfx = sigma0*u1 + (1 - alp0*r0mag)*u2 + r0mag;
      dfx2 = sigma0*(1 - y*cy) + (1 - alp0*r0mag)*u1;
      sdfx = dfx /(fabs(dfx));
      
      dx0 = 16*dfx*dfx;
      dx1 = 20*fx*dfx2;
      dx2 = 16*dfx*dfx - 20*fx*dfx2;

      if(dx2 > 0)
      {
         dx = 5*fx/( dfx + sdfx*sqrt(dx2) );
      }
      else
      {
         dx = 0.5*x;
      }
      
      if(fabs(dx) < 1.0e-10) break;

      x = x - dx;

   }   

   /*
    *  Kepler solution  converged
    *  Coefficients and constants associated with the partials
    */
   rmag = dfx;
   f = 1 - u2/r0mag;
   g = dt - u3/muqr;

   /*
    *  Time derivatives associated with the partials
    */
   df = -muqr*u1/(rmag*r0mag);
   dg = 1 - u2/rmag;
   
   for(i = 0; i < 3; i++)
   {
      x1[i] = ( f*r0[i] + g*v0[i] )*rex;
      x1[i+3] = ( df*r0[i] + dg*v0[i] )*vex;
   }

   x = x*sqrt(rex);		   /* Universal variable for state transition */

   *xxx = x;               /* So we don't have to change all the x's */

   dt = dt*time_factor;
}

/******************************************************************************
 * Converts Vinti mean elements to classical elements
 * (Elliptcal case only for now)
 * H.B. Reynolds, June 1997
 ******************************************************************************/
void VintToKep(double planet[4], double vmean[6], double kmean[6])
{
   double p, e, si2, b1, b2, b3;
   double a, nsq, n, M, i;
   double GM, RE;
   double DEGS = 57.2957795130823200;    // Radians to degrees

   RE = planet[0];
   GM = planet[1];

   p   = vmean[0];
   e   = vmean[1];
   si2 = vmean[2];

   b1 = vmean[3];         // b1 = -T
   b2 = vmean[4];         // b2 = small omega
   b3 = vmean[5];         // b3 = large omega

   a = RE*p/(1 - e*e);    // Ellipse

   nsq = GM/(a*a*a);
   n = sqrt(nsq);
   M = n*b1;

   i = asin( sqrt(si2) );

   kmean[0] = a;
   kmean[1] = e;
   kmean[2] = i*DEGS;      kmean[2] = hmod360(kmean[2]);
   kmean[3] = b3*DEGS;     kmean[3] = hmod360(kmean[3]);
   kmean[4] = b2*DEGS;     kmean[4] = hmod360(kmean[4]);
   kmean[5] = M*DEGS;      kmean[5] = hmod360(kmean[5]);
}

/******************************************************************************
 * Positive mod 2pi (degrees)
 * H.B. Reynolds, June 1997
 ******************************************************************************/
double hmod360(double angle)      
{                              
   double twopi;               
                               
   twopi = 360;               
   angle = fmod(angle, twopi); 
   if(angle < 0) angle += 360;
                               
   return angle;               
}                              

void StateOut(FILE *hOutput, double t, double x[6])
{
   int j;

   fprintf(hOutput, "%8.0lf", t);

   for(j = 0; j < 6; j++)
   {
      fprintf(hOutput, "%17.10lf", x[j]);
   }

   fprintf(hOutput, "\n");
}

A_SUPPRESS_WARNINGS_END
