#include <cmath>
#include <matplot/matplot.h>
#include "AstChart/QwtBackend.hpp"

int main() {
    AST_USING_NAMESPACE
    aUseQwtBackend();
    using namespace matplot;
    std::pair<vector_2d, vector_2d> XY = meshgrid(iota(-8, .5, +8));
    auto& X = XY.first;
    auto& Y = XY.second;
    auto Z = transform(X, Y, [](double x, double y) {
        double eps = std::nextafter(0.0, 1.0);
        double R = sqrt(pow(x, 2) + pow(y, 2)) + eps;
        return sin(R) / R;
    });
    mesh(X, Y, Z);

    show();
    return 0;
}