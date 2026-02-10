///
/// @file      QuantityParser.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-15
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Quantity
    @{
*/



class Quantity;
class Unit;

/// @brief 解析量字符串，将其解析为数值和单位。
/// @details 例如 "123.456 m/s", "123.456m", "123.456[m/s]"。
/// @param[in] str 量字符串
/// @param[out] value 解析得到的数值
/// @param[out] unit 解析得到的单位
/// @return err_t 错误码
AST_UTIL_API err_t aQuantityParse(StringView str, double& value, Unit& unit);



/// @brief 解析量字符串
/// @details 例如 "123.456 m/s", "123.456m", "123.456[m/s]"。
/// @param[in] str 量字符串
/// @param[out] quantity 解析后的量
/// @return err_t 错误码
AST_UTIL_API err_t aQuantityParse(StringView str, Quantity& quantity);


/// @brief 解析量字符串
/// @details 例如 "123.456 m/s", "123.456m", "123.456[m/s]"。
/// @param[in] str 量字符串
/// @return Quantity 解析得到的量，如果解析失败返回 Quantity::NaN()
AST_UTIL_API Quantity aQuantityParse(StringView str);


/*! @} */


AST_NAMESPACE_END
