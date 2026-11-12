#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#if defined(AST_WITH_AGG) && defined(AST_WITH_MATPLOTPLUSPLUS)
#include <matplot/matplot.h>
#include "AstPlot/PlotBackend.hpp"

AST_USING_NAMESPACE

int main()
{
    // 选择 Agg 后端（由 AstPluginAgg 插件提供）
    if (aUsePlotBackend("agg") != eNoError)
        return 1;

    const int n = 50;
    std::vector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i * (2.0 * M_PI) / (n - 1);
        y[i] = std::sin(x[i]);
    }

    auto fig = matplot::figure(true);

    auto l = matplot::plot(x, y, "-o");
    l->display_name("sin(x)");
    l->marker_size(6.0f);
    l->marker_face(true);

    matplot::legend();
    matplot::grid(matplot::on);
    matplot::xlabel("x");
    matplot::ylabel("sin(x)");
    matplot::title("Plot of the Sine Function");

    fig->show();
    fig->save("agg_backend2.bmp");

    return 0;
}
#else
int main()
{
    // Agg 或 MatPlot 不可用，跳过该示例。
    return 0;
}
#endif

