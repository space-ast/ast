#ifdef AST_WITH_MATPLOT
#include <matplot/matplot.h>
#include <random>
#include "ast/QwtBackend.hpp"
AST_USING_NAMESPACE
int main() {
    using namespace matplot;
    // aUseQwtBackend();

    auto x = linspace(0, 3 * pi, 200);
    auto y = transform(x, [&](double x) { return cos(x) + rand(0, 1); });
    auto c = linspace(1, 10, x.size());

    auto l = scatter(x, y, 6, c);
    l->marker_face(true);

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
