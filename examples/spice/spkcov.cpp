///
/// @file      spkcov.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-03-26
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#ifndef AST_NO_CSPICE
#include "SpiceUsr.h"
#include "ast/TestConfig.hpp"
#include "ast/Environment.hpp"
#include <cstdio>

AST_USING_NAMESPACE

int main()
{
/*
Local parameters
*/
#define FILSIZ 256
#define MAXIV 1000
#define WINSIZ (2 * MAXIV)
#define TIMLEN 51
#define MAXOBJ 1000

    if(aIsGithubCI())
        return 0;
    /*
    Local variables
    */
    SPICEDOUBLE_CELL(cover, WINSIZ);
    SPICEINT_CELL(ids, MAXOBJ);

    SpiceChar timstr[TIMLEN];

    SpiceDouble b;
    SpiceDouble e;

    SpiceInt i;
    SpiceInt j;
    SpiceInt niv;
    SpiceInt obj;

    std::string lsk = aTestGetConfigValue("LSK_FILE").toString();
    furnsh_c(lsk.c_str());

    std::string spkfnm = aTestGetConfigValue("SPK_FILE").toString();
    furnsh_c(spkfnm.c_str());

    /*
    Find the set of objects in the SPK file.
    */
    spkobj_c(spkfnm.c_str(), &ids);

    /*
    We want to display the coverage for each object. Loop over
    the contents of the ID code set, find the coverage for
    each item in the set, and display the coverage.
    */
    for (i = 0; i < card_c(&ids); i++)
    {
        /*
        Find the coverage window for the current object.
        Empty the coverage window each time so we don't
        include data for the previous object.
        */
        obj = SPICE_CELL_ELEM_I(&ids, i);

        scard_c(0, &cover);
        spkcov_c(spkfnm.c_str(), obj, &cover);

        /*
        Get the number of intervals in the coverage window.
        */
        niv = wncard_c(&cover);

        /*
        Display a simple banner.
        */
        printf("%s\n", "========================================");

        printf("Coverage for object %d\n", (int)obj);
        /*
        Convert the coverage interval start and stop times to TDB
        calendar strings.
        */
        for (j = 0; j < niv; j++)
        {
            /*
            Get the endpoints of the jth interval.
            */
            wnfetd_c(&cover, j, &b, &e);

            /*
            Convert the endpoints to TDB calendar
            format time strings and display them.
            */
            timout_c(b,
                     "YYYY MON DD HR:MN:SC.### (TDB) ::TDB",
                     TIMLEN,
                     timstr);

            printf("\n"
                   "Interval:  %d\n"
                   "Start:     %s\n",
                   (int)j,
                   timstr);

            timout_c(e,
                     "YYYY MON DD HR:MN:SC.### (TDB) ::TDB",
                     TIMLEN,
                     timstr);
            printf("Stop:      %s\n", timstr);
        }
    }
}
#else
int main() {}
#endif
