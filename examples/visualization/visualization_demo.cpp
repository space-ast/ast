///
/// @file      visualization_simple_demo.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-06
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

#include "ast/AstVisualization.hpp"
#include "ast/EphemerisTwoBody.hpp"
#include "ast/Constants.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/RunTime.hpp"
#include "ast/Literals.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    const double radius = kEarthRadius + 35786.0 * 1000.0;   // 地球同步轨道半径 (m)
    const double speed  = std::sqrt(kEarthGrav / radius);    // 圆轨道速度 (m/s)
    CartState initState{Vector3d{radius, 0.0, 0.0}, Vector3d{0.0, speed, 0.0}};

    auto epoch = TimePoint::CurrentTime();
    HFrame icrf = aGetEarth()->getFrameICRF();   // 参考系：地球 ICRF
    SharedPtr<EphemerisTwoBody> twoBody = new EphemerisTwoBody(icrf.get(), kEarthGrav, epoch, initState);

    namespace vis = _AST visualization;
    auto earth = aGetEarth();
    auto moon = aGetMoon();

    vis::trajectory(twoBody)->setInterval({epoch, epoch + 3_day});

    vis::planet(earth);
    vis::planet(moon);

    // 说明：vis::show() 会在运行时加载渲染后端。若存在渲染后端则弹出交互窗口；
    // 否则会打印警告。
    vis::show();

}