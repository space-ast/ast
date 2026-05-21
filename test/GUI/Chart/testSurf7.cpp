#include <cmath>
#include <matplot/matplot.h>
#include "AstChart/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();
    auto xy = meshgrid(iota(1, .5, 10), iota(1, 20));
    auto& X = xy.first;
    auto& Y = xy.second;
    auto Z = transform(X, Y, [](double, double) { return 5.; });
    surf(X, Y, Z);

    show();
    return 0;
}