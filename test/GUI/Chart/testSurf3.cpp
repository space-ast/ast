#include <cmath>
#include <matplot/matplot.h>
#include "AstChart/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();
    auto xy = meshgrid(iota(1, 0.5, 10), iota(1, 20));
    auto& X = xy.first;
    auto& Y = xy.second;
    auto Z =
        transform(X, Y, [](double x, double y) { return sin(x) + cos(y); });
    auto C = transform(X, Y, [](double x, double y) { return x * y; });
    surf(X, Y, Z, C);
    colorbar();

    show();
    return 0;
}