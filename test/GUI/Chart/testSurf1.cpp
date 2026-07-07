#include <cmath>
#include <matplot/matplot.h>
#include "ast/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();
    auto xy = meshgrid(linspace(-5, +5, 40), linspace(-5, +5, 40));
    auto& X = xy.first;
    auto& Y = xy.second;
    auto Z = transform(X, Y, [](double x, double y) {
        return 10 * 2 + pow(x, 2) - 10 * cos(2 * pi * x) + pow(y, 2) -
               10 * cos(2 * pi * y);
    });
    surf(X, Y, Z);

    show();
    return 0;
}