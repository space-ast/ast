///
/// @file      ColoredMeshEnrichment.hpp
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

#pragma once

#include "AstGlobal.h"
#include <qwt3d_surfaceplot.h>
#include <qwt3d_enrichment.h>


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/



/// Qwt3D Enrichment 子类 — 在 drawBegin() 中渲染遮挡面 + 逐顶点着色的网格线
/// 配合 ColoredMeshPlot 使用
class ColoredSurfaceEnrichment : public Qwt3D::VertexEnrichment {
public:
    ColoredSurfaceEnrichment() = default;
    ColoredSurfaceEnrichment(Qwt3D::PLOTSTYLE plotStyle) 
        : plotStyle_(plotStyle) {}
    ColoredSurfaceEnrichment* clone() const override { return new ColoredSurfaceEnrichment(*this); }
    void drawBegin() override;
    void draw(Qwt3D::Triple const &) override;
public:
    /// 在 createEnrichment() 覆写中，drawBegin() 调用前设置数据
    void setData(const Qwt3D::GridData* grid);
    void setPlotStyle(Qwt3D::PLOTSTYLE plotStyle){plotStyle_ = plotStyle;}
    Qwt3D::PLOTSTYLE plotStyle() const {return plotStyle_;}
    const Qwt3D::SurfacePlot* surface() const;

private:
    const Qwt3D::GridData* grid_{nullptr};
    Qwt3D::PLOTSTYLE plotStyle_{Qwt3D::WIREFRAME};

    void drawFaces();               ///< 面渲染
    void drawColoredMeshLines();    ///< 逐顶点着色的网格线
};


/*! @} */

AST_NAMESPACE_END
