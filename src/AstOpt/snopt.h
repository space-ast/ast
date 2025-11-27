/* Josh Griffin ... modeled after npsol.h written by           */
/* Mike Gertz - 2-Aug-98                                       */
/* Function prototypes for functions in the snopt distribution */

#ifndef SNOPT
#define SNOPT

//#ifndef F2C_INCLUDE
#include "f2c.h"
//#endif

#ifdef CSNOPT_BUILD
#include "np02lib.h"
#include "npopt.h"
#include "sn02lib.h"
#include "sn03prnt.h"
#include "sn05wrpa.h"
#include "sn05wrpb.h"
#include "sn05wrpc.h"
#include "sn05wrpn.h"
#include "sn10mach.h"
#include "sn12ampl.h"
#include "sn15blas.h"
#include "sn17util.h"
#include "sn20amat.h"
#include "sn25bfac.h"
#include "sn27lu.h"
#include "sn30spec.h"
#include "sn35mps.h"
#include "sn37wrap.h"
#include "sn40bfil.h"
#include "sn50lp.h"
#include "sn55qp.h"
#include "sn56qncg.h"
#include "sn57qopt.h"
#include "sn60srch.h"
#include "sn65rmod.h"
#include "sn70nobj.h"
#include "sn80ncon.h"
#include "sn85hess.h"
#include "sn87sopt.h"
#include "sn90lmqn.h"
#include "sn95fmqn.h"
#include "sq02lib.h"
#include "snopta.h"
#include "snoptb.h"
#include "snoptc.h"
#include "snoptq.h"
#endif

#ifdef __cplusplus
extern "C" {
	typedef int (*U_fp)(...);
#else
typedef int (*U_fp)();

#endif

typedef int (*My_fp)(integer* Status, integer* n,
	doublereal x[], integer* needF, integer* neF, doublereal F[],
	integer* needG, integer* neG, doublereal G[],
	char* cu, integer* lencu,
	integer    iu[], integer* leniu,
	doublereal ru[], integer* lenru);

int snopt_(char* start, integer* m, integer* n, integer* ne,
	integer* nname, integer* nncon, integer* nnobj, integer* nnjac,
	integer* iobj, doublereal* objadd, char* prob, U_fp fgcon, U_fp fgobj,
	doublereal* jcol, integer* indj, integer* locj, doublereal* bl,
	doublereal* bu, char* names, integer* hs, doublereal* x, doublereal*
	pi, doublereal* rc, integer* info, integer* mincw, integer* miniw,
	integer* minrw, integer* ns, integer* ninf, doublereal* sinf,
	doublereal* obj, char* cu, integer* lencu, integer* iu, integer*
	leniu, doublereal* ru, integer* lenru, char* cw, integer* lencw,
	integer* iw, integer* leniw, doublereal* rw, integer* lenrw, ftnlen
	start_len, ftnlen prob_len, ftnlen names_len, ftnlen cu_len, ftnlen
	cw_len);


int snopta_
(integer* start, integer* nf, integer* n,
	integer* nxname, integer* nfname, doublereal* objadd, integer* objrow,
	char* prob, U_fp usrfun, integer* iafun, integer* javar,
	integer* lena, integer* nea, doublereal* a, integer* igfun,
	integer* jgvar, integer* leng, integer* neg, doublereal* xlow,
	doublereal* xupp, char* xnames, doublereal* flow, doublereal* fupp,
	char* fnames, doublereal* x, integer* xstate, doublereal* xmul,
	doublereal* f, integer* fstate, doublereal* fmul, integer* inform__,
	integer* mincw, integer* miniw, integer* minrw, integer* ns,
	integer* ninf, doublereal* sinf, char* cu, integer* lencu, integer* iu,
	integer* leniu, doublereal* ru, integer* lenru, char* cw, integer* lencw,
	integer* iw, integer* leniw, doublereal* rw, integer* lenrw,
	ftnlen prob_len, ftnlen xnames_len, ftnlen fnames_len, ftnlen cu_len,
	ftnlen cw_len);



int snoptb_(char* start, integer* m, integer* n, integer* ne,
	integer* nname, integer* nncon, integer* nnobj, integer* nnjac,
	integer* iobj, doublereal* objadd, char* prob, U_fp fgcon, U_fp fgobj,
	doublereal* jcol, integer* indj, integer* locj, doublereal* bl,
	doublereal* bu, char* names, integer* hs, doublereal* x, doublereal*
	pi, doublereal* rc, integer* info, integer* mincw, integer* miniw,
	integer* minrw, integer* ns, integer* ninf, doublereal* sinf,
	doublereal* obj, char* cu, integer* lencu, integer* iu, integer*
	leniu, doublereal* ru, integer* lenru, char* cw, integer* lencw,
	integer* iw, integer* leniw, doublereal* rw, integer* lenrw, ftnlen
	start_len, ftnlen prob_len, ftnlen names_len, ftnlen cu_len, ftnlen
	cw_len);

int snoptc_(char* start, integer* m, integer* n, integer* ne,
	integer* nname, integer* nncon, integer* nnobj, integer* nnjac,
	integer* iobj, doublereal* objadd, char* prob, U_fp userfg,
	doublereal* jcol, integer* indj, integer* locj, doublereal* bl,
	doublereal* bu, char* names, integer* hs, doublereal* x, doublereal*
	pi, doublereal* rc, integer* info, integer* mincw, integer* miniw,
	integer* minrw, integer* ns, integer* ninf, doublereal* sinf,
	doublereal* obj, char* cu, integer* lencu, integer* iu, integer*
	leniu, doublereal* ru, integer* lenru, char* cw, integer* lencw,
	integer* iw, integer* leniw, doublereal* rw, integer* lenrw, ftnlen
	start_len, ftnlen prob_len, ftnlen names_len, ftnlen cu_len, ftnlen
	cw_len);

int snoptq_(char* start, U_fp qphx, integer* m, integer* n,
	integer* ne, integer* nname, integer* lencobj, integer* ncolh,
	integer* iobj, doublereal* objadd, char* prob, doublereal* acol,
	integer* inda, integer* loca, doublereal* bl, doublereal* bu,
	doublereal* cobj, char* names, integer* helast, integer* hs,
	doublereal* x, doublereal* pi, doublereal* rc, integer* info, integer
	* mincw, integer* miniw, integer* minrw, integer* ns, integer* ninf,
	doublereal* sinf, doublereal* obj, char* cu, integer* lencu, integer*
	iu, integer* leniu, doublereal* ru, integer* lenru, char* cw, integer
	* lencw, integer* iw, integer* leniw, doublereal* rw, integer* lenrw,
	ftnlen start_len, ftnlen prob_len, ftnlen names_len, ftnlen cu_len,
	ftnlen cw_len);

int sninit_
(integer* iPrint, integer* iSumm, char* cw,
	integer* lencw, integer* iw, integer* leniw,
	doublereal* rw, integer* lenrw, ftnlen cw_len);

int sngeti_
(char* buffer, integer* ivalue, integer* inform__,
	char* cw, integer* lencw, integer* iw,
	integer* leniw, doublereal* rw, integer* lenrw,
	ftnlen buffer_len, ftnlen cw_len);

int sngetr_
(char* buffer, doublereal* ivalue, integer* inform__,
	char* cw, integer* lencw, integer* iw,
	integer* leniw, doublereal* rw, integer* lenrw,
	ftnlen buffer_len, ftnlen cw_len);

int snset_
(char* buffer, integer* iprint, integer* isumm,
	integer* inform__, char* cw, integer* lencw,
	integer* iw, integer* leniw,
	doublereal* rw, integer* lenrw,
	ftnlen buffer_len, ftnlen cw_len);

int sngetc_
(char* buffer, char* ivalue, integer* inform__,
	char* cw, integer* lencw, integer* iw,
	integer* leniw, doublereal* rw, integer* lenrw,
	ftnlen buffer_len, ftnlen ivalue_len, ftnlen cw_len);

int snseti_
(char* buffer, integer* ivalue, integer* iprint,
	integer* isumm, integer* inform__, char* cw,
	integer* lencw, integer* iw, integer* leniw,
	doublereal* rw, integer* lenrw, ftnlen buffer_len,
	ftnlen cw_len);

int snsetr_
(char* buffer, doublereal* rvalue, integer* iprint,
	integer* isumm, integer* inform__, char* cw,
	integer* lencw, integer* iw, integer* leniw,
	doublereal* rw, integer* lenrw, ftnlen buffer_len,
	ftnlen cw_len);

int snspec_
(integer* ispecs, integer* inform__, char* cw,
	integer* lencw, integer* iw, integer* leniw,
	doublereal* rw, integer* lenrw, ftnlen cw_len);

int snmema_
(integer* iexit, integer* nf, integer* n, integer* nxname,
	integer* nfname, integer* nea, integer* neg,
	integer* mincw, integer* miniw,
	integer* minrw, char* cw, integer* lencw, integer* iw,
	integer* leniw, doublereal* rw, integer* lenrw,
	ftnlen cw_len);

int snjac_
(integer* inform__, integer* nf, integer* n, U_fp userfg, //My_fp
	integer* iafun, integer* javar, integer* lena,
	integer* nea, doublereal* a, integer* igfun,
	integer* jgvar, integer* leng, integer* neg,
	doublereal* x, doublereal* xlow, doublereal* xupp,
	integer* mincw, integer* miniw,
	integer* minrw, char* cu, integer* lencu,
	integer* iu, integer* leniu, doublereal* ru,
	integer* lenru, char* cw, integer* lencw, integer* iw,
	integer* leniw, doublereal* rw, integer* lenrw,
	ftnlen cu_len, ftnlen cw_len);

//   int snjac_
// 	(integer *iexit, integer *nf, integer *n, U_fp userfg, 
//    	integer *iafun, integer *javar, integer *lena, 
//    	integer *nea, doublereal *a, integer *igfun, 
// 	integer *jgvar, integer *leng, integer *neg, 
// 	doublereal *x, doublereal *xlow, doublereal *xupp, 
//    	integer *mincw, integer *miniw, 
// 	integer *minrw, char *cu, integer *lencu, 
//    	integer *iu, integer *leniu, doublereal *ru, integer *lenru, char *cw,
//    	 integer *lencw, integer *iw, integer *leniw, doublereal *rw, integer 
//  	*lenrw, ftnlen cu_len, ftnlen cw_len);

#ifdef __cplusplus
}
#endif

#endif
