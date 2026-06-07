/**
 * @file exampleAggLine.cpp
 * @brief  验证 AggBackend 的 line 绘制功能
 *
 * 使用 matplot++ API 创建 figure → axes → line，
 * 通过 AggBackend 渲染并输出为 BMP。
 *
 * 编译: xmake b exampleAggLine
 * 运行: xmake run exampleAggLine
 * 输出: exampleAggLine.bmp
 */

#ifdef AST_WITH_AGG
#ifdef AST_WITH_MATPLOT

#define NOMINMAX

#include <cmath>
#include <cstdio>
#include <vector>

#include <matplot/matplot.h>
#include <matplot/backend/backend_registry.h>

#include "AstPlot/AggBackend.hpp"
#include "AstPlot/AggRenderer.hpp"

int main() {
    // ---- 注册 Agg 后端 ----
    matplot::register_backend("agg", []() -> matplot::backend::backend_interface* {
        return new ast::AggBackend();
    });
    matplot::change_default_backend("agg");

    // ---- 创建 figure ----
    auto fig = matplot::figure(true);
    fig->title("AggBackend Line Test");

    // ---- 画一条正弦曲线 ----
    std::vector<double> x(200);
    std::vector<double> y(200);
    for (size_t i = 0; i < 200; ++i) {
        x[i] = i * 0.1;  // 0 .. 19.9
        y[i] = std::sin(x[i]);
    }
    auto l1 = matplot::plot(x, y);
    l1->display_name("sin(x)");
    l1->line_width(1.5f);

    // ---- 再画一条余弦虚线 ----
    std::vector<double> y2(200);
    for (size_t i = 0; i < 200; ++i) {
        y2[i] = std::cos(x[i]);
    }
    auto l2 = matplot::plot(x, y2);
    l2->display_name("cos(x)");
    l2->line_width(2.0f);
    l2->line_style("--");  // 虚线

    // ---- 显示图例 ----
    matplot::legend("on");

    // ---- 渲染 ----
    fig->backend()->output("agg_backend1.bmp", "bmp");
    fig->show();

    printf("Done! Check agg_backend1.bmp\n");
    return 0;
}

#else
#include <cstdio>
int main() {
    printf("matplot++ not available.\n");
    return 0;
}
#endif
#else
#include <cstdio>
int main() {
    printf("AGG not available.\n");
    return 0;
}
#endif
