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

#ifdef AST_WITH_LIBF2C
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

///
/// @brief 利用 IGRF 地磁场模型计算磁壳参数 L 和磁场比值 B/B₀
///
/// 通过对底层 Fortran 例程的顺序调用，将地理坐标转换为辐射带建模所需的磁坐标参数。
///
/// 内部流程：
///   1. INITIZE  — 从数据文件加载 IGRF 高斯系数
///   2. FELDCOF  — 根据小数年份插值计算该时刻的地磁场系数和偶极矩 DIMO
///   3. FELDG    — 计算指定位置的地磁场分量（北向、东向、垂直、总强度 BABS）
///   4. SHELLG   — 沿磁力线追踪，计算 McIlwain L 参数。返回状态码 ICODE：
///                 - 1: 正常完成
///                 - 2: 共轭点不在物理范围内（此时 L 值无意义）
///                 - 3: 磁壳参数超出精确计算上限，使用近似值
///   5. BEQU = DIMO / L³ — 用偶极子近似估计赤道磁场强度
///   6. FINDB0   — （仅当 ICODE=1 即正常完成时）迭代求解更精确的赤道磁场 B₀，
///                 以修正偶极子近似的误差。若迭代不收敛，保留偶极子近似值
///   7. BBX = BABS / BEQU — 计算当地磁场强度与赤道磁场强度的比值
///
/// @see 参考实现：
///   - IGRF: https://github.com/nasa/radbelt/blob/main/radbelt/core.f#IGRF
///   - bilcal: https://ccmc.gsfc.nasa.gov/pub/modelweb/geomagnetic/igrf/fortran_code/bilcal.for
///             https://git.smce.nasa.gov/ccmc-share/modelwebarchive/-/blob/main/IGRF/fortran_code/bilcal.for
///
void igrf(double lon, double lat, double height, double year, double &xl, double &bbx)
{
#ifdef AST_WITH_LIBF2C
    fs::path datadir = aDataDir();
    datadir /= AST_DEFAULT_DIR_IGRF;
    // 切换工作目录到 igrf 数据目录（底层 Fortran 代码通过相对路径读取数据文件）
    WorkingDirectory cwd{datadir.string()};
    if(!cwd.isChanged()){
        aError("failed to change working directory to '%.*s'", datadir.string().size(), datadir.string().data());
        return;
    }

    // 第1步：初始化 IGRF 模型，从数据文件加载高斯系数
    initize_();

    // 第2步：根据年份插值计算地磁场系数，DIMO = 偶极矩
    double dimo;
    feldcof_(&year, &dimo);

    // 第3步：计算指定位置的地磁场三分量及总强度
    //   BNORTH = 北向分量（地理北，非磁北）
    //   BEAST  = 东向分量
    //   BDOWN  = 垂直向下分量
    //   BABS   = 磁场总强度（标量值）
    double bnorth, beast, bdown, babs;
    feldg_(&lat, &lon, &height, &bnorth, &beast, &bdown, &babs);

    // 第4步：沿磁力线追踪，计算 McIlwain L 参数
    //   XL     = McIlwain L 值（输出，以地球半径为单位）
    //   ICODE  = 状态码：
    //              1 = 正常完成
    //              2 = 共轭点不在物理范围内（L 值无意义）
    //              3 = 磁壳参数超出精确计算上限，使用近似值
    //   BAB1   = 沿磁力线追踪的辅助量
    integer icode = 0;
    double bab1;
    shellg_(&lat, &lon, &height, &dimo, &xl, &icode, &bab1);

    // 第5步：用偶极子近似估计赤道磁场强度 B_eq ≈ DIMO / L³
    //        当 ICODE ≠ 1（非正常完成）时，直接使用该近似值；
    //        当 ICODE = 1（正常完成）时，该值作为下一步迭代的初值
    double bequ = dimo / (xl * xl * xl);

    // 第6步：正常完成时（ICODE=1），以偶极子近似值为初值，
    //       沿磁力线迭代追踪（步长 0.05、收敛精度 1e-3）精确求解赤道磁场 B₀。
    //       若迭代不收敛（VAL=false），保留偶极子近似值。
    //       当 ICODE=2 或 3 时跳过此步——此时 L 值不准或超出范围，迭代无意义。
    if(icode == 1){
        double bdel = 1e-3;   // 收敛精度
        double step = 0.05;   // 追踪步长
        long val = 0;         // 迭代是否成功标志
        double beq;           // 迭代得到的赤道磁场
        double rr0;           // 迭代得到的赤道半径
        findb0_(&step, &bdel, &val, &beq, &rr0);
        if(val)
            bequ = beq;       // 迭代收敛，采用修正后的赤道磁场值
    }

    // 第7步：计算 B/B₀ 比值（磁镜比）
    bbx = babs / bequ;
#else
    aError("igrf not implemented, please check whether libf2c package is enabled.");
#endif
}


AST_NAMESPACE_END

