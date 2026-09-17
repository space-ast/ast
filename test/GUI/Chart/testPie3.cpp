#ifdef AST_WITH_MATPLOT
#include <matplot/matplot.h>
#include "ast/QwtBackend.hpp"

int main() {
    using namespace matplot;
    AST_USING_NAMESPACE
    aUseQwtBackend();     

    std::vector<double> x = iota(1, 3);
    std::vector<std::string> labels = {"Taxes", "Expenses", "Profit"};
    pie(x, labels);

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
