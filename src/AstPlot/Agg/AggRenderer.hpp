#pragma once

#include "AstGlobal.h"

#if defined(AST_WITH_AGG)

#include "agg/agg_basics.h"
#include "agg/agg_rendering_buffer.h"
#include "agg/agg_rasterizer_scanline_aa.h"
#include "agg/agg_scanline_p.h"
#include "agg/agg_renderer_scanline.h"
#include "agg/agg_pixfmt_rgba.h"
#include "agg/agg_path_storage.h"
#include "agg/agg_conv_stroke.h"
#include "agg/agg_conv_transform.h"
#include "agg/agg_conv_curve.h"
#include "agg/agg_conv_dash.h"
#include "agg/agg_trans_affine.h"
#include "agg/agg_color_rgba.h"
#include "agg/agg_gsv_text.h"

#include "LineStyle.hpp"


AST_NAMESPACE_BEGIN

/// Agg 核心渲染器 — 仿 matplotlib RendererAgg
class AggRenderer {
public:
    // 像素格式: 普通 RGBA (非预乘)
    typedef agg::pixfmt_rgba32 pixfmt_type;
    typedef agg::renderer_base<pixfmt_type> renderer_base_type;
    typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_aa_type;
    typedef agg::rasterizer_scanline_aa<> rasterizer_type;
    typedef agg::scanline_p8 scanline_type;

    AggRenderer(unsigned int width, unsigned int height, double dpi = 96.0);
    AggRenderer(const AggRenderer&) = delete;
    AggRenderer& operator=(const AggRenderer&) = delete;
    ~AggRenderer();

    unsigned int width()  const { return width_; }
    unsigned int height() const { return height_; }
    double dpi()          const { return dpi_; }

    /// 清空画布
    void clear(const agg::rgba& bg = agg::rgba(1.0, 1.0, 1.0, 1.0));

    /// 绘制线条 — 完整复制 matplotlib draw_path + _draw_path 管线
    void draw_line(const double* x, const double* y, size_t n,
                   const LineStyle& style, const agg::trans_affine& trans);

    /// 绘制路径 (已变换，不含 Y-flip)
    void draw_path(agg::path_storage& path, const LineStyle& style,
                   const agg::trans_affine& trans, bool hasFace = false);

    /// 绘制填充矩形 (无描边) — 用于 axes 背景
    void draw_filled_rect(double x1, double y1, double x2, double y2,
                          const agg::rgba& fill_color,
                          const agg::trans_affine& trans);

    /// 绘制文字 (使用 gsv_text 矢量字体)
    /// @param text  文本字符串
    /// @param x, y  像素坐标 (Y-down, 已翻转后的坐标)
    /// @param size_pt 字号 (points)
    /// @param color 颜色
    void draw_text(const char* text, double x, double y, double size_pt,
                   const agg::rgba& color, double angle_deg = 0.0);

    /// 绘制标记 (circle, square, diamond, triangle, plus, cross, asterisk, point)
    /// @param shape 形状: 0=circle, 1=square, 2=diamond, 3=up_tri, 4=down_tri, 5=plus, 6=cross, 7=asterisk, 8=point
    /// @param cx, cy  像素坐标 (Y-down)
    /// @param size_pt  标记大小 (points)
    /// @param edge_color, fill_color  描边/填充颜色
    /// @param linewidth  描边线宽 (points), 0 表示不描边
    void draw_marker(int shape, double cx, double cy, double size_pt,
                     const agg::rgba& edge_color, const agg::rgba& fill_color,
                     double linewidth);

    /// 获取像素缓冲区
    const unsigned char* buffer() const { return pixBuffer_; }

    /// 保存为 BMP
    void save_bmp(const char* filename) const;

private:

    void draw_stroke_impl(/* 处理后的 path pipeline */);

    unsigned int width_, height_;
    double dpi_;

    agg::int8u* pixBuffer_;
    agg::rendering_buffer rbuf_;
    pixfmt_type pixFmt_;
    renderer_base_type renBase_;
    renderer_aa_type renAA_;
    rasterizer_type ras_;
    scanline_type sl_;
};


AST_NAMESPACE_END

#endif // AST_WITH_AGG && AST_WITH_MATPLOT