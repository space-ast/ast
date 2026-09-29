///
/// @file      testUnitManager.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-15
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/AstTestMacro.h"
#include "ast/UnitManager.hpp"

AST_USING_NAMESPACE 

TEST(UnitManager, AddUnit)
{
    UnitManager unitManager;
}


TEST(UnitManager, GetUnit)
{
    UnitManager unitManager;
    auto unit = unitManager.getUnit("m");
    nothing();

    A_UNUSED(unit);
}





// 验证温度 SI 单位判定(应为开尔文而非摄氏度)与按量纲取单位的去重
TEST(UnitManager, TemperatureSiAndDimension)
{
    // 温度 SI 单位应为 K(纯乘法 scale=1)，而非摄氏度(仿射)
    auto si = aUnitGetSI(Dimension::Temperature());
    ASSERT_NE(si, nullptr);
    EXPECT_EQ(si->name(), "K");

    // K/°C/°F/°R 四种不同换算，去重后仍应保留 ≥4 个
    auto list = aUnitsGetByDimension(Dimension::Temperature());
    EXPECT_GE(list.size(), 4u);

    // 且应包含开尔文
    bool hasK = false;
    for (auto& u : list) {
        if (u.name() == "K") hasK = true;
    }
    EXPECT_TRUE(hasK);
}

GTEST_MAIN()