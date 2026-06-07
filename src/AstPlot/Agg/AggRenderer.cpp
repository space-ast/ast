#include "AggRenderer.hpp"

#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "agg/agg_conv_dash.h"

#include "path_converters.h"

AST_NAMESPACE_BEGIN


// ========================================================================
// BMP 输出 — 标准 bottom-up 32-bit BMP
// ========================================================================
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct BmpFileHeader {
    unsigned short bfType;      // 'BM'
    unsigned int   bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int   bfOffBits;
}
#ifndef _MSC_VER
__attribute__((packed))
#endif
;

struct BmpInfoHeader {
    unsigned int   biSize;
    int            biWidth;
    int            biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int   biCompression;
    unsigned int   biSizeImage;
    int            biXPelsPerMeter;
    int            biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
}
#ifndef _MSC_VER
__attribute__((packed))
#endif
;
#ifdef _MSC_VER
#pragma pack(pop)
#endif

// ========================================================================
// AggRenderer
// ========================================================================
AggRenderer::AggRenderer(unsigned int w, unsigned int h, double dpi)
    : width_(w), height_(h), dpi_(dpi), pixBuffer_(nullptr)
{
    if (width_ < 1 || height_ < 1)
        throw std::invalid_argument("Invalid renderer dimensions");
    if (dpi_ <= 0.0)
        throw std::invalid_argument("DPI must be positive");

    size_t numBytes = (size_t)width_ * height_ * 4;
    pixBuffer_ = new agg::int8u[numBytes];
    rbuf_.attach(pixBuffer_, width_, height_, width_ * 4);
    pixFmt_.attach(rbuf_);
    renBase_.attach(pixFmt_);
    renBase_.clear(agg::rgba(1.0, 1.0, 1.0, 1.0));
    renAA_.attach(renBase_);
}

AggRenderer::~AggRenderer() {
    delete[] pixBuffer_;
}

void AggRenderer::clear(const agg::rgba& bg) {
    renBase_.clear(bg);
}

// ========================================================================
// draw_line — 完整复制 matplotlib RendererAgg::draw_path 管线
// ========================================================================
void AggRenderer::draw_line(const double* x, const double* y, size_t n,
                             const LineStyle& style, const agg::trans_affine& trans)
{
    if (n < 2 || style.linewidth <= 0.0) return;

    // 1. 构建 path_storage
    agg::path_storage path;
    path.move_to(x[0], y[0]);
    for (size_t i = 1; i < n; ++i)
        path.line_to(x[i], y[i]);

    draw_path(path, style, trans, false);
}

void AggRenderer::draw_path(agg::path_storage& path, const LineStyle& style,
                             const agg::trans_affine& trans, bool hasFace)
{
    // 清除光栅化器
    ras_.reset();
    ras_.clip_box(0, 0, width_, height_);
    ras_.gamma(agg::gamma_none());

    double lw_px = style.linewidth_px(dpi_);

    // conv_transform 需要非 const 引用，做可变拷贝
    agg::trans_affine mut_trans = trans;

    // 2. 变换管线 — 复制 matplotlib:
    //    conv_transform → PathNanRemover → PathClipper → PathSnapper
    //    → PathSimplifier → conv_curve → Sketch → conv_stroke
    //    用 typedef 链避免 MSVC auto 推导问题
    typedef agg::conv_transform<agg::path_storage>                   TP;
    typedef PathNanRemover<TP>                                       NR;
    typedef PathClipper<NR>                                          PC;
    typedef PathSnapper<PC>                                          PS;
    typedef PathSimplifier<PS>                                       PSim;
    typedef agg::conv_curve<PSim>                                    CV;
    typedef Sketch<CV>                                               SK;
    typedef agg::conv_dash<SK>                                       DASH;
    typedef agg::conv_stroke<SK>                                     STROKE_S;
    typedef agg::conv_stroke<DASH>                                   STROKE_D;

    TP     tpath(path, mut_trans);
    NR     nan_removed(tpath, true, false);
    bool   do_clip = !hasFace;
    PC     clipped(nan_removed, do_clip, (double)width_, (double)height_);
    PS     snapped(clipped, style.snap, path.total_vertices(), lw_px);
    PSim   simplified(snapped, style.simplify && do_clip, style.simplify_threshold);
    CV     curve(simplified);
    SK     sketch(curve, style.sketch_scale, style.sketch_length, style.sketch_randomness);

    // 3. 描边
    if (style.is_dashed()) {
        double ppf = dpi_ / 72.0;  // points → pixels
        DASH dash(sketch);
        for (size_t i = 0; i + 1 < style.dash_pattern.size(); i += 2)
            dash.add_dash(style.dash_pattern[i] * ppf, style.dash_pattern[i + 1] * ppf);
        dash.dash_start(style.dash_offset * ppf);

        STROKE_D stroke(dash);
        stroke.width(lw_px);
        stroke.line_cap(style.cap);
        stroke.line_join(style.join);
        stroke.miter_limit(lw_px);
        ras_.add_path(stroke);
    } else {
        STROKE_S stroke(sketch);
        stroke.width(lw_px);
        stroke.line_cap(style.cap);
        stroke.line_join(style.join);
        stroke.miter_limit(lw_px);
        ras_.add_path(stroke);
    }

    // 4. 光栅化 (antialiased)
    renAA_.color(style.color);
    agg::render_scanlines(ras_, sl_, renAA_);
}

