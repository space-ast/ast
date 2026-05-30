#include <cmath>
#include <matplot/matplot.h>
#include "ast/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();

    std::vector<double> x = linspace(0, 2 * pi);
    std::vector<double> y = transform(x, [](double x) { return sin(x); });

    plot(x, y, "-o");
    hold(on);
    plot(x, transform(y, [](double y) { return -y; }), "--xr");
    plot({1.0, 0.7, 0.4, 0.0, -0.4, -0.7, -1}, "k");
    plot(x, transform(x, [](double x) { return x / pi - 1.; }), "-:gs");

    legend({"sin(x)", "-sin(x)", "", "x/pi - 1"});
    show();
    return 0;
}