///
/// @file      testMetaData.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-29
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

#include "AstAI/AgentUtil.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstSim/MotionTwoBody.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/IO.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(MetaDataTest, TestClassSchema)
{
    Class* cls = MotionTwoBody::StaticType();
    ASSERT_TRUE(cls != nullptr);
    JsonValue schema = aClassJsonSchema(cls);
    ast_printf("%s\n", schema.toJsonString(2).c_str());
}


GTEST_MAIN()