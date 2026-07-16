///
/// @file      QwtPlotVisitor.cpp
/// @brief     matplot++ visitor that creates Qwt plot items
/// @details   Implements matplot::visitor to convert axes_objects into Qwt plot items
/// @author    axel
/// @date      2026-05-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "QwtPlotVisitor.hpp"
#include "ColoredSurfacePlot.hpp"
#include "ColoredSurfaceEnrichment.hpp"

#include <matplot/axes_objects/function_line.h>
#include <matplot/axes_objects/histogram.h>
#include <matplot/axes_objects/line.h>
#include <matplot/axes_objects/stair.h>
#include <matplot/axes_objects/surface.h>
#include <matplot/core/line_spec.h>
#include <matplot/core/axes_type.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_histogram.h>
#include <qwt_symbol.h>
#include <qwt_series_data.h>

#include <QPen>
#include <QColor>
#include <QDebug>

#include <qwt3d_surfaceplot.h>
#include <qwt3d_color.h>

#include <algorithm>
#include <cmath>

AST_NAMESPACE_BEGIN

// 按 C_data 值 + colormap 着色，仿 StandardColor 模式
class CDataColor : public Qwt3D::Color {
public:
    CDataColor(const std::vector<std::vector<double>>& C,
               double xMin, double xMax, double yMin, double yMax,
               double cMin, double cMax, Qwt3D::ColorVector cv)
        : C_(C), xMin_(xMin), xMax_(xMax), yMin_(yMin), yMax_(yMax),
          cMin_(cMin), cMax_(cMax), cv_(std::move(cv)),
          nCols_(static_cast<int>(C[0].size())), nRows_(static_cast<int>(C.size()))
    {}

    Qwt3D::RGBA operator()(double x, double y, double) const override {
        double dx = nCols_ > 1 ? (xMax_ - xMin_) / (nCols_ - 1) : 1.0;
        double dy = nRows_ > 1 ? (yMax_ - yMin_) / (nRows_ - 1) : 1.0;
        int col = std::max(0, std::min(nCols_ - 1,
            static_cast<int>(std::round((x - xMin_) / dx))));
        int row = std::max(0, std::min(nRows_ - 1,
            static_cast<int>(std::round((y - yMin_) / dy))));
        // 用 C 值代替 Z 值映射 colormap，和 StandardColor 一致
        double cVal = C_[static_cast<size_t>(row)][static_cast<size_t>(col)];
        double cRng = (cMax_ > cMin_) ? (cMax_ - cMin_) : 1.0;
        int idx = static_cast<int>((cv_.size() - 1) * (cVal - cMin_) / cRng);
        idx = std::max(0, std::min(static_cast<int>(cv_.size()) - 1, idx));
        return cv_[static_cast<size_t>(idx)];
    }
    Qwt3D::ColorVector& createVector(Qwt3D::ColorVector& vec) override {
        vec = cv_;
        return vec;
    }
private:
    const std::vector<std::vector<double>>& C_;
    double xMin_, xMax_, yMin_, yMax_, cMin_, cMax_;
    Qwt3D::ColorVector cv_;
    int nCols_, nRows_;
};

static Qt::PenStyle toPenStyle(enum matplot::line_spec::line_style s) {
    switch (s) {
    case matplot::line_spec::line_style::none:
        return Qt::NoPen;
    case matplot::line_spec::line_style::dashed_line:
        return Qt::DashLine;
    case matplot::line_spec::line_style::dotted_line:
        return Qt::DotLine;
    case matplot::line_spec::line_style::dash_dot_line:
        return Qt::DashDotLine;
    case matplot::line_spec::line_style::solid_line:
    default:
        return Qt::SolidLine;
    }
}

static QwtSymbol::Style toSymbolStyle(enum matplot::line_spec::marker_style s) {
    switch (s) {
    case matplot::line_spec::marker_style::plus_sign:
        return QwtSymbol::Cross;
    case matplot::line_spec::marker_style::circle:
        return QwtSymbol::Ellipse;
    case matplot::line_spec::marker_style::asterisk:
        return QwtSymbol::Star1;
    case matplot::line_spec::marker_style::point:
        return QwtSymbol::Ellipse;  // small filled circle
    case matplot::line_spec::marker_style::cross:
        return QwtSymbol::XCross;
    case matplot::line_spec::marker_style::square:
        return QwtSymbol::Rect;
    case matplot::line_spec::marker_style::diamond:
        return QwtSymbol::Diamond;
    case matplot::line_spec::marker_style::upward_pointing_triangle:
        return QwtSymbol::UTriangle;
    case matplot::line_spec::marker_style::downward_pointing_triangle:
        return QwtSymbol::DTriangle;
    case matplot::line_spec::marker_style::right_pointing_triangle:
        return QwtSymbol::RTriangle;
    case matplot::line_spec::marker_style::left_pointing_triangle:
        return QwtSymbol::LTriangle;
    case matplot::line_spec::marker_style::none:
    default:
        return QwtSymbol::NoSymbol;
    }
}

QwtPlotVisitor::QwtPlotVisitor(QwtPlot* plot) : plot_(plot) {}
QwtPlotVisitor::QwtPlotVisitor(ColoredSurfacePlot* surface) : surface_(surface) {}
QwtPlotVisitor::~QwtPlotVisitor() = default;

void QwtPlotVisitor::visit(matplot::line& l) {
    auto& ls = l.line_spec();
    auto& x = l.x_data();
    auto& y = l.y_data();

    if (y.empty()) return;

    auto* curve = new QwtPlotCurve();
    if (x.empty()) {
        std::vector<double> implicitX(y.size());
        for (size_t i = 0; i < y.size(); ++i) implicitX[i] = static_cast<double>(i + 1);
        curve->setSamples(implicitX.data(), y.data(), static_cast<int>(y.size()));
    } else {
        curve->setSamples(x.data(), y.data(), static_cast<int>(std::min(x.size(), y.size())));
    }

    auto color = toQColor(l.color());
    float lw = l.line_width();

    auto penStyle = toPenStyle(ls.line_style());
    if (penStyle != Qt::NoPen) {
        curve->setPen(QPen(color, lw, penStyle));
        curve->setStyle(QwtPlotCurve::Lines);
    } else {
        curve->setStyle(QwtPlotCurve::NoCurve);
    }

    auto ms = ls.marker_style();
    if (ms != matplot::line_spec::marker_style::none) {
        auto* symbol = new QwtSymbol();
        symbol->setStyle(toSymbolStyle(ms));
        auto markerColor = toQColor(l.marker_color());
        symbol->setPen(QPen(markerColor, 1.0));
        int sz = static_cast<int>(l.marker_size());
        symbol->setSize(QSize(sz, sz));
        if (l.marker_face()) {
            auto faceColor = toQColor(l.marker_face_color());
            // qDebug() << "faceColor:" << faceColor;
            symbol->setBrush(QBrush(faceColor));
        }
        curve->setSymbol(symbol);
    }

    curve->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
    curve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);
    // setLegendIconSize要放在setLegendAttribute后面和setSymbol后面，避免legend大小被重置
    curve->setLegendIconSize(QSize(40, 8));
    curve->setVisible(l.visible());
    curve->attach(plot_);
}

void QwtPlotVisitor::visit(matplot::histogram& h) {
    auto& values = h.values();
    auto& edges = h.bin_edges();

    if (values.empty() || edges.size() < 2) return;

    auto* hist = new QwtPlotHistogram();
    QVector<QwtIntervalSample> samples;
    samples.reserve(static_cast<int>(values.size()));
    for (size_t i = 0; i < values.size() && i + 1 < edges.size(); ++i) {
        samples += QwtIntervalSample(values[i], edges[i], edges[i + 1]);
    }
    hist->setSamples(samples);

    QColor faceColor = toQColor(h.face_color());
    hist->setBrush(QBrush(faceColor));
    QColor edgeColor = toQColor(h.edge_color());
    hist->setPen(QPen(edgeColor));
    hist->setStyle(QwtPlotHistogram::Columns);

    hist->attach(plot_);
}

