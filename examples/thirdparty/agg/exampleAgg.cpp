/**
 * @file exampleAgg.cpp
 * @brief  Anti-Grain Geometry (AGG) 库使用示例
 *
 * 本示例演示 AGG 2D 渲染库的核心功能：
 *   - 渲染缓冲区 (rendering_buffer) 与像素格式 (pixfmt)
 *   - 反走样光栅化器 (rasterizer_scanline_aa)
 *   - 扫描线渲染 (scanline + renderer_scanline)
 *   - 路径存储 (path_storage) 与几何图形生成
 *   - 描边 (conv_stroke) 与变换 (conv_transform)
 *   - 贝塞尔曲线 (conv_curve)
 *   - 虚线 (conv_dash) 与等值线 (conv_contour)
 *   - 线性渐变与径向渐变填充
 *   - Gouraud 着色
 *   - 矢量文字渲染 (gsv_text)
 *   - 输出为 BMP 图像文件
 *
 * 编译: xmake b exampleAgg
 * 运行: xmake run exampleAgg
 * 输出: exampleAgg.bmp (可在任何图像查看器中打开)
 */

#ifdef AST_WITH_AGG

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

// AGG 核心头文件
#include "agg/agg_basics.h"
#include "agg/agg_rendering_buffer.h"
#include "agg/agg_rasterizer_scanline_aa.h"
#include "agg/agg_scanline_p.h"
#include "agg/agg_renderer_scanline.h"

// 像素格式
#include "agg/agg_pixfmt_rgba.h"

// 路径与几何
#include "agg/agg_path_storage.h"
#include "agg/agg_conv_stroke.h"
#include "agg/agg_conv_transform.h"
#include "agg/agg_conv_curve.h"
#include "agg/agg_conv_contour.h"
#include "agg/agg_conv_dash.h"
#include "agg/agg_ellipse.h"
#include "agg/agg_arc.h"
#include "agg/agg_bezier_arc.h"
#include "agg/agg_rounded_rect.h"

// 渐变与着色
#include "agg/agg_span_gradient.h"
#include "agg/agg_span_gouraud_rgba.h"
#include "agg/agg_span_allocator.h"
#include "agg/agg_span_interpolator_linear.h"

// 变换
#include "agg/agg_trans_affine.h"

// 文字
#include "agg/agg_gsv_text.h"

// 数学工具
#include "agg/agg_math.h"

// ============================================================================
// 工具: 将 RGBA 缓冲区保存为 32-bit BMP 文件 (BGRA, 无需任何第三方库)
// ============================================================================
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct BmpFileHeader {
    unsigned short bfType;      // 'BM' = 0x4D42
    unsigned int   bfSize;      // 文件总大小
    unsigned short bfReserved1; // 0
    unsigned short bfReserved2; // 0
    unsigned int   bfOffBits;   // 像素数据偏移 = 54
}
#ifndef _MSC_VER
__attribute__((packed))
#endif
;

struct BmpInfoHeader {
    unsigned int   biSize;          // 40
    int            biWidth;
    int            biHeight;
    unsigned short biPlanes;        // 1
    unsigned short biBitCount;      // 32
    unsigned int   biCompression;   // 0 = BI_RGB
    unsigned int   biSizeImage;     // 可为 0
    int            biXPelsPerMeter; // 0
    int            biYPelsPerMeter; // 0
    unsigned int   biClrUsed;       // 0
    unsigned int   biClrImportant;  // 0
}
#ifndef _MSC_VER
__attribute__((packed))
#endif
;
#ifdef _MSC_VER
#pragma pack(pop)
#endif

static void saveBmp(const char* filename,
                    const unsigned char* buffer,
                    unsigned width, unsigned height, int stride)
{
    FILE* fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Cannot create file: %s\n", filename);
        return;
    }

    BmpFileHeader fileHeader;
    fileHeader.bfType      = 0x4D42;  // 'BM' in little-endian
    fileHeader.bfSize      = 54 + width * height * 4;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits   = 54;

    BmpInfoHeader infoHeader;
    memset(&infoHeader, 0, sizeof(infoHeader));
    infoHeader.biSize        = 40;
    infoHeader.biWidth       = (int)width;
    infoHeader.biHeight      = (int)height;   // 正值 = bottom-up (标准 BMP)
    infoHeader.biPlanes      = 1;
    infoHeader.biBitCount    = 32;
    infoHeader.biCompression = 0;  // BI_RGB

    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(&infoHeader, sizeof(infoHeader), 1, fp);

    // 像素数据 (BMP bottom-up: 从下到上逐行, RGBA → BGRA)
    for (int y = (int)height - 1; y >= 0; --y)
    {
        const unsigned char* row = buffer + y * stride;
        for (unsigned x = 0; x < width; ++x)
        {
            fputc(row[x * 4 + 2], fp);  // B
            fputc(row[x * 4 + 1], fp);  // G
            fputc(row[x * 4 + 0], fp);  // R
            fputc(row[x * 4 + 3], fp);  // A
        }
    }

    fclose(fp);
    printf("Image saved: %s (%ux%u)\n", filename, width, height);
}

// ============================================================================
// 绘图: 在渲染缓冲区上绘制所有示例
// ============================================================================
template<class PixFmt>
void drawScene(agg::rendering_buffer& rbuf, unsigned width, unsigned height)
{
    typedef agg::renderer_base<PixFmt>              RenBase;
    typedef agg::rasterizer_scanline_aa<>            Rasterizer;
    typedef agg::scanline_p8                         Scanline;

    // ---- 初始化渲染器 ----
    PixFmt pixf(rbuf);
    RenBase renBase(pixf);

    // 白色背景
    renBase.clear(agg::rgba(1.0, 1.0, 1.0, 1.0));

    Rasterizer ras;
    Scanline   sl;

    // Anti-alias 伽马值 (默认1.0)
    ras.gamma(agg::gamma_none());

    // ======================================================================
    // 1. 绘制区域边框
    // ======================================================================
    {
        agg::path_storage border;
        border.move_to(0.5, 0.5);
        border.line_to(width - 0.5, 0.5);
        border.line_to(width - 0.5, height - 0.5);
        border.line_to(0.5, height - 0.5);
        border.close_polygon();

        agg::conv_stroke<agg::path_storage> borderStroke(border);
        borderStroke.width(1.0);
        ras.add_path(borderStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase, agg::rgba8(40, 40, 40, 80));
    }

    // ======================================================================
    // 2. 实心矩形 + 描边 (左上区域)
    // ======================================================================
    {
        // 实心填充矩形
        agg::path_storage rect;
        rect.move_to(20, 20);
        rect.line_to(180, 20);
        rect.line_to(180, 90);
        rect.line_to(20, 90);
        rect.close_polygon();
        ras.add_path(rect);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(70, 130, 180, 200));

        // 描边
        agg::conv_stroke<agg::path_storage> rectStroke(rect);
        rectStroke.width(2.5);
        ras.add_path(rectStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(25, 25, 112, 255));
    }

    // ======================================================================
    // 3. 圆角矩形 (左上第二块)
    // ======================================================================
    {
        agg::rounded_rect rrect(200, 20, 370, 90, 12.0);
        ras.add_path(rrect);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(60, 179, 113, 200));

        agg::conv_stroke<agg::rounded_rect> rrectStroke(rrect);
        rrectStroke.width(2.0);
        ras.add_path(rrectStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(46, 139, 87, 255));
    }

    // ======================================================================
    // 4. 椭圆 (右上)
    // ======================================================================
    {
        agg::ellipse ell(500, 55, 70, 35);
        ras.add_path(ell);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(255, 127, 80, 180));

        agg::conv_stroke<agg::ellipse> ellStroke(ell);
        ellStroke.width(2.5);
        ras.add_path(ellStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(233, 84, 32, 255));
    }

    // ======================================================================
    // 5. 三次贝塞尔曲线 (第二行)
    // ======================================================================
    {
        agg::path_storage curvePath;
        curvePath.move_to(20, 170);
        curvePath.curve4(60, 100, 140, 240, 200, 170);
        curvePath.curve4(260, 100, 340, 240, 400, 170);
        curvePath.curve4(460, 100, 540, 240, 600, 170);

        agg::conv_curve<agg::path_storage> smooth(curvePath);
        smooth.approximation_scale(2.0);

        agg::conv_stroke<agg::conv_curve<agg::path_storage>> curveStroke(smooth);
        curveStroke.width(3.0);
        curveStroke.line_cap(agg::round_cap);
        curveStroke.line_join(agg::round_join);
        ras.add_path(curveStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(220, 20, 60, 255));

        // 控制点
        double ctrlX[] = {60, 140, 260, 340, 460, 540};
        double ctrlY[] = {100, 240, 100, 240, 100, 240};
        for (int i = 0; i < 6; ++i)
        {
            agg::ellipse dot(ctrlX[i], ctrlY[i], 3.5, 3.5, 8);
            ras.add_path(dot);
            agg::render_scanlines_aa_solid(ras, sl, renBase,
                                           agg::rgba8(128, 128, 128, 200));
        }
    }

    // ======================================================================
    // 6. 虚线正弦波 (第三行)
    // ======================================================================
    {
        agg::path_storage wave;
        wave.move_to(20, 240);
        for (int i = 0; i <= 120; ++i)
        {
            double t = i / 120.0;
            double x = 20 + t * 600;
            double y = 240 + std::sin(t * 4.0 * agg::pi) * 28;
            wave.line_to(x, y);
        }

        agg::conv_dash<agg::path_storage> dash(wave);
        dash.add_dash(10.0, 5.0);   // 10px on, 5px off
        dash.add_dash(3.0, 5.0);    // 3px on, 5px off
        dash.dash_start(0);

        agg::conv_stroke<agg::conv_dash<agg::path_storage>> dashLine(dash);
        dashLine.width(2.5);
        dashLine.line_cap(agg::round_cap);
        ras.add_path(dashLine);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(106, 90, 205, 255));
    }

    // ======================================================================
    // 7. 线性渐变填充 (第四行)
    // ======================================================================
    {
        typedef agg::span_allocator<agg::rgba8>            SpanAlloc;
        typedef agg::span_interpolator_linear<>           Interpolator;
        typedef agg::span_gradient<agg::rgba8,
                                   Interpolator,
                                   agg::gradient_x,
                                   agg::gradient_linear_color<agg::rgba8>> SpanGrad;

        // 渐变函数: 从左(红) 到右(蓝), 水平方向
        agg::gradient_x gradFunc;
        agg::gradient_linear_color<agg::rgba8> colorFunc(
            agg::rgba8(255, 60, 60, 255),   // 起点颜色: 红色
            agg::rgba8(60, 60, 255, 255)    // 终点颜色: 蓝色
        );

        // 插值器: 将渐变映射到矩形区域
        agg::trans_affine mtx;
        mtx *= agg::trans_affine_translation(20, 300);
        mtx *= agg::trans_affine_scaling(600.0, 50.0);
        Interpolator interpolator(mtx);

        SpanAlloc spanAlloc;
        SpanGrad spanGrad(interpolator, gradFunc, colorFunc, 0, 600);

        agg::path_storage gradRect;
        gradRect.move_to(20, 300);
        gradRect.line_to(620, 300);
        gradRect.line_to(620, 350);
        gradRect.line_to(20, 350);
        gradRect.close_polygon();

        ras.add_path(gradRect);
        agg::render_scanlines_aa(ras, sl, renBase, spanAlloc, spanGrad);

        // 描边
        agg::conv_stroke<agg::path_storage> gradStroke(gradRect);
        gradStroke.width(1.5);
        ras.add_path(gradStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase, agg::rgba8(40, 40, 80, 255));
    }

    // ======================================================================
    // 8. 径向渐变 (右侧中部 - 模拟发光球体)
    // ======================================================================
    {
        typedef agg::span_allocator<agg::rgba8>            RadSpanAlloc;
        typedef agg::span_interpolator_linear<>           RadInterpolator;
        typedef agg::span_gradient<agg::rgba8,
                                   RadInterpolator,
                                   agg::gradient_radial,
                                   agg::gradient_linear_color<agg::rgba8>> RadSpanGrad;

        agg::gradient_radial radFunc;
        agg::gradient_linear_color<agg::rgba8> colorFunc(
            agg::rgba8(255, 255, 255, 255),   // 内圈: 白色高光
            agg::rgba8(60, 30, 10, 255)       // 外圈: 深色边缘
        );

        agg::trans_affine mtx;
        mtx *= agg::trans_affine_translation(500, 430);
        mtx *= agg::trans_affine_scaling(40, 40);
        RadInterpolator interpolator(mtx);

        RadSpanAlloc spanAlloc;
        RadSpanGrad spanGrad(interpolator, radFunc, colorFunc, 0, 40);

        agg::ellipse ball(500, 430, 40, 40);
        ras.add_path(ball);
        agg::render_scanlines_aa(ras, sl, renBase, spanAlloc, spanGrad);
    }

    // ======================================================================
    // 9. Gouraud 着色三角形 (右下)
    // ======================================================================
    {
        typedef agg::span_allocator<agg::rgba8>          SpanAlloc;
        typedef agg::span_gouraud_rgba<agg::rgba8>       GouraudSpan;

        double tx[3] = { 600, 580, 500 };
        double ty[3] = { 500, 580, 580 };

        agg::rgba8 colors[3] = {
            agg::rgba8(255, 0,   0,   220),   // 红
            agg::rgba8(0,   255, 0,   220),   // 绿
            agg::rgba8(0,   0,   255, 220)    // 蓝
        };

        SpanAlloc spanAlloc;
        GouraudSpan spanGouraud;
        spanGouraud.colors(colors[0], colors[1], colors[2]);
        spanGouraud.triangle(tx[0], ty[0], tx[1], ty[1], tx[2], ty[2], 0.0);
        ras.add_path(spanGouraud);
        agg::render_scanlines_aa(ras, sl, renBase, spanAlloc, spanGouraud);

        // 三角形描边
        agg::path_storage tri;
        tri.move_to(tx[0], ty[0]);
        tri.line_to(tx[1], ty[1]);
        tri.line_to(tx[2], ty[2]);
        tri.close_polygon();
        agg::conv_stroke<agg::path_storage> triStroke(tri);
        triStroke.width(1.5);
        ras.add_path(triStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase, agg::rgba8(0, 0, 0, 150));
    }

    // ======================================================================
    // 10. 等值线 (左下) - 多个向外扩展的椭圆轮廓
    // ======================================================================
    {
        agg::ellipse inner(120, 450, 65, 40);

        // 最内层实心填充
        ras.add_path(inner);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(100, 149, 237, 200));

        // 连续向外扩展的描边
        agg::conv_contour<agg::ellipse> contour(inner);
        for (int i = 0; i < 5; ++i)
        {
            contour.width(-5.0 * (i + 1));
            ras.add_path(contour);
            int a = 200 - i * 30;
            agg::render_scanlines_aa_solid(ras, sl, renBase,
                agg::rgba8(70, 130, 180, (unsigned char)(a < 30 ? 30 : a)));
        }

        // 最内层描边
        agg::conv_stroke<agg::ellipse> innerStroke(inner);
        innerStroke.width(1.5);
        ras.add_path(innerStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase, agg::rgba8(25, 25, 112, 255));
    }

    // ======================================================================
    // 11. 圆弧/扇形 (中下)
    // ======================================================================
    {
        agg::bezier_arc arc(370, 490, 55, 55,
                            agg::deg2rad(45), agg::deg2rad(315));
        agg::conv_curve<agg::bezier_arc> arcCurve(arc);
        arcCurve.approximation_scale(2.0);

        agg::path_storage arcPath;
        arcPath.concat_path(arcCurve, 0);
        arcPath.close_polygon();

        ras.add_path(arcPath);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(255, 215, 0, 180));

        agg::conv_stroke<agg::path_storage> arcStroke(arcPath);
        arcStroke.width(2.0);
        ras.add_path(arcStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(218, 165, 32, 255));
    }

    // ======================================================================
    // 12. 仿射变换星形 (右侧)
    // ======================================================================
    {
        agg::path_storage star;
        double cx = 500, cy = 530;
        double outerR = 22, innerR = 10;
        int nPts = 5;

        for (int i = 0; i < nPts * 2; ++i)
        {
            double angle = -agg::pi / 2.0 + i * agg::pi / nPts;
            double r = (i % 2 == 0) ? outerR : innerR;
            double x = cx + std::cos(angle) * r;
            double y = cy + std::sin(angle) * r;
            if (i == 0) star.move_to(x, y);
            else        star.line_to(x, y);
        }
        star.close_polygon();

        agg::trans_affine starMtx;
        starMtx *= agg::trans_affine_translation(-cx, -cy);
        starMtx *= agg::trans_affine_skewing(0.3, 0.0);
        starMtx *= agg::trans_affine_scaling(1.0, 0.7);
        starMtx *= agg::trans_affine_rotation(agg::deg2rad(15));
        starMtx *= agg::trans_affine_translation(cx + 55, cy + 30);

        agg::conv_transform<agg::path_storage> starTrans(star, starMtx);
        ras.add_path(starTrans);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(255, 215, 0, 230));

        agg::conv_stroke<agg::conv_transform<agg::path_storage>> starStroke(starTrans);
        starStroke.width(1.5);
        ras.add_path(starStroke);
        agg::render_scanlines_aa_solid(ras, sl, renBase,
                                       agg::rgba8(180, 120, 0, 255));
    }

    // ======================================================================
    // 13. 散点图 (右下角)
    // ======================================================================
    {
        std::srand(42);
        for (int i = 0; i < 60; ++i)
        {
            double cx = 30 + (std::rand() % 250);
            double cy = 500 + (std::rand() % 80);
            double cr = 1.0 + (std::rand() % 4);

            agg::ellipse dot(cx, cy, cr, cr, 6);
            ras.add_path(dot);
            agg::render_scanlines_aa_solid(ras, sl, renBase,
                agg::rgba8((unsigned char)(180 + std::rand() % 76),
                           (unsigned char)(80 + std::rand() % 100),
                           (unsigned char)(std::rand() % 120), 200));
        }
    }

    // ======================================================================
    // 14. 矢量文字渲染 (底部)
    // ======================================================================
    {
        agg::gsv_text txt;
        txt.size(11.0);
        txt.flip(true);

        const char* lines[] = {
            "AGG Anti-Grain Geometry - 2D Vector Graphics Library Demo",
            "Anti-Aliased Rendering | Bezier Curves | Gradient Fills | Gouraud Shading",
            "Rectangles | Ellipses | Arcs | Dashed Lines | Contours | Star Transform"
        };

        for (int i = 0; i < 3; ++i)
        {
            txt.start_point(18, 568 + i * 16.0);
            txt.text(lines[i]);

            agg::conv_stroke<agg::gsv_text> txtStroke(txt);
            txtStroke.width(1.2);
            txtStroke.line_cap(agg::round_cap);
            ras.add_path(txtStroke);
            agg::render_scanlines_aa_solid(ras, sl, renBase,
                                           agg::rgba8(45, 45, 45, 255));
        }
    }
}

// ============================================================================
// 主函数
// ============================================================================
int main()
{
    const unsigned width  = 640;
    const unsigned height = 600;
    const unsigned stride = width * 4;  // RGBA = 4 bytes per pixel

    // 分配渲染缓冲区
    std::vector<unsigned char> buffer(stride * height);
    agg::rendering_buffer rbuf(&buffer[0], width, height, stride);

    // 执行绘制 (使用预乘 Alpha 像素格式)
    {
        typedef agg::pixfmt_rgba32_pre PixFmt;
        drawScene<PixFmt>(rbuf, width, height);
    }

    // 保存为 BMP 文件
    saveBmp("exampleAgg.bmp", &buffer[0], width, height, stride);

    printf("\n=== AGG Example Zones ===\n");
    printf("  Top-left:     Filled rect + stroke\n");
    printf("  Top-mid:      Rounded rect\n");
    printf("  Top-right:    Ellipse\n");
    printf("  Row 2:        Cubic Bezier curves (gray dots = control points)\n");
    printf("  Row 3:        Dashed sine wave\n");
    printf("  Row 4:        Linear gradient (red -> blue)\n");
    printf("  Mid-right:    Radial gradient sphere\n");
    printf("  Bottom-right: Gouraud shaded triangle\n");
    printf("  Bottom-left:  Contour outlines (concentric ellipses)\n");
    printf("  Bottom-mid:   Arc/fan shape\n");
    printf("  Bottom-right: Transformed star + scatter plot\n");
    printf("  Bottom text:  Vector text via gsv_text\n");
    printf("\nDone!\n");

    return 0;
}

#else  // !AST_WITH_AGG

#include <cstdio>
int main()
{
    printf("AGG library is not available.\n");
    printf("Please ensure the 'agg' package is installed and AST_WITH_AGG is defined.\n");
    printf("Run: xmake f --with_test=y && xmake\n");
    return 0;
}

#endif  // AST_WITH_AGG
