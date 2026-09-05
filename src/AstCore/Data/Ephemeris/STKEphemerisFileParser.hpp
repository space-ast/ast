///
/// @file      STKEphemerisFileParser.hpp
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
#include "AstCore/EphemerisFileParser.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class BKVParser;

/// @brief 加载STK星历文件
/// @param filepath 星历文件路径
/// @param ephemeris 星历对象
/// @return 错误码
AST_CORE_API errc_t aLoadSTKEphemeris(StringView filepath, HEphemeris& ephemeris);

/// @brief 加载STK星历文件
/// @param parser BKV解析器
/// @param ephemeris 星历句柄
/// @return 错误码
AST_CORE_API errc_t aLoadSTKEphemeris(BKVParser& parser, HEphemeris &ephemeris);


/// @brief 加载STK星历文件
/// @param parser BKV解析器
/// @param ephemeris 星历句柄
/// @return 错误码
AST_CORE_API errc_t aLoadSTKEphemeris(BKVParser& parser, ScopedPtr<Ephemeris>& ephemeris);


/// @brief STK星历文件解析器
/// 参考了[Orekit的STKEphemerisFileParser类](https://www.orekit.org/site-orekit-12.0.1/apidocs/org/orekit/files/stk/STKEphemerisFileParser.html)
class AST_CORE_API STKEphemerisFileParser final: public EphemerisFileParser
{
public:
    STKEphemerisFileParser() = default;
    ~STKEphemerisFileParser() override = default;
public:
    using EphemerisFileParser::parse;
    errc_t parse(StringView filepath, HEphemeris& ephemeris) const override;
protected:

};

/*! @} */

AST_NAMESPACE_END
