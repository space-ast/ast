///
/// @file      testSequenceProperty.cpp
/// @brief     Sequence 的对象列表属性测试
/// @details   验证 getCommandsSize/getCommandsItem/replaceCommands 通过反射注册后可用
/// @author    axel
/// @date      2026-11-05
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

#include "ast/Sequence.hpp"
#include "ast/Stop.hpp"
#include "ast/Class.hpp"
#include "ast/Property.hpp"
#include "ast/PropertyObjectList.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

TEST(SequenceProperty, Commands)
{
    Sequence sequence;

    // 反射属性已注册
    Property* property = Sequence::StaticType()->getProperty("Commands");
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->getValueType(), EValueType::eObjectList);

    auto* list = dynamic_cast<PropertyObjectList*>(property);
    ASSERT_NE(list, nullptr);
    EXPECT_EQ(list->getSize(&sequence), 0u);

    // 通过 Sequence 自身接口添加的命令，属性接口应能读到
    auto* first = new Stop();
    first->setName("first");
    sequence.addCommand(first);
    ASSERT_EQ(list->getSize(&sequence), 1u);

    Object* item = nullptr;
    ASSERT_EQ(list->getItem(&sequence, 0, item), eNoError);
    EXPECT_EQ(item, first);
    EXPECT_EQ(item->getName(), "first");

    auto* second = new Stop();
    second->setName("second");
    sequence.addCommand(second);
    ASSERT_EQ(list->getSize(&sequence), 2u);
    ASSERT_EQ(list->getItem(&sequence, 1, item), eNoError);
    EXPECT_EQ(item, second);

    // 通过属性接口删除
    ASSERT_EQ(list->removeItem(&sequence, 0), eNoError);
    ASSERT_EQ(list->getSize(&sequence), 1u);
    ASSERT_EQ(list->getItem(&sequence, 0, item), eNoError);
    EXPECT_EQ(item, second);

    // 通过属性接口插入
    auto* third = new Stop();
    third->setName("third");
    ASSERT_EQ(list->insertItem(&sequence, 0, third), eNoError);
    ASSERT_EQ(list->getSize(&sequence), 2u);
    ASSERT_EQ(list->getItem(&sequence, 0, item), eNoError);
    EXPECT_EQ(item, third);

    // 批量替换
    Object* items[] = {third};
    ASSERT_EQ(list->setItems(&sequence, Span<Object*>(items)), eNoError);
    ASSERT_EQ(list->getSize(&sequence), 1u);
    ASSERT_EQ(list->getItem(&sequence, 0, item), eNoError);
    EXPECT_EQ(item, third);

    // 清空
    ASSERT_EQ(list->clearItems(&sequence), eNoError);
    EXPECT_EQ(list->getSize(&sequence), 0u);
}

GTEST_MAIN()
