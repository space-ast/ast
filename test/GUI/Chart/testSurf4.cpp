#include <cmath>
#include <matplot/matplot.h>
#include "AstChart/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();
    auto xy = meshgrid(iota(-5, 0.5, 5));
    auto& X = xy.first;
    auto& Y = xy.second;
    auto Z = transform(
        X, Y, [](double x, double y) { return y * sin(x) - x * cos(y); });
    auto C = transform(X, Y, [](double x, double y) { return x * y; });
    surf(X, Y, Z)->face_alpha(0.5).edge_color("none");

    show();
    return 0;
}