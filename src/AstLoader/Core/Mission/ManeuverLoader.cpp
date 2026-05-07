///
/// @file      ManeuverLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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

#include "ManeuverLoader.hpp"
#include "AstUtil/StringView.hpp"
#include "AstScript/Value.hpp"
#include "ValXMLLoader.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Burn.hpp"
#include "AstCore/BurnImpulsive.hpp"
#include "AstCore/BurnCollocation.hpp"
#include "AstCore/BurnFinite.hpp"
#include "AstLoader/BurnLoader.hpp"
#include "AstLoader/SegmentLoader.hpp"


AST_NAMESPACE_BEGIN

errc_t aLoadManeuver(const Value& dictRoot, Maneuver& maneuver)
{
    errc_t rc;
    const std::string type = dictRoot["Type"];
    if(type != "Maneuver")
    {
        aError("invalid type, expect 'Maneuver'");
        return eErrorInvalidParam;
    }

    
    const std::string maneuverType = dictRoot["MnvrType"];
    const std::string mnvrKey = maneuverType + "Mnvr";
    SharedPtr<Burn> burn;
    rc = aLoadBurn(dictRoot[mnvrKey], burn);
    if(rc || !burn)
    {
        aError("failed to load maneuver '%s'", mnvrKey.c_str());
    }
    else
    {
        burn->setParentScope(&maneuver);    // 设置父作用域为机动，避免内存泄漏与野指针问题
        maneuver.setBurn(burn);
    }

    // 加载公共属性
    aLoadSegment(dictRoot, maneuver);
    return eNoError;
}

errc_t aLoadManeuver(StringView filename, Maneuver& maneuver)
{
    errc_t rc;
    SharedPtr<Value> value;
    rc = aLoadValue(filename, value);
    if(rc)  return rc;
    if(!value) return eErrorNullPtr;
    return aLoadManeuver(*value, maneuver);
}

AST_NAMESPACE_END
