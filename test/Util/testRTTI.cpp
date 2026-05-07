///
/// @file      testRTTI.cpp
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

#include "AstUtil/RTTIAPI.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/FrameAssembly.hpp"

#include "AstTest/Test.h"
#include <vector>
#include <string>

AST_USING_NAMESPACE



class RTTITest: public testing::Test
{
public:
    void SetUp()
    {
        aInitialize();
    }
    void TearDown()
    {
        aUninitialize();
    }
};

TEST_F(RTTITest, NewObject) 
{
    std::vector<std::string> classNames;
    aGetAllClassNames(classNames);
    for(auto className: classNames)
    {
        bool isVirtual = aIsVirtualClass(className);
        if(!isVirtual){
            printf("test for className: %s\n", className.c_str());
            auto obj = aMakeObject(className);
            EXPECT_TRUE(obj != nullptr);
        }
    }
}

TEST_F(RTTITest, ClassDefaultObject)
{
    std::vector<std::string> classNames;
    aGetAllClassNames(classNames);
    for(auto className: classNames)
    {
        bool isVirtual = aIsVirtualClass(className);
        if(!isVirtual){
            printf("test for className: %s\n", className.c_str());
            auto obj = aGetClassDefaultObject(className);
            EXPECT_TRUE(obj != nullptr);
        }
    }
}

TEST_F(RTTITest, ObjectParentScope)
{
    // 测试父作用域能否正确设置和获取，以及父作用域被销毁后子对象的弱引用是否失效
    {
        SharedPtr<CelestialBody> body = new CelestialBody();
        auto frame = new FrameAssembly();
        frame->setParentScope(body);
        EXPECT_EQ(body.get(), frame->getParentScope());
        WeakPtr<FrameAssembly> frame_wp = frame;
        EXPECT_FALSE(frame_wp.expired());
        body.reset();
        EXPECT_TRUE(frame_wp.expired());
    }
    // 测试父作用域为临时对象时，子对象的弱引用是否失效
    {
        WeakPtr<FrameAssembly> frame_wp;
        {
            CelestialBody body;
            auto frame = new FrameAssembly();
            frame->setParentScope(&body);
            frame_wp = frame;
            EXPECT_FALSE(frame_wp.expired());
        }
        EXPECT_TRUE(frame_wp.expired());
    }
    {
        SharedPtr<CelestialBody> body = new CelestialBody();
        
        auto frame = new FrameAssembly();
        frame->setParentScope(body);
        WeakPtr<FrameAssembly> frame_wp = frame;

        StateCartesian* state = new StateCartesian();
        state->setParentScope(frame);
        WeakPtr<StateCartesian> state_wp = state;

        EXPECT_FALSE(frame_wp.expired());
        EXPECT_FALSE(state_wp.expired());
        body.reset();
        EXPECT_TRUE(frame_wp.expired());
        EXPECT_TRUE(state_wp.expired());
    }
}




GTEST_MAIN()
