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
