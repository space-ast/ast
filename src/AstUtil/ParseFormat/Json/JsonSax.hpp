///
/// @file      JsonSax.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-16
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_UTIL_API JsonSax
{
public:
    virtual ~JsonSax() = default;
    
    /// @brief 开始解析对象
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t startObject() { return 0; }
    
    /// @brief 结束解析对象
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t endObject() { return 0; }

    /// @brief 开始解析数组
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t startArray() { return 0; }
    
    /// @brief 结束解析数组
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t endArray() { return 0; }

    /// @brief 解析对象键
    /// @param key 对象键
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t key(StringView key) { return 0; }
    
    /// @brief 解析数组索引
    /// @param index 数组索引
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t index(size_t index) { return 0; }

    
    /// @brief 解析 null 值
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t nullValue() { return 0; }
    
    /// @brief 解析布尔值
    /// @param value 布尔值
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t boolValue(bool value) { return 0; }
    
    /// @brief 解析整数值
    /// @param value 整数值
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t intValue(int value) { return 0; }
    
    /// @brief 解析双精度浮点数值
    /// @param value 双精度浮点数值
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t doubleValue(double value) { return 0; }
    
    /// @brief 解析字符串值
    /// @param value 字符串值
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t stringValue(StringView value) { return 0; }
    
    /// @brief 解析错误
    /// @param message 错误消息
    /// @return 如果继续解析返回 0，否则返回错误码
    virtual errc_t parseError(StringView message) { return 0; }
};

/*! @} */

AST_NAMESPACE_END