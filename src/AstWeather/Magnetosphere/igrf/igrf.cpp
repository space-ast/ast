///
/// @file      igrf.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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

#include "igrf.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/WorkingDirectory.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/RunTime.hpp"

#ifndef AST_NO_LIBF2C
#include "f2c.h"

extern "C"{
    int initize_(void);
    int feldcof_(doublereal *year, doublereal *dimo);
    int feldg_(doublereal *glat, doublereal *glon, doublereal *
	alt, doublereal *bnorth, doublereal *beast, doublereal *bdown, 
	doublereal *babs);
    int shellg_(doublereal *glat, doublereal *glon, doublereal *
	alt, doublereal *dimo, doublereal *fl, integer *icode, doublereal *b0);
    int findb0_(doublereal *stps, doublereal *bdel, logical *
	value, doublereal *bequ, doublereal *rr0);
}


#endif

AST_NAMESPACE_BEGIN

void igrf(double lon, double lat, double height, double year, double &xl, double &bbx)
{
#ifndef AST_NO_LIBF2C
    fs::path datadir = aDataDir();
    datadir /= AST_DEFAULT_DIR_IGRF;
    // 切换工作目录到 igrf 数据目录
    WorkingDirectory _{datadir.string()};
    
    initize_();
    double dimo;
    feldcof_(&year, &dimo);
    double bnorth, beast, bdown, babs;
    feldg_(&lat, &lon, &height, &bnorth, &beast, &bdown, &babs);
    long icode = 0;
    double bab1;
    shellg_(&lat, &lon, &height, &dimo, &xl, &icode, &bab1);
    double bequ = dimo / (xl * xl * xl);
    if(icode == 1){
        double bdel = 1e-3;
        double step = 0.05;
        long val = 0; 
        double beq;
        double rr0;
        findb0_(&step, &bdel, &val, &beq, &rr0);
        if(val)
            bequ = beq;
    }
    bbx = babs / bequ;
#else
    aError("igrf not implemented, please check whether libf2c package is enabled.");
#endif
}


AST_NAMESPACE_END

