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

#include <matplot/axes_objects/function_line.h>
#include <matplot/axes_objects/histogram.h>
#include <matplot/axes_objects/line.h>
#include <matplot/axes_objects/stair.h>
#include <matplot/core/line_spec.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_histogram.h>
#include <qwt_symbol.h>
#include <qwt_series_data.h>

#include <QPen>
#include <QColor>

AST_NAMESPACE_BEGIN

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
            symbol->setBrush(QBrush(faceColor));
        }
        curve->setSymbol(symbol);
    }

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

AST_NAMESPACE_END
