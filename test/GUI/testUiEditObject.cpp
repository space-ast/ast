///
/// @file      testUiEditObject.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
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

#include "AstUtil/Object.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstCore/RunTime.hpp"
#include "AstTest/Test.h"
#include "AstSim/MotionTwoBody.hpp"

AST_USING_NAMESPACE

TEST(UiEditObject, Test)
{
    SharedPtr<MotionTwoBody> motion = new MotionTwoBody();
    aInitialize();
    std::vector<std::string> names;
    names = {"BodyEphemerisSPK", "BodyEphemerisDE"};
    for(auto& name: names)
    {
        printf("test for className: %s\n", name.c_str());
        auto obj = aMakeObject(name);
        EXPECT_TRUE(obj);
        EXPECT_EQ(obj->showEditDialog(), 0);
    }

    aGetAllClassNames(names);
    for(auto& name: names)
    {
        printf("test for className: %s\n", name.c_str());
        auto obj = aMakeObject(name);
        EXPECT_TRUE(obj);
        EXPECT_EQ(obj->showEditDialog(), 0);
    }
}

GTEST_MAIN()