void QwtPlotVisitor::visit(matplot::function_line& fl) {
    auto fn = fl.fn();
    if (!fn) return;

    auto tRange = fl.t_range();
    size_t n = fl.mesh_density() > 0 ? fl.mesh_density() : 200;
    double tMin = tRange[0];
    double tMax = tRange[1];
    double dt = (tMax - tMin) / static_cast<double>(n);

    QVector<double> xData(static_cast<int>(n + 1));
    QVector<double> yData(static_cast<int>(n + 1));
    for (size_t i = 0; i <= n; ++i) {
        double t = tMin + static_cast<double>(i) * dt;
        xData[static_cast<int>(i)] = t;
        yData[static_cast<int>(i)] = fn(t);
    }

    auto* curve = new QwtPlotCurve();
    curve->setSamples(xData, yData);

    auto color = toQColor(fl.color());
    float lw = fl.line_width();
    auto penStyle = toPenStyle(fl.line_spec().line_style());
    if (penStyle != Qt::NoPen) {
        curve->setPen(QPen(color, lw, penStyle));
        curve->setStyle(QwtPlotCurve::Lines);
    } else {
        curve->setStyle(QwtPlotCurve::NoCurve);
    }

    curve->attach(plot_);
}

void QwtPlotVisitor::visit(matplot::stair& s) {
    auto& x = s.x_data();
    auto& y = s.y_data();

    if (y.empty()) return;

    auto* curve = new QwtPlotCurve();
    if (x.empty()) {
        std::vector<double> implicitX(y.size());
        for (size_t i = 0; i < y.size(); ++i) implicitX[i] = static_cast<double>(i + 1);
        curve->setSamples(implicitX.data(), y.data(), static_cast<int>(y.size()));
    } else {
        curve->setSamples(x.data(), y.data(), static_cast<int>(std::min(x.size(), y.size())));
    }

    auto color = toQColor(s.color());
    float lw = s.line_width();
    auto penStyle = toPenStyle(s.line_spec().line_style());
    if (penStyle != Qt::NoPen) {
        curve->setPen(QPen(color, lw, penStyle));
        curve->setStyle(QwtPlotCurve::Steps);
    } else {
        curve->setStyle(QwtPlotCurve::NoCurve);
    }

    curve->attach(plot_);
}

