///
/// @file      ColoredSurfacePlot.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-22
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ColoredSurfacePlot.hpp"
#include "ColoredSurfaceEnrichment.hpp"

AST_NAMESPACE_BEGIN

void ColoredSurfacePlot::createEnrichment(Qwt3D::Enrichment& p) {
    auto actualData = this->actualData();
    if (!actualData) return;
    if (actualData->datatype != Qwt3D::GRID) {
        return;
    }
    auto* ce = static_cast<ColoredSurfaceEnrichment*>(&p);
    auto* grid = static_cast<Qwt3D::GridData*>(actualData);
    ce->setData(grid);
    ce->assign(*this);
    ce->drawBegin();
    ce->drawEnd();
}

AST_NAMESPACE_END
