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
    matplot::register_backend("agg", create_agg_backend);
    matplot::change_default_backend("agg");

    // 生成数据
    const int n = 200;
    std::vector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i * (2.0 * M_PI) / (n - 1);
        y[i] = std::sin(x[i]);
    }

    // quiet_mode=true: 只在 show() 时 draw 一次
    auto fig = matplot::figure(true);

    matplot::plot(x, y);
    matplot::xlabel("x");
    matplot::ylabel("sin(x)");
    matplot::title("Plot of the Sine Function");

    fig->show();
    fig->save("agg_backend2.bmp");

    return 0;
}
