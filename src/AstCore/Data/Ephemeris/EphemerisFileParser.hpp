///
/// @file      EphemerisFileParser.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-23
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
#include "AstCore/Ephemeris.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 星历文件解析器接口
/// 参考了Orekit的EphemerisFileParser的接口名称和设计
class AST_CORE_API EphemerisFileParser
{
public:
    virtual ~EphemerisFileParser() = default;
    virtual errc_t parse(StringView filepath, HEphemeris& ephemeris) const = 0;

    inline HEphemeris parse(StringView filepath) const
    {
        HEphemeris ephemeris;
        parse(filepath, ephemeris);
        return ephemeris;
    }
};


/*! @} */

AST_NAMESPACE_END
