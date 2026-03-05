///
/// @file      AxesTransform.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-04
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

#include "AxesTransform.hpp"
#include "AstCore/BuiltinAxes.hpp"


AST_NAMESPACE_BEGIN

Axes *aAxesRoot()
{
    return AxesRoot::Instance();
}

Axes *aAxesICRF()
{
    return AxesICRF::Instance();
}

Axes *aAxesECF()
{
    return AxesECF::Instance();
}


Axes *aAxesJ2000()
{
    return AxesJ2000::Instance();
}

Axes *aAxesMOD()
{
    return AxesMOD::Instance();
}

Axes *aAxesTOD()
{
    return AxesTOD::Instance();
}

Axes *aAxesGTOD()
{
    return AxesGTOD::Instance();
}

Axes *aAxesB1950()
{
    return AxesB1950::Instance();
}

Axes* aAxesB1950Spice()
{
    return AxesB1950Spice::Instance();
}

AST_NAMESPACE_END
