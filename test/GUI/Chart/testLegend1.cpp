#include <cmath>
#ifdef AST_WITH_MATPLOT
#include <matplot/matplot.h>
#include "ast/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();

    std::vector<double> x = linspace(0, pi);
    std::vector<double> y1 = transform(x, [](double x) { return cos(x); });
    plot(x, y1);

    hold(on);
    std::vector<double> y2 = transform(x, [](double x) { return cos(2 * x); });
    plot(x, y2);

    ::matplot::legend({"cos(x)", "cos(2x)"});

    std::vector<double> y3 = transform(x, [](double x) { return cos(3 * x); });
    auto p = plot(x, y3);
    p->display_name("cos(3x)");
    hold(off);

    show();

    ::matplot::legend(off);
    show();

    return 0;
}
#else
#include <stdio.h>
int main(){
    puts("没有启用matplot++库");
    return 0;
}
#endif
