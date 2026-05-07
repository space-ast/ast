///
/// @file      testObjectManager.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-03
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

#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(ObjectManagerTest, MaxObjectCount)
{
    auto instance = &ObjectManager::CurrentInstance();
    uint32_t count = instance->getObjectCount();
    EXPECT_TRUE(count == 0);
    const uint32_t maxCount = 1000;
    instance->setMaxObjectCount(maxCount);
    Object* objectList[maxCount];
    // 测试添加对象到最大数量
    for(uint32_t i = 0; i < maxCount; i++)
    {
        ObjectNamed *obj = new ObjectNamed();
        uint32_t id = aAddObject(obj);
        objectList[i] = obj;
        EXPECT_TRUE(id != INVALID_ID);
    }
    // 测试添加对象到最大数量后，再添加对象
    {
        SharedPtr<Object> obj_sp = new ObjectNamed();
        uint32_t id = aAddObject(obj_sp);
        EXPECT_TRUE(id == INVALID_ID);
    }
    // 测试删除对象后，再添加对象，是否能成功添加
    {
        SharedPtr<Object> obj_sp = objectList[73];
        obj_sp = nullptr;
        Object* obj = new ObjectNamed();
        uint32_t id = aAddObject(obj);
        EXPECT_TRUE(id != INVALID_ID);
    }
}

TEST(ObjectManagerTest, ObjectCount)
{

}

GTEST_MAIN()