// ========================================================================
// draw_filled_rect — 填充矩形 (无描边), 用于 axes 背景
// ========================================================================
void AggRenderer::draw_filled_rect(double x1, double y1, double x2, double y2,
                                    const agg::rgba& fill_color,
                                    const agg::trans_affine& trans)
{
    agg::path_storage rect;
    rect.move_to(x1, y1);
    rect.line_to(x2, y1);
    rect.line_to(x2, y2);
    rect.line_to(x1, y2);
    rect.close_polygon();

    // 变换
    agg::trans_affine mut_trans = trans;
    typedef agg::conv_transform<agg::path_storage, agg::trans_affine> CT;
    CT tpath(rect, mut_trans);

    ras_.reset();
    ras_.clip_box(0, 0, width_, height_);
    ras_.gamma(agg::gamma_none());
    ras_.add_path(tpath);

    renAA_.color(fill_color);
    agg::render_scanlines(ras_, sl_, renAA_);
}

// ========================================================================
// draw_text — 用 gsv_text 渲染矢量文字
// ========================================================================
void AggRenderer::draw_text(const char* text, double x, double y,
                             double size_pt, const agg::rgba& color)
{
    agg::gsv_text txt;
    txt.size(size_pt);
    txt.flip(true);                // BMP bottom-up → 文字需要 flip
    txt.start_point(x, y);
    txt.text(text);

    agg::conv_stroke<agg::gsv_text> stroke(txt);
    stroke.width(size_pt / 10.0);  // 笔画宽度 ≈ 字号的 1/10
    stroke.line_cap(agg::round_cap);
    stroke.line_join(agg::round_join);

    ras_.reset();
    ras_.add_path(stroke);
    renAA_.color(color);
    agg::render_scanlines(ras_, sl_, renAA_);
}

// ========================================================================
// BMP 输出
// ========================================================================
void AggRenderer::save_bmp(const char* filename) const {
    FILE* fp = fopen(filename, "wb");
    if (!fp) { fprintf(stderr, "Cannot create: %s\n", filename); return; }

    BmpFileHeader fh;
    fh.bfType = 0x4D42;
    fh.bfSize = 54 + width_ * height_ * 4;
    fh.bfReserved1 = fh.bfReserved2 = 0;
    fh.bfOffBits = 54;

    BmpInfoHeader ih;
    memset(&ih, 0, sizeof(ih));
    ih.biSize = 40;
    ih.biWidth = (int)width_;
    ih.biHeight = (int)height_;
    ih.biPlanes = 1;
    ih.biBitCount = 32;
    ih.biCompression = 0;

    fwrite(&fh, sizeof(fh), 1, fp);
    fwrite(&ih, sizeof(ih), 1, fp);

    // Bottom-up: 从最后一行到第一行, RGBA → BGRA
    for (int y = (int)height_ - 1; y >= 0; --y) {
        const unsigned char* row = pixBuffer_ + y * width_ * 4;
        for (unsigned x = 0; x < width_; ++x) {
            fputc(row[x * 4 + 2], fp);  // B
            fputc(row[x * 4 + 1], fp);  // G
            fputc(row[x * 4 + 0], fp);  // R
            fputc(row[x * 4 + 3], fp);  // A
        }
    }
    fclose(fp);
    printf("BMP saved: %s (%ux%u)\n", filename, width_, height_);
}


AST_NAMESPACE_END

