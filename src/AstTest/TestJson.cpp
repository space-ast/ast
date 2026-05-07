///
/// @file      TestJson.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-24
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

#include "TestJson.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/JsonValue.hpp"

AST_NAMESPACE_BEGIN


errc_t aTestJsonParse(StringView json)
{
    JsonValue value1;
    value1.parseFromString(json);
    std::string str1 = value1.toJsonString();
    JsonValue value2;
    value2.parseFromString(str1);
    std::string str2 = value2.toJsonString();
    if(str1 != str2){
        return -1;
    }
    printf("str1:\n %s\n", str1.c_str());
    printf("str2:\n %s\n", str2.c_str());
    return 0;
}

AST_NAMESPACE_END
