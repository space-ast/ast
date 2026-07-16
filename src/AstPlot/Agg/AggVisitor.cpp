#include "AggVisitor.hpp"

#if defined(AST_WITH_AGG) && defined(AST_WITH_MATPLOT)

A_SUPPRESS_WARNINGS_BEGIN
#include <matplot/core/axes_type.h>
#include <matplot/core/axis_type.h>
#include <matplot/core/figure_type.h>
#include <matplot/axes_objects/line.h>
#include <matplot/util/colors.h>
A_SUPPRESS_WARNINGS_END

#include "AggRenderer.hpp"
#include "LineStyle.hpp"

#include "AstUtil/Logger.hpp"


#define AST_DEBUG_AGG_VISITOR

AST_NAMESPACE_BEGIN

// ---- 工具函数 ----
static inline agg::rgba to_agg_color(const matplot::color_array& c) {
    return agg::rgba(c[1], c[2], c[3], 1.0 - c[0]);
}

static int marker_shape(enum matplot::line_spec::marker_style ms) {
    switch (ms) {
    case matplot::line_spec::marker_style::circle:                   return 0;
    case matplot::line_spec::marker_style::square:                   return 1;
    case matplot::line_spec::marker_style::diamond:                  return 2;
    case matplot::line_spec::marker_style::upward_pointing_triangle:  return 3;
    case matplot::line_spec::marker_style::downward_pointing_triangle:return 4;
    case matplot::line_spec::marker_style::plus_sign:                 return 5;
    case matplot::line_spec::marker_style::cross:                     return 6;
    case matplot::line_spec::marker_style::asterisk:                  return 7;
    case matplot::line_spec::marker_style::point:                     return 8;
    default: return -1;
    }
}