void QwtPlotVisitor::visit(class matplot::surface& s) {
    if (!surface_) return;

    auto& X = s.X_data();
    auto& Y = s.Y_data();
    auto& Z = s.Z_data();
    if (Z.empty() || Z[0].empty()) return;

    // matplot Z_data is row-major Z[row][col]
    // Qwt3D loadFromData expects column-major data[col][row] — transpose
    int nRows = static_cast<int>(Z.size());
    int nCols = static_cast<int>(Z[0].size());
    bool hasXY = !X.empty() && !X[0].empty() && !Y.empty() && !Y[0].empty();

    std::vector<Qwt3D::Triple*> triples(nCols);
    std::vector<std::vector<Qwt3D::Triple>> tripleData(nCols);
    for (int c = 0; c < nCols; ++c) {
        tripleData[c].resize(nRows);
        for (int r = 0; r < nRows; ++r) {
            double xv = hasXY ? X[r][c] : static_cast<double>(c);
            double yv = hasXY ? Y[r][c] : static_cast<double>(r);
            tripleData[c][r] = Qwt3D::Triple(xv, yv, Z[r][c]);
        }
        triples[c] = tripleData[c].data();
    }

    surface_->loadFromData(triples.data(), nCols, nRows);

    // 读取 axes 的 colormap，转换为 Qwt3D 颜色表
    auto& cmap = s.parent()->colormap();
    Qwt3D::ColorVector cv(cmap.size());
    for (size_t i = 0; i < cmap.size(); ++i) {
        auto& row = cmap[i];
        if (row.size() >= 4)  // ARGB: {a, r, g, b}
            cv[i] = Qwt3D::RGBA(row[1], row[2], row[3], 1.0 - row[0]);
        else                  // RGB: {r, g, b}
            cv[i] = Qwt3D::RGBA(row[0], row[1], row[2], 1.0);
    }
    const auto hull = surface_->hull();

    auto& C = s.C_data();
    if (!C.empty() && static_cast<int>(C.size()) == nRows
                   && static_cast<int>(C[0].size()) == nCols) {
        double cMin = C[0][0], cMax = C[0][0];
        for (auto& row : C)
            for (double v : row)
                cMin = std::min(cMin, v), cMax = std::max(cMax, v);
        surface_->setDataColor(new CDataColor(C,
            hull.minVertex.x, hull.maxVertex.x,
            hull.minVertex.y, hull.maxVertex.y,
            cMin, cMax, cv));
        surface_->legend()->setLimits(cMin, cMax);
        surface_->showColorLegend(true);
    } else {
        // 无 C_data：用 colormap 替换默认 StandardColor
        auto* sc = new Qwt3D::StandardColor(surface_, static_cast<unsigned>(cv.size()));
        sc->setColorVector(cv);
        surface_->setDataColor(sc);
        // 设置颜色范围
        {
            const auto hull = surface_->hull();
            double zMin = hull.minVertex.z, zMax = hull.maxVertex.z;
            if(zMin < zMax)
                surface_->legend()->setLimits(zMin, zMax);
        }
    }
    // 设置网格线宽
    // surface_->setMeshLineWidth(s.line_width());

    surface_->setFloorStyle(Qwt3D::NOFLOOR);
    // 设置坐标轴的显示样式
    surface_->setCoordinateStyle(Qwt3D::FRAME);
    if(s.palette_map_at_surface())
    {
        surface_->setMeshLineWidth(1);
        surface_->setPlotStyle(Qwt3D::FILLEDMESH); // 曲面 + 网格
    } else{
        surface_->setMeshLineWidth(2);
        Qwt3D::PLOTSTYLE plotStyle;
        if (s.hidden_3d()) {
            plotStyle = Qwt3D::HIDDENLINE;  // 网格 + 考虑遮挡关系（隐藏被遮挡的线）
        } else {
            plotStyle = Qwt3D::WIREFRAME;   // 网格 + 不考虑遮挡关系（透视所有线）
        }
        // 设置自定义网格线样式
        surface_->setPlotStyle(ColoredSurfaceEnrichment(plotStyle));
        // surface_->setPlotStyle(plotStyle);
    }

    // 均衡三个坐标轴长度：以平均范围为基准，缩放因子折中
    double xR = hull.maxVertex.x - hull.minVertex.x;
    double yR = hull.maxVertex.y - hull.minVertex.y;
    double zR = hull.maxVertex.z - hull.minVertex.z;
    // 保证对角线能完整显示在视图中
    double R = sqrt((xR * xR + yR * yR + zR * zR)/3);
    if (xR > 0 && yR > 0 && zR > 0) {
        surface_->setScale(R / xR, R / yR, R / zR);
    }
    // 设置光照参数（不知道为什么效果不明显）
    if(s.palette_map_at_surface() && s.lighting())
    {
        unsigned int light = 0;
        surface_->enableLighting(true);
        surface_->illuminate(light);
        if(s.primary() > 0)
            surface_->setLightComponent(GL_DIFFUSE, s.primary(), light);
        if(s.specular() > 0)
            surface_->setLightComponent(GL_SPECULAR, s.specular(), light);
    }
    surface_->setTitle(QString::fromStdString(s.display_name()));
    surface_->updateData();
    surface_->update();
}


AST_NAMESPACE_END
