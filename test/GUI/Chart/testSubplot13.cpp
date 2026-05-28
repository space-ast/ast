#include <matplot/matplot.h>
#include "ast/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();

    auto ax1 = subplot(2, 1, 0);
    auto xyz = peaks();
    auto& z = std::get<2>(xyz);

    z = transpose(z);
    plot(ax1, z);
    xlim(ax1, {0, 20});

    auto ax2 = subplot(2, 1, 1);
    plot(ax2, z);

    ax1->font_size(15);
    ax2->line_width(2);

    show();
    return 0;
}