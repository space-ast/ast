#include "AggVisitor.hpp"

#if defined(AST_WITH_AGG) && defined(AST_WITH_MATPLOT)

#include <matplot/core/axes_type.h>
#include <matplot/core/axis_type.h>
#include <matplot/core/figure_type.h>
#include <matplot/axes_objects/line.h>
#include <matplot/util/colors.h>

#include "AggRenderer.hpp"
#include "LineStyle.hpp"

#include "AstUtil/Logger.hpp"


#define AST_DEBUG_AGG_VISITOR

AST_NAMESPACE_BEGIN



static void compute_limits(const matplot::axes_type& axes,
                           double& xmin, double& xmax,
                           double& ymin, double& ymax)
{
    // ---- X 轴 ----
    if (axes.x_axis().limits_mode_auto()) {
        bool first = true;
        for (auto& obj : axes.children()) {
            double oxmin = obj->xmin(), oxmax = obj->xmax();
            if (!std::isfinite(oxmin) || !std::isfinite(oxmax)) continue;
            if (oxmin == oxmax) { oxmin -= 1.0; oxmax += 1.0; }
            if (first) { xmin = oxmin; xmax = oxmax; first = false; }
            else {
                if (oxmin < xmin) xmin = oxmin;
                if (oxmax > xmax) xmax = oxmax;
            }
        }
        if (!first) {
            double xm = (xmax - xmin) * 0.05; if (xm <= 0) xm = 1.0;
            xmin -= xm; xmax += xm;
        }
    } else {
        auto xlim = axes.x_axis().limits();
        xmin = xlim[0]; xmax = xlim[1];
    }

    // ---- Y 轴 ----
    if (axes.y_axis().limits_mode_auto()) {
        bool first = true;
        for (auto& obj : axes.children()) {
            double oymin = obj->ymin(), oymax = obj->ymax();
            if (!std::isfinite(oymin) || !std::isfinite(oymax)) continue;
            if (oymin == oymax) { oymin -= 1.0; oymax += 1.0; }
            if (first) { ymin = oymin; ymax = oymax; first = false; }
            else {
                if (oymin < ymin) ymin = oymin;
                if (oymax > ymax) ymax = oymax;
            }
        }
        if (!first) {
            double ym = (ymax - ymin) * 0.05; if (ym <= 0) ym = 1.0;
            ymin -= ym; ymax += ym;
        }
    } else {
        auto ylim = axes.y_axis().limits();
        ymin = ylim[0]; ymax = ylim[1];
    }
}



AggVisitor::AggVisitor(AggRenderer& renderer, double fig_w, double fig_h)
    : renderer_(renderer)
    , fig_w_(fig_w)
    , fig_h_(fig_h) 
{
}


void AggVisitor::setAxes(const matplot::axes_type& axes)
{
    axes_ = &axes;
    compute_limits(axes, xmin_, xmax_, ymin_, ymax_);
    trans_ = makeTransform(xmin_, xmax_, ymin_, ymax_);
}



// ========================================================================
// 自动刻度生成 (当 matplot 未提供合理刻度时使用)
// ========================================================================
static std::vector<double> auto_ticks(double lo, double hi) {
    std::vector<double> result;
    if (hi <= lo) return result;

    double range = hi - lo;
    double rough = range / 6.0;
    double power = std::pow(10.0, std::floor(std::log10(rough)));
    double residual = rough / power;
    double step;
    if      (residual < 1.5)  step = 1.0 * power;
    else if (residual < 3.5)  step = 2.0 * power;
    else if (residual < 7.5)  step = 5.0 * power;
    else                      step = 10.0 * power;

    double start = std::ceil(lo / step) * step;
    for (double v = start; v <= hi; v += step)
        result.push_back(v);
    return result;
}



