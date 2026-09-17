#ifdef AST_WITH_MATPLOT
#include <matplot/matplot.h>
#include <random>
#include "ast/QwtBackend.hpp"
AST_USING_NAMESPACE
int main() {
    using namespace matplot;
    aUseQwtBackend();

    auto theta = linspace(0, 1, 500);
    auto x = transform(
        theta, [&](double theta) { return exp(theta) * sin(100 * theta); });
    auto y = transform(
        theta, [&](double theta) { return exp(theta) * cos(100 * theta); });

    auto s = scatter(x, y);
    s->marker_color("b");
    s->marker_face_color({0, .5, .5});

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
