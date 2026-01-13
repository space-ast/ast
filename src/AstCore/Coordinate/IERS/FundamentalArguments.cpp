///
/// @file      FundamentalArguments.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-10
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

#include "FundamentalArguments.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN

void aFundamentalArguments_IERS2010(double t, FundamentalArguments &fundargs)
{
    // IERS 2010规范章动理论所涉及的行星基本轨道参数
    // 参考IERS技术手册36(IERS Technical Note 36)的公式5.43和公式5.44
    // https://www.iers.org/IERS/EN/Publications/TechnicalNotes/TechnicalNotes

    // F1 ≡ l = 134.96340251◦ + 1717915923.2178″t + 31.8792″t² + 0.051635″t³ − 0.00024470″t⁴
    fundargs.l() = 134.96340251*kDegToRad + kArcSecToRad * (1717915923.2178 + (31.8792 + (0.051635 - 0.00024470 * t)*t)*t)*t;
    // F2 ≡ l' = 357.52910918◦ + 129596581.0481″t − 0.5532″t² + 0.000136″t³ − 0.00001149″t⁴
    fundargs.lP() = 357.52910918*kDegToRad + kArcSecToRad * (129596581.0481 + (-0.5532 + (0.000136 - 0.00001149 * t)*t)*t)*t;
    // F3 ≡ F = 93.27209062◦ + 1739527262.8478″t − 12.7512″t² − 0.001037″t³ + 0.00000417″t⁴
    fundargs.f() = 93.27209062*kDegToRad + kArcSecToRad * (1739527262.8478 + (-12.7512 + (-0.001037 + 0.00000417 * t)*t)*t)*t;
    // F4 ≡ D = 297.85019547◦ + 1602961601.2090″t − 6.3706″t² + 0.006593″t³ − 0.00003169″t⁴
    fundargs.d() = 297.85019547*kDegToRad + kArcSecToRad * (1602961601.2090 + (-6.3706 + (0.006593 - 0.00003169 * t)*t)*t)*t;
    // F5 ≡ Ω = 125.04455501◦ − 6962890.5431″t + 7.4722″t² + 0.007702″t³ − 0.00005939″t⁴
    fundargs.om() = 125.04455501*kDegToRad + kArcSecToRad * (-6962890.5431 + (7.4722 + (0.007702 - 0.00005939 * t)*t)*t)*t;

    // F6 ≡ LMe = 4.402608842 + 2608.7903141574 × t
    fundargs.lMe() = 4.402608842 + 2608.7903141574 * t;
    // F7 ≡ LVe = 3.176146697 + 1021.3285546211 × t
    fundargs.lVe() = 3.176146697 + 1021.3285546211 * t;
    // F8 ≡ LE  = 1.753470314 +  628.3075849991 × t
    fundargs.lE() = 1.753470314 +  628.3075849991 * t;
    // F9 ≡ LMa = 6.203480913 +  334.0612426700 × t
    fundargs.lMa() = 6.203480913 +  334.0612426700 * t;
    // F10 ≡ LJ  = 0.599546497 +   52.9690962641 × t
    fundargs.lJu() = 0.599546497 +   52.9690962641 * t;
    // F11 ≡ LSa = 0.874016757 +   21.3299104960 × t
    fundargs.lSa() = 0.874016757 +   21.3299104960 * t;
    // F12 ≡ LU  = 5.481293872 +    7.4781598567 × t
    fundargs.lUr() = 5.481293872 +    7.4781598567 * t;
    // F13 ≡ LNe = 5.311886287 +    3.8133035638 × t
    fundargs.lNe() = 5.311886287 +    3.8133035638 * t;
    // F14 ≡ pA  = 0.02438175 × t + 0.00000538691 × t²
    fundargs.pa() = (0.02438175 + 0.00000538691 * t) * t;
}

void aFundamentalArguments_IERS2003(double t, FundamentalArguments &fundargs)
{
    // 2003年规范章动理论所涉及的行星基本轨道参数与2010年相同
    return aFundamentalArguments_IERS2010(t, fundargs);
}

void aFundamentalArgumentsNormalizeAngle0To2Pi(FundamentalArguments &fundargs)
{
    fundargs.l() = aNormalizeAngle0To2Pi(fundargs.l());
    fundargs.lP() = aNormalizeAngle0To2Pi(fundargs.lP());
    fundargs.f() = aNormalizeAngle0To2Pi(fundargs.f());
    fundargs.d() = aNormalizeAngle0To2Pi(fundargs.d());
    fundargs.om() = aNormalizeAngle0To2Pi(fundargs.om());
    fundargs.lMe() = aNormalizeAngle0To2Pi(fundargs.lMe());
    fundargs.lVe() = aNormalizeAngle0To2Pi(fundargs.lVe());
    fundargs.lE() = aNormalizeAngle0To2Pi(fundargs.lE());
    fundargs.lMa() = aNormalizeAngle0To2Pi(fundargs.lMa());
    fundargs.lJu() = aNormalizeAngle0To2Pi(fundargs.lJu());
    fundargs.lSa() = aNormalizeAngle0To2Pi(fundargs.lSa());
    fundargs.lUr() = aNormalizeAngle0To2Pi(fundargs.lUr());
    fundargs.lNe() = aNormalizeAngle0To2Pi(fundargs.lNe());
    fundargs.pa() = aNormalizeAngle0To2Pi(fundargs.pa());
}

AST_NAMESPACE_END