static LineStyle from_line(const matplot::line& l) {
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


// ========================================================================
// 坐标变换: data → figure pixels (Y-up)
// ========================================================================
agg::trans_affine AggVisitor::makeTransform(double xmin, double xmax,
                                              double ymin, double ymax) const {
    if (!axes_) return agg::trans_affine();

    auto pos = axes_->position();
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
// draw_axes — 绘制坐标轴背景 + spines
// ========================================================================
void AggVisitor::drawAxes() {
    if (!axes_) return;

    auto pos  = axes_->position();  // {left, bottom, width, height} in [0,1]

    double xmin = xmin_, xmax = xmax_, ymin = ymin_, ymax = ymax_;

    #ifdef AST_DEBUG_AGG_VISITOR
    printf("[DEBUG] auto_x=%d auto_y=%d  xlim=[%.2f, %.2f] ylim=[%.2f, %.2f] pos=[%.2f,%.2f,%.2f,%.2f]\n",
           axes_->x_axis().limits_mode_auto(), axes_->y_axis().limits_mode_auto(),
           xmin, xmax, ymin, ymax, pos[0], pos[1], pos[2], pos[3]);
    #endif

    double ax_x  = pos[0] * fig_w_;
    double ax_y  = pos[1] * fig_h_;
    double ax_w  = pos[2] * fig_w_;
    double ax_h  = pos[3] * fig_h_;

    // ---- 全图变换 (data → figure pixels, Y-up → Y-down) ----
    auto trans = getTransform();
    trans *= agg::trans_affine_scaling(1.0, -1.0);
    trans *= agg::trans_affine_translation(0.0, (double)fig_h_);

    // ---- 1. 背景填充 (白色, 覆盖整个 axes 区域) ----
    auto bg = axes_->color();
    agg::rgba bg_color(bg[1], bg[2], bg[3], 1.0 - bg[0]);
    renderer_.draw_filled_rect(xmin, ymin, xmax, ymax, bg_color, trans);

    // ---- 2. 四边 spines (黑色, 0.8pt, square_cap) ----
    LineStyle spineStyle;
    spineStyle.linewidth = 0.8f;
    spineStyle.color = agg::rgba(0.0, 0.0, 0.0, 1.0);
    spineStyle.cap = agg::square_cap;
    spineStyle.join = agg::round_join;
    spineStyle.antialiased = true;
    spineStyle.simplify = false;  // 只有两点, 不需要简化

    auto drawLine = [&](double x1, double y1, double x2, double y2,
                          const LineStyle& style) {
        double xs[2] = {x1, x2};
        double ys[2] = {y1, y2};
        renderer_.draw_line(xs, ys, 2, style, trans);
    };

    // bottom spine:  (xmin, ymin) → (xmax, ymin)
    drawLine(xmin, ymin, xmax, ymin, spineStyle);
    // top spine:     (xmin, ymax) → (xmax, ymax)
    drawLine(xmin, ymax, xmax, ymax, spineStyle);
    // left spine:    (xmin, ymin) → (xmin, ymax)
    drawLine(xmin, ymin, xmin, ymax, spineStyle);
    // right spine:   (xmax, ymin) → (xmax, ymax)
    drawLine(xmax, ymin, xmax, ymax, spineStyle);

    // ---- 3. 刻度线 (matplotlib 默认: 3.5pt, 黑色, 方向 'out') ----
    double dx = xmax - xmin; if (dx <= 0.0) dx = 1.0;
    double dy = ymax - ymin; if (dy <= 0.0) dy = 1.0;
    double sx = ax_w / dx;  // data → pixels scale X
    double sy = ax_h / dy;  // data → pixels scale Y
    double dpi = 96.0;

    // 刻度参数 (匹配 matplotlib 默认)
    double tick_len_pt = 3.5;                         // 刻度线长 (points)
    double tick_len_px = tick_len_pt * dpi / 72.0;    // → pixels
    double tick_dx = (sx > 0) ? tick_len_px / sx : 0;  // → data coords X
    double tick_dy = (sy > 0) ? tick_len_px / sy : 0;  // → data coords Y

    LineStyle tickStyle;
    tickStyle.linewidth = 0.8f;
    tickStyle.color = agg::rgba(0.0, 0.0, 0.0, 1.0);
    tickStyle.cap = agg::butt_cap;
    tickStyle.simplify = false;

    // ---- 标签定位参数 ----
    double label_pad_px = 8.0;
    double axes_pixel_bottom = ymin;  // spine bottom 在 AGG 像素坐标中的 Y
    { double dummy = xmin; trans.transform(&dummy, &axes_pixel_bottom); }

    auto xticks = auto_ticks(xmin, xmax);
    for (auto tx : xticks) {
        drawLine(tx, ymin, tx, ymin + tick_dy, tickStyle);

        double tick_px = tx, dummy_y = ymin;
        trans.transform(&tick_px, &dummy_y);

        char buf[32];
        int len = snprintf(buf, sizeof(buf), "%g", tx);
        double text_w = len * 9.0 * 0.55;

        // 文字居中, 在 spine 下方 label_pad_px 处
        double label_x = tick_px - text_w * 0.5;
        double label_y = axes_pixel_bottom + label_pad_px + 10.0;

        renderer_.draw_text(buf, label_x, label_y, 9.0,
                            agg::rgba(0.0, 0.0, 0.0, 1.0));
    }

    // Y 轴刻度 + 标签 (靠右对齐, 在绘图区左侧)
    double axes_pixel_left = ax_x;

    auto yticks = auto_ticks(ymin, ymax);
    for (auto ty : yticks) {
        drawLine(xmin, ty, xmin + tick_dx, ty, tickStyle);

        double dummy_x = xmin, tick_py = ty;
        trans.transform(&dummy_x, &tick_py);

        char buf[32];
        int len = snprintf(buf, sizeof(buf), "%g", ty);
        double text_w = len * 9.0 * 0.55;
        double text_h = 9.0 * 0.8;

        // 文字右端 = Y 轴左侧 label_pad_px 处
        double right_edge = axes_pixel_left - label_pad_px - 10;
        double label_x = right_edge - text_w;
        double label_y = tick_py + text_h * 0.35;

        renderer_.draw_text(buf, label_x, label_y, 9.0,
                            agg::rgba(0.0, 0.0, 0.0, 1.0));
    }
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
    LineStyle style = from_line(l);

    // 坐标变换
    auto trans = getTransform();

    // Y-up → Y-down (matplotlib 做法)
    trans *= agg::trans_affine_scaling(1.0, -1.0);
    trans *= agg::trans_affine_translation(0.0, (double)fig_h_);

    renderer_.draw_line(px, y.data(), y.size(), style, trans);
}



AST_NAMESPACE_END


#endif  // AST_WITH_AGG && AST_WITH_MATPLOT
