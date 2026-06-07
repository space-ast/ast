/**
 * @file agg_backend2.cpp
 * @brief  Agg Backend 示例 — 使用 matplot++ API 绘制正弦曲线
 *
 * MATLAB 等效代码:
 *   figure()
 *   x = 0:pi/100:2*pi;
 *   y = sin.(x);
 *   plot(x, y);
 *   xlabel("x");
 *   ylabel("sin(x)");
 *   title("Plot of the Sine Function");
 *
 * 编译: xmake b agg_backend2
 * 运行: xmake run agg_backend2
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <matplot/matplot.h>
#include <matplot/backend/backend_registry.h>
#include "AstPlot/AggBackend.hpp"

static matplot::backend::backend_interface* create_agg_backend() {
    return new ast::AggBackend();
}

int main()
{
    // 注册 Agg 后端
    matplot::register_backend("agg", create_agg_backend);
    matplot::change_default_backend("agg");

    // ---- 生成数据 x = 0:pi/100:2*pi, y = sin(x) ----
    const int n = 200;
    std::vector<double> x(n);
    std::vector<double> y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i * (2.0 * M_PI) / (n - 1);
        y[i] = std::sin(x[i]);
    }

    // ---- 绘图 ----
    auto fig = matplot::figure();
    fig->backend()->output("agg_backend2.bmp");

    matplot::plot(x, y);
    matplot::xlabel("x");
    matplot::ylabel("sin(x)");
    matplot::title("Plot of the Sine Function");

    fig->show();
    fig->save("agg_backend2.bmp");

    return 0;
}
