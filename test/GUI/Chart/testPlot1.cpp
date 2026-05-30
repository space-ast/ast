///
/// @file      testPlotLines.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-19
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstPlot/AstPlot.hpp"
#include "AstChart/QwtBackend.hpp"

AST_USING_NAMESPACE

int main()
{
    aUseQwtBackend();
    
    using plt::pi;
    using plt::on;
    std::vector<double> x = plt::linspace(0, 2 * pi);
    std::vector<double> y = plt::transform(x, [](double x) { return sin(x); });

    plt::plot(x, y, "-o");
    plt::hold(on);
    plt::plot(x, plt::transform(y, [](double y) { return -y; }), "--xr");
    plt::plot(x, plt::transform(x, [](double x) { return x / pi - 1.; }), "-:gs");
    plt::plot({1.0, 0.7, 0.4, 0.0, -0.4, -0.7, -1}, "k");

    plt::show();
}