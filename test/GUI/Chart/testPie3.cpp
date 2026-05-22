#include <matplot/matplot.h>
#include "AstChart/QwtBackend.hpp"

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