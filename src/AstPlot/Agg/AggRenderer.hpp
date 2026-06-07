#pragma once

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

#include "LineStyle.hpp"

/// Agg 核心渲染器 — 仿 matplotlib RendererAgg
class AggRenderer {
public:
    // 像素格式: 预乘 Alpha RGBA
    typedef agg::pixfmt_rgba32_pre pixfmt_type;
    typedef agg::renderer_base<pixfmt_type> renderer_base_type;
    typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_aa_type;
    typedef agg::rasterizer_scanline_aa<> rasterizer_type;
    typedef agg::scanline_p8 scanline_type;

    AggRenderer(unsigned int width, unsigned int height, double dpi = 96.0);
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
