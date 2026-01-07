///
/// @file      SOFA.hpp
/// @brief     天文基础计算函数（基于SOFA库实现）
/// @details   本模块的函数基于国际天文联合会(IAU) SOFA库实现，
///            但为适应本项目需求进行了接口和实现上的调整。
/// @author    axel
/// @date      2026-01-06
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN


//==============================================================================
// SOFA (Standards of Fundamental Astronomy) 许可证声明
//==============================================================================
/*
 * 重要声明：本文件中的函数基于国际天文联合会(IAU)的SOFA软件库实现。
 *
 * 根据SOFA软件许可证的要求，特此声明：
 *
 * 1. 本模块的代码使用了从SOFA软件库（版本2023-10-11）获得的算法和计算例程。
 *    具体对应的SOFA函数请参考函数注释
 *
 * 2. SOFA软件由国际天文联合会(IAU) SOFA Board所有。
 *    SOFA软件许可证请参考：https://www.iausofa.org/terms-and-conditions
 *
 * 3. 根据SOFA许可证第3(c)条要求，本衍生作品的函数名不使用"iau"或"sofa"前缀。
 *    本模块使用"a"前缀表示"astronomy"天文计算。
 *
 * 4. 根据SOFA许可证第3(a)条要求，声明本软件：
 *    (i) 使用了从SOFA软件派生的计算例程；
 *    (ii) 不是由SOFA提供或认可的软件。
 *
 * 5. 本衍生作品与原始SOFA软件的主要区别：
 *    - 函数接口针对ast项目的TimePoint等自定义类型进行了适配
 *    - 函数命名遵循ast项目的命名规范
 *    - 部分实现细节根据项目需求进行了优化和调整
 *    - 不保证与原始SOFA库在边界情况和精度上完全一致
 *
 * 6. 本声明根据SOFA许可证第3(e)条要求，必须在任何源代码分发中完整保留。
 * 
 */



/// @brief 计算给定时间差的岁差角，依据IAU1976规范
/// @details 参考SOFA函数 iauPrec76()
/// @param[in] t 与J2000.0 TDB 的时间差（儒略世纪）
/// @param[out] zeta 岁差zeta角（弧度）
/// @param[out] z 岁差z角（弧度）
/// @param[out] theta 岁差theta角（弧度）
AST_CORE_CAPI void aPrecession_IAU1976(double t, double& zeta, double& z, double& theta);



/// @brief 计算给定时间差的平均黄赤交角，依据IAU1980规范
/// @details 参考SOFA函数 iauObl80()
/// @param[in] t 与J2000.0 TT 的时间差（儒略世纪）
/// @return 平均黄赤交角（弧度）
AST_CORE_CAPI double aMeanObliquity_IAU1980(double t);


/// @brief 计算给定时间差的章动角，依据IAU1980规范
/// @details 参考SOFA函数 iauNut80()
/// @param[in] t 与J2000.0 TT 的时间差（儒略世纪）
/// @param[out] dpsi 章动角dpsi（弧度）
/// @param[out] deps 章动角deps（弧度）
AST_CORE_CAPI void aNutation_IAU1980(double t, double &dpsi, double &deps);




A_ALWAYS_INLINE void aPrecession_IAU1976(const TimePoint& tp, double& zeta, double& z, double& theta){
    /// @fixme
    /// aPrecession_IAU1976函数的参数是TDB，而不是TT
    /// 直接将TT作为TDB使用是可行的吗？
    /// 参考SOFA的 iauPmat76， 也是将TT作为TDB使用
    aPrecession_IAU1976(tp.julianCenturyFromJ2000TT(), zeta, z, theta);
}


A_ALWAYS_INLINE double aMeanObliquity_IAU1980(const TimePoint& tp){
    return aMeanObliquity_IAU1980(tp.julianCenturyFromJ2000TT());
}


typedef err_t (*NutationFunc)(const TimePoint& tp, double &dpsi, double &deps);

A_ALWAYS_INLINE void aNutation_IAU1980(const TimePoint& tp, double& dpsi, double& deps)
{
    return aNutation_IAU1980(tp.julianCenturyFromJ2000TT(), dpsi, deps);
}

/// @brief 章动角计算函数指针
/// @details 指向内部实现的函数指针，默认指向aNutation_IAU1980
/// 可根据需要切换为其他实现，例如通过JPL DE星历数据计算章动角
/// 或者用户提供的其他实现，注意需要确保接口一致（参数和返回值），否则会导致调用错误
AST_CORE_CAPI NutationFunc aNutation;



/// @brief 设置章动角计算函数指针
/// @details 
/// 根据输入的函数指针，切换内部实现的章动角计算函数指针，例如用户提供的其他实现
/// 注意需要确保接口一致（参数和返回值），否则会导致调用错误
inline void aNutationFuncSet(NutationFunc func){
    aNutation = func;
}

enum class ENutationMethod
{
    eIAU1980 = 0,
    eJplDe = 1,
    eUser = 2,      // 用户自定义实现
};

/// @brief 设置章动角计算函数
/// @details 根据输入的枚举值，切换内部实现的章动角计算函数指针
/// @param[in] func 枚举值，指定要切换的章动角计算函数
/// @return err_t 错误码，0表示成功，其他值表示失败
AST_CORE_CAPI err_t aNutationMethodSet(ENutationMethod method);


/// @brief 获取当前章动角计算函数
/// @details 返回当前内部实现的章动角计算函数指针
/// @return ENutationMethod 当前章动角计算函数的枚举值
AST_CORE_CAPI ENutationMethod aNutationMethodGet();


AST_NAMESPACE_END
