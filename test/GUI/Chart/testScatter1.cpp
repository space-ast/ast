#ifdef AST_WITH_MATPLOT
#include <matplot/matplot.h>
#include "ast/QwtBackend.hpp"
#include <random>

int main() {
    using namespace matplot;

    auto x = linspace(0, 3 * pi, 200);
    auto y = transform(x, [&](double x) { return cos(x) + rand(0, 1); });

    scatter(x, y);

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
