/// @file      MockSnopt.c
/// @brief     
/// @details   ~
/// @author    axel
/// @date      28.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。


#include <stdio.h>
#include "AstGlobal.h"

typedef long int integer;
typedef double   doublereal;
typedef long int ftnlen;
#ifdef __cplusplus
typedef int (*U_fp)(...);
#else
typedef int (*U_fp)();
#endif
typedef int (*My_fp)( integer *Status, integer *n,
       doublereal x[],     integer *needF, integer *neF,  doublereal F[],
       integer    *needG,  integer *neG,  doublereal G[],
       char       *cu,     integer *lencu,
       integer    iu[],    integer *leniu,
       doublereal ru[],    integer *lenru );


AST_MOCK_CAPI
void snopta_(integer* start, integer* nf, integer* n,
    integer* nxname, integer* nfname, doublereal* objadd, integer* objrow,
    char* prob, My_fp usrfun, integer* iafun, integer* javar,
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
    ftnlen cw_len)
{
    printf("call mock snopta_\n");
}

AST_MOCK_CAPI
void sninit_(integer* iPrint, integer* iSumm, char* cw,
    integer* lencw, integer* iw, integer* leniw,
    doublereal* rw, integer* lenrw, ftnlen cw_len)
{
	printf("call mock sninit_\n");
}

AST_MOCK_CAPI
void sngeti_(char* buffer, integer* ivalue, integer* inform__,
    char* cw, integer* lencw, integer* iw,
    integer* leniw, doublereal* rw, integer* lenrw,
    ftnlen buffer_len, ftnlen cw_len)
{
	printf("call mock sngeti_\n");
}

AST_MOCK_CAPI
void sngetr_(char* buffer, doublereal* ivalue, integer* inform__,
    char* cw, integer* lencw, integer* iw,
    integer* leniw, doublereal* rw, integer* lenrw,
    ftnlen buffer_len, ftnlen cw_len)
{
	printf("call mock sngetr_\n");
}

AST_MOCK_CAPI
void snset_(char* buffer, integer* iprint, integer* isumm,
    integer* inform__, char* cw, integer* lencw,
    integer* iw, integer* leniw,
    doublereal* rw, integer* lenrw,
    ftnlen buffer_len, ftnlen cw_len)
{
	printf("call mock snset_\n");
}

AST_MOCK_CAPI
void sngetc_(char* buffer, char* ivalue, integer* inform__,
    char* cw, integer* lencw, integer* iw,
    integer* leniw, doublereal* rw, integer* lenrw,
    ftnlen buffer_len, ftnlen ivalue_len, ftnlen cw_len)
{
	printf("call mock sngetc_\n");
}

AST_MOCK_CAPI
void snseti_(char* buffer, integer* ivalue, integer* iprint,
    integer* isumm, integer* inform__, char* cw,
    integer* lencw, integer* iw, integer* leniw,
    doublereal* rw, integer* lenrw, ftnlen buffer_len,
    ftnlen cw_len)
{
	printf("call mock snseti_\n");
}

AST_MOCK_CAPI
void snsetr_(char* buffer, doublereal* rvalue, integer* iprint,
    integer* isumm, integer* inform__, char* cw,
    integer* lencw, integer* iw, integer* leniw,
    doublereal* rw, integer* lenrw, ftnlen buffer_len,
    ftnlen cw_len)
{
	printf("call mock snsetr_\n");
}

AST_MOCK_CAPI
void snspec_(integer* ispecs, integer* inform__, char* cw,
    integer* lencw, integer* iw, integer* leniw,
    doublereal* rw, integer* lenrw, ftnlen cw_len)
{
	printf("call mock snspec_\n");
}

AST_MOCK_CAPI
void snmema_(integer* iexit, integer* nf, integer* n, integer* nxname,
    integer* nfname, integer* nea, integer* neg,
    integer* mincw, integer* miniw,
    integer* minrw, char* cw, integer* lencw, integer* iw,
    integer* leniw, doublereal* rw, integer* lenrw,
    ftnlen cw_len)
{
	printf("call mock snmema_\n");
}

AST_MOCK_CAPI
void snjac_(integer* inform__, integer* nf, integer* n, My_fp userfg,
    integer* iafun, integer* javar, integer* lena,
    integer* nea, doublereal* a, integer* igfun,
    integer* jgvar, integer* leng, integer* neg,
    doublereal* x, doublereal* xlow, doublereal* xupp,
    integer* mincw, integer* miniw,
    integer* minrw, char* cu, integer* lencu,
    integer* iu, integer* leniu, doublereal* ru,
    integer* lenru, char* cw, integer* lencw, integer* iw,
    integer* leniw, doublereal* rw, integer* lenrw,
    ftnlen cu_len, ftnlen cw_len)
{
	printf("call mock snjac_\n");
}