static agg::trans_affine apply_y_flip(const agg::trans_affine& t, double fig_h) {
    agg::trans_affine r = t;
    r *= agg::trans_affine_scaling(1.0, -1.0);
    r *= agg::trans_affine_translation(0.0, fig_h);
    return r;
}



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

    s.color = to_agg_color(l.color());

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
    //double ax_y  = pos[1] * fig_h_;
    double ax_w  = pos[2] * fig_w_;
    double ax_h  = pos[3] * fig_h_;

    // ---- 全图变换 (data → figure pixels, Y-up → Y-down) ----
    auto trans = apply_y_flip(getTransform(), fig_h_);

    // ---- 1. 背景填充 (白色, 覆盖整个 axes 区域) ----
    auto bg = axes_->color();
    agg::rgba bg_color = to_agg_color(bg);
    renderer_.draw_filled_rect(xmin, ymin, xmax, ymax, bg_color, trans);

    // ---- 2. 网格线 (在 spines 之前, 匹配 matplotlib zorder) ----
    auto xticks = auto_ticks(xmin, xmax);
    auto yticks = auto_ticks(ymin, ymax);

    if (axes_->x_grid() || axes_->y_grid()) {
        LineStyle gridStyle;
        gridStyle.linewidth = 0.5f;
        auto gc = axes_->grid_color();
        #ifdef AST_DEBUG_AGG_VISITOR
        printf("[DEBUG] grid_color=[%.2f,%.2f,%.2f,%.2f]\n",
               gc[1], gc[2], gc[3], (1.0 - gc[0]));
        printf("[DEBUG] grid_alpha=%.2f\n", axes_->grid_alpha());
        #endif
        gridStyle.color = to_agg_color(gc);
        gridStyle.cap = agg::butt_cap;
        gridStyle.antialiased = true;
        gridStyle.simplify = false;

        if (axes_->x_grid()) {
            for (auto tx : xticks) {
                double gx[2] = {tx, tx}, gy[2] = {ymin, ymax};
                renderer_.draw_line(gx, gy, 2, gridStyle, trans);
            }
        }
        if (axes_->y_grid()) {
            for (auto ty : yticks) {
                double gx[2] = {xmin, xmax}, gy[2] = {ty, ty};
                renderer_.draw_line(gx, gy, 2, gridStyle, trans);
            }
        }
    }

    // ---- 3. 四边 spines (黑色, 0.8pt, square_cap) ----
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
    // 刻度参数 (匹配 matplotlib 默认)
    double tick_len_pt = 3.5;                         // 刻度线长 (points)
    double tick_len_px = tick_len_pt * renderer_.dpi() / 72.0;    // → pixels
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

    // ---- 4. 标题、轴标签 ----
    #ifdef AST_DEBUG_AGG_VISITOR
    printf("[DEBUG] title='%s' xlabel='%s' ylabel='%s'\n",
           axes_->title().c_str(), axes_->xlabel().c_str(), axes_->ylabel().c_str());
    #endif
    agg::rgba label_color(0.0, 0.0, 0.0, 1.0);

    // 计算 axes 在像素空间的四边
    double ax_px_left = ax_x;
    double ax_px_right = ax_x + ax_w;
    //{ double d = xmin; trans.transform(&d, &axes_pixel_bottom); }
    double ax_px_top = ymax;
    { double d = xmin; trans.transform(&d, &ax_px_top); }

    double axes_center_x = (ax_px_left + ax_px_right) * 0.5;
    double axes_center_y = (ax_px_top + axes_pixel_bottom) * 0.5;

    #ifdef AST_DEBUG_AGG_VISITOR

    printf("[DEBUG] axes_center_x=%.2f axes_center_y=%.2f\n", axes_center_x, axes_center_y);
    printf("ax_px_top=%.2f\n", ax_px_top);
    printf("ax_px_left=%.2f\n", ax_px_left);
    printf("ax_px_right=%.2f\n", ax_px_right);
    printf("axes_pixel_bottom=%.2f\n", axes_pixel_bottom);
    #endif



    // Title — 顶部居中
    {
        auto title = axes_->title();
        if (!title.empty()) {
            double title_y = ax_px_top - label_pad_px;
            int len = (int)title.size();
            double tw = len * 9.0 * 0.55;
            renderer_.draw_text(title.c_str(),
                axes_center_x - tw * 0.5, title_y,
                10.0, label_color);
        }
    }

    // X label — 底部居中
    {
        auto xl = axes_->xlabel();
        if (!xl.empty()) {
            double xl_y = axes_pixel_bottom + label_pad_px + 30.0;
            int len = (int)xl.size();
            double tw = len * 10.0 * 0.6;
            renderer_.draw_text(xl.c_str(),
                axes_center_x - tw * 0.5, xl_y,
                10.0, label_color);
        }
    }

    // Y label — 左侧居中, 逆时针旋转90°
    {
        auto yl = axes_->ylabel();
        if (!yl.empty()) {
            int len = (int)yl.size();
            double tw = len * 10.0 * 0.6;    // 原始文字宽度
            double th = 10.0;                 // 文字高度 ≈ 字号
            // 旋转后: baseline 向上, X方向宽=th, Y方向高=tw
            double yl_x = ax_px_left - label_pad_px - th - 30.0;
            double yl_y = axes_center_y + tw * 0.5;
            renderer_.draw_text(yl.c_str(),
                yl_x, yl_y,
                10.0, label_color, 90.0);
        }
    }

    // ---- 5. 图例 ----
    auto leg = axes_->legend();
    if (leg && leg->visible() && !leg->strings().empty()) {
        double leg_font = leg->font_size();  // pt
        double line_len = 30.0;              // 图例线段长度 (px)
        double gap = 6.0;                    // 线段与文字间距
        double item_h = leg_font + 6.0;      // 每项高度
        double pad = 8.0;                    // 内边距

        // 计算图例框尺寸
        auto& strs = leg->strings();
        double max_tw = 0;
        for (auto& s : strs) {
            double tw = s.size() * leg_font * 0.6;
            if (tw > max_tw) max_tw = tw;
        }
        double box_w = line_len + gap + max_tw + pad * 2;
        double box_h = item_h * strs.size() + pad * 2;

        // 位置: 右上角 (inside)
        double leg_x = ax_px_right - box_w - 5.0;
        double leg_y = ax_px_top + 5.0;

        // 背景 (半透明白色)
        renderer_.draw_filled_rect(leg_x, leg_y, leg_x + box_w, leg_y + box_h,
                                    agg::rgba(1.0, 1.0, 1.0, 0.85), agg::trans_affine());

        // 边框
        if (leg->box()) {
            LineStyle boxStyle;
            boxStyle.linewidth = 0.5f;
            boxStyle.color = agg::rgba(0.5, 0.5, 0.5, 1.0);
            double b[4][2] = {
                {leg_x, leg_y}, {leg_x+box_w, leg_y},
                {leg_x+box_w, leg_y+box_h}, {leg_x, leg_y+box_h}
            };
            for (int i = 0; i < 4; ++i) {
                double xs[2] = {b[i][0], b[(i+1)%4][0]};
                double ys[2] = {b[i][1], b[(i+1)%4][1]};
                renderer_.draw_line(xs, ys, 2, boxStyle, agg::trans_affine());
            }
        }

        // 逐项绘制
        auto tc = leg->text_color();
        agg::rgba txt_color = to_agg_color(tc);

        for (size_t i = 0; i < axes_->children().size(); ++i) {
            auto child = axes_->children()[i];
            auto lptr = std::dynamic_pointer_cast<matplot::line>(child);
            
            if(!lptr) continue;
            LineStyle ls = from_line(*lptr);

            double iy = leg_y + pad + item_h * i + leg_font * 0.8;
            // 线段样本
            double lx = leg_x + pad;
            double mid_x = lx + line_len * 0.5;
            double xs[2] = {lx, lx + line_len};
            double ys[2] = {iy, iy};
            renderer_.draw_line(xs, ys, 2, ls, agg::trans_affine());

            // 标记样本
            int shape = marker_shape(lptr->marker_style());
            if (shape < 0) shape = 0;  // fallback: circle
            auto mc  = lptr->marker_color();
            auto mfc = lptr->marker_face_color();
            double msz = lptr->marker_size() * 0.7;
            double mew = lptr->line_width() * 0.5f; // marker edge width

            agg::rgba edge_c = to_agg_color(mc);
            agg::rgba face_c = lptr->marker_face()
                ? to_agg_color(mfc) : agg::rgba(0,0,0,0);
            renderer_.draw_marker(shape, mid_x, iy, msz,
                                  edge_c, face_c, mew);

            std::string name = lptr->display_name();
            if(name.empty())
            {
                if(strs.size() > i) name = strs[i];
                if(name.empty()) name = "data" + std::to_string(i + 1);
            }

            // 文字
            renderer_.draw_text(name.c_str(),
                lx + line_len + gap, iy + leg_font * 0.35,
                leg_font, txt_color);
        }
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

    // 坐标变换 + Y-flip
    auto trans = apply_y_flip(getTransform(), fig_h_);

    renderer_.draw_line(px, y.data(), y.size(), style, trans);

    // ---- 绘制标记 ----
    auto ms = l.marker_style();
    if (ms != matplot::line_spec::marker_style::none && l.marker_size() > 0) {
        // 映射 matplot marker_style → shape id
        int shape = marker_shape(ms);

        if (shape >= 0) {
            auto mc  = l.marker_color();
            auto mfc = l.marker_face_color();
            double msz = l.marker_size();
            double mew = l.line_width() * 0.5f; // marker edge width

            agg::rgba edge_c = to_agg_color(mc);
            agg::rgba face_c = l.marker_face()
                ? to_agg_color(mfc) : agg::rgba(0,0,0,0);

            size_t n = y.size();
            for (size_t i = 0; i < n; ++i) {
                double px_d = px[i], py_d = y[i];
                trans.transform(&px_d, &py_d);
                renderer_.draw_marker(shape, px_d, py_d, msz, edge_c, face_c, mew);
            }
        }
    }
}



AST_NAMESPACE_END


#endif  // AST_WITH_AGG && AST_WITH_MATPLOT
