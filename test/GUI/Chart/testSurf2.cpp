#ifdef AST_WITH_MATPLOT
#include <cmath>
#include <matplot/matplot.h>
#include "ast/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();
    
    auto xy = meshgrid(iota(1, 0.5, 10), iota(1, 20));
    auto& X = xy.first;
    auto& Y = xy.second;
    auto Z =
        transform(X, Y, [](double x, double y) { return sin(x) + cos(y); });
    surf(X, Y, Z);

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
