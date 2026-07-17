///
/// @file      BasicComponentLoader.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/EventInterval.hpp"
#include "AstCore/EventTime.hpp"
#include "AstCore/CelestialBody.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


struct VehiclePathData
{
    SharedPtr<CelestialBody> centralBody_{};
    bool storeEphemeris_ = false;
    bool smoothInterp_ = false;
};

/// @brief 获取对象文件扩展名
/// @details objectType 对象类型
/// @return 文件扩展名，例如 "sc", "sa", "fa", "sn"
AST_LOADER_API StringView aClassSTKExtension(StringView objectType);


errc_t _aLoadEventInterval(BKVParser& parser, SharedPtr<EventInterval>& eventInterval);
errc_t _aLoadEventTime(BKVParser& parser, SharedPtr<EventTime>& eventTime);
errc_t _aLoadSubObjects(BKVParser& parser, Object* parentObject);

/// @brief 跳过未知块
/// @details 跳过未知块，直到遇到指定块名
/// @param parser BKVParser
/// @param blockName 块名
/// @return errc_t 错误码
void _aSkipUnknownBlock(BKVParser& parser, StringView blockName);

/*! @} */

AST_NAMESPACE_END
