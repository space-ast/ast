#include <cmath>
#include <matplot/matplot.h>
#include "ast/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();

    auto xyz = peaks();
    auto& X = std::get<0>(xyz);
    auto& Y = std::get<1>(xyz);
    auto& Z = std::get<2>(xyz);
    mesh(X, Y, Z)->hidden_3d(false);

    show();
    return 0;
}