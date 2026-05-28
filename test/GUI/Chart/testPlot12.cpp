#include "ast/AstPlot.hpp"
#include "ast/QwtBackend.hpp"

AST_USING_NAMESPACE

int main() {
    using namespace plt;
    aUseQwtBackend();

    std::vector<int> y = {2, 4, 7, 7, 6, 3, 9, 7, 3, 5};
    plot(y);

    show();
    return 0;
}