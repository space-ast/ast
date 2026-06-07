#include "AggVisitor.hpp"

#if defined(AST_WITH_AGG) && defined(AST_WITH_MATPLOT)

#include <matplot/core/axes_type.h>
#include <matplot/core/axis_type.h>
#include <matplot/core/figure_type.h>
#include <matplot/axes_objects/line.h>
#include <matplot/util/colors.h>

#include "AggRenderer.hpp"
#include "LineStyle.hpp"

AggVisitor::AggVisitor(AggRenderer& renderer, double fig_w, double fig_h)
    : renderer_(renderer), fig_w_(fig_w), fig_h_(fig_h) {}

void AggVisitor::set_axes(const matplot::axes_type& ax) {
    axes_ = &ax;
}

// ========================================================================
// 坐标变换: data → figure pixels (Y-up)
// ========================================================================
agg::trans_affine AggVisitor::make_transform() const {
    if (!axes_) return agg::trans_affine();

    std::array<double, 2> xlim = axes_->x_axis().limits();
    std::array<double, 2> ylim = axes_->y_axis().limits();
    auto pos = axes_->position();  // {left, bottom, width, height} in [0,1]

    double xmin = xlim[0], xmax = xlim[1];
    double ymin = ylim[0], ymax = ylim[1];

    double ax_x = pos[0] * fig_w_;
    double ax_y = pos[1] * fig_h_;
    double ax_w = pos[2] * fig_w_;
    double ax_h = pos[3] * fig_h_;

    double dx = xmax - xmin;
    double dy = ymax - ymin;
    if (dx <= 0.0) dx = 1.0;
    if (dy <= 0.0) dy = 1.0;

    double sx = ax_w / dx;
    double sy = ax_h / dy;
    double tx = ax_x - xmin * sx;
    double ty = ax_y - ymin * sy;

    return agg::trans_affine(sx, 0.0, 0.0, sy, tx, ty);
}

// ========================================================================
// visit(matplot::line&)
// ========================================================================
void AggVisitor::visit(matplot::line& l) {
    if (!l.visible()) return;

    auto x = l.x_data();
    auto y = l.y_data();
    if (y.empty()) return;

    // 若未提供 x, 自动生成 0..n-1
    std::vector<double> gen_x;
    const double* px;
    if (x.size() != y.size()) {
        gen_x.resize(y.size());
        for (size_t i = 0; i < y.size(); ++i) gen_x[i] = (double)i;
        px = gen_x.data();
    } else {
        px = x.data();
    }

    // 构建 LineStyle
    LineStyle style = matplot_line_bridge::from_line(l);

    // 坐标变换
    auto trans = make_transform();

    // Y-up → Y-down (matplotlib 做法)
    trans *= agg::trans_affine_scaling(1.0, -1.0);
    trans *= agg::trans_affine_translation(0.0, (double)fig_h_);

    renderer_.draw_line(px, y.data(), y.size(), style, trans);
}

// matplot_line_bridge::from_line — 全局作用域 (声明在 LineStyle.hpp 中未包裹 namespace)
LineStyle matplot_line_bridge::from_line(const matplot::line& l) {
    LineStyle s;

    s.linewidth = l.line_width();
    if (s.linewidth <= 0.0f) s.linewidth = 1.5f;

    auto c = l.color();
    s.color = agg::rgba(c[1], c[2], c[3], 1.0 - c[0]);

    auto ls = l.line_spec().line_style();
    switch (ls) {
    case matplot::line_spec::line_style::solid_line:
        s.cap = agg::square_cap;
        break;
    case matplot::line_spec::line_style::dashed_line:
        s.dash_pattern = {3.7, 1.6};
        s.cap = agg::butt_cap;
        break;
    case matplot::line_spec::line_style::dotted_line:
        s.dash_pattern = {1.0, 1.65};
        s.cap = agg::butt_cap;
        break;
    case matplot::line_spec::line_style::dash_dot_line:
        s.dash_pattern = {6.4, 1.6, 1.0, 1.6};
        s.cap = agg::butt_cap;
        break;
    case matplot::line_spec::line_style::none:
        s.linewidth = 0.0;
        break;
    default:
        s.cap = agg::square_cap;
        break;
    }

    if (s.is_dashed()) {
        for (auto& v : s.dash_pattern) v *= s.linewidth;
    }
    s.join = agg::round_join;

    return s;
}

#endif  // AST_WITH_AGG && AST_WITH_MATPLOT
