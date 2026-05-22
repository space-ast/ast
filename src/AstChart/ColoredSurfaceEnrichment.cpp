///
/// @file      ColoredSurfaceEnrichment.cpp
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

#include "ColoredSurfaceEnrichment.hpp"
#include "qwt3d_openglhelper.h"

AST_NAMESPACE_BEGIN

#define GL2PS_POLYGON_OFFSET_FILL 1

extern "C" {
A_DECL_IMPORT GLint gl2psLineWidth(GLfloat value);
A_DECL_IMPORT GLint gl2psEnable(GLint mode);
}

void setDeviceLineWidth(GLfloat val)
{
    if (val < 0)
        val = 0;

    GLint ret = gl2psLineWidth(val);

    GLfloat lw[ 2 ];
    glGetFloatv(GL_LINE_WIDTH_RANGE, lw);

    if (val < lw[ 0 ])
        val = lw[ 0 ];
    else if (val > lw[ 1 ])
        val = lw[ 1 ];

    glLineWidth(val);
}


void setDevicePolygonOffset(GLfloat factor, GLfloat units)
{
    glPolygonOffset(factor, units);
    gl2psEnable(GL2PS_POLYGON_OFFSET_FILL);
}


// === ColoredSurfaceEnrichment ===

void ColoredSurfaceEnrichment::setData(const Qwt3D::GridData* grid) {
    grid_ = grid;
}

void ColoredSurfaceEnrichment::drawBegin() {
    if (!grid_) return;
    if (grid_->vertices.empty()) return;

    const auto* surface = this->surface();
    
    setDeviceLineWidth(surface->meshLineWidth());
    
    Qwt3D::GLStateBewarer sb(GL_POLYGON_OFFSET_FILL, true);
    setDevicePolygonOffset(surface->polygonOffset(), 1.0);

    Qwt3D::GLStateBewarer sb2(GL_LINE_SMOOTH, surface->smoothDataMesh());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto plotStyle = this->plotStyle();
    if (plotStyle != Qwt3D::WIREFRAME) {
        drawFaces();
    }

    if (plotStyle == Qwt3D::FILLEDMESH || plotStyle == Qwt3D::WIREFRAME || plotStyle == Qwt3D::HIDDENLINE) {
        drawColoredMeshLines();
    }
}

void ColoredSurfaceEnrichment::draw(Qwt3D::Triple const &)
{
}

void ColoredSurfaceEnrichment::drawFaces() {
    int lastcol = static_cast<int>(grid_->vertices.size());
    int lastrow = lastcol > 0 ? static_cast<int>(grid_->vertices[0].size()) : 0;
    const auto* surface = this->surface();
    auto* color = surface->dataColor();
    int step = surface->resolution();

    glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);

    bool hl = (plotStyle() == Qwt3D::HIDDENLINE);
    if (hl) {
        Qwt3D::RGBA col = surface->backgroundRGBAColor();
        glColor4d(col.r, col.g, col.b, col.a);
    }

    auto grid = grid_;
    auto setVertex = [grid, color, hl](int col, int row) {
        if (!hl) {
            const auto& v = grid->vertices[col][row];
            Qwt3D::RGBA c = (*color)(v[0], v[1], v[2]);
            glColor4d(c.r, c.g, c.b, c.a);
        }
        glNormal3dv(grid->normals[col][row]);
        glVertex3dv(grid->vertices[col][row]);
    };
        
    for (int i = 0; i < lastcol - step; i += step) {


        glBegin(GL_TRIANGLE_STRIP);

        setVertex(i, 0);
        setVertex(i + step, 0);

        for (int j = 0; j < lastrow - step; j += step) {
            setVertex(i, j + step);
            setVertex(i + step, j + step);
        }
        glEnd();
    }
}

void ColoredSurfaceEnrichment::drawColoredMeshLines() {
    int cols = static_cast<int>(grid_->vertices.size());
    int rows = cols > 0 ? static_cast<int>(grid_->vertices[0].size()) : 0;
    const auto* surface = this->surface();
    auto* color = surface->dataColor();
    int step = surface->resolution();

    auto grid = grid_;
    auto setColoredVertex = [grid, color](int col, int row) {
        const auto& v = grid->vertices[col][row];
        Qwt3D::RGBA c = (*color)(v[0], v[1], v[2]);
        glColor4d(c.r, c.g, c.b, c.a);
        glVertex3dv(v);
    };

    // 边界线 (LINE_LOOP)
    if (step < cols && step < rows) {
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < cols - step; i += step)
            setColoredVertex(i, 0);
        for (int j = 0; j < rows - step; j += step)
            setColoredVertex(cols - step, j);
        for (int i = cols - step; i >= 0; i -= step)
            setColoredVertex(i, rows - step);
        for (int j = rows - step; j >= 0; j -= step)
            setColoredVertex(0, j);
        glEnd();
    }

    // 列方向的编织线
    for (int i = step; i < cols - step; i += step) {
        glBegin(GL_LINE_STRIP);
        for (int j = 0; j < rows; j += step)
            setColoredVertex(i, j);
        glEnd();
    }

    // 行方向的编织线
    for (int j = step; j < rows - step; j += step) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < cols; i += step)
            setColoredVertex(i, j);
        glEnd();
    }
}



const Qwt3D::SurfacePlot *ColoredSurfaceEnrichment::surface() const
{
    return static_cast<const Qwt3D::SurfacePlot*>(plot);
}



AST_NAMESPACE_END

