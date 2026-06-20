#pragma once

#include <vector>

#if defined(AST_WITH_AGG) 

#include "agg/agg_color_rgba.h"
#include "agg/agg_math_stroke.h"
#include "path_converters.h"  // e_snap_mode


AST_NAMESPACE_BEGIN


/// 线型样式 — 默认值匹配 matplotlib rcParams
struct LineStyle {
    double linewidth = 1.5;          // pt
    agg::rgba color{0.122, 0.467, 0.706, 1.0};  // C0 blue
    agg::line_cap_e cap = agg::square_cap;       // "projecting"
    agg::line_join_e join = agg::round_join;
    bool antialiased = true;
    e_snap_mode snap = SNAP_AUTO;

    double  dash_offset = 0.0;
    std::vector<double> dash_pattern;   // 空 = 实线, 单位 pt (已按 lw 缩放)

    bool simplify = true;
    double simplify_threshold = 0.111111; // M_SQRT2/9 ≈ matplot default

    double sketch_scale = 0.0;
    double sketch_length = 0.0;
    double sketch_randomness = 0.0;

    bool is_dashed() const { return !dash_pattern.empty(); }
    double linewidth_px(double dpi) const { return linewidth * dpi / 72.0; }
};


AST_NAMESPACE_END

#endif // AST_WITH_AGG 