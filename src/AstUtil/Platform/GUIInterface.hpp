///
/// @file      GUIInterface.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Platform
    @{
*/

/// @brief GUI接口
/// @details ~
class AST_UTIL_API GUIInterface
{
public:
    /// @brief 获取空GUI接口实例
    /// @details ~
    /// @return GUIInterface* 空GUI接口实例
    static GUIInterface* NoopInstance();

    /// @brief 获取当前GUI接口实例
    /// @details GUI接口实例
    /// @return GUIInterface* 当前GUI接口实例
    static GUIInterface* CurrentInstance();

    /// @brief 设置当前GUI接口实例
    /// @details GUI接口实例
    /// @param instance 要设置的GUI接口实例
    static void SetCurrentInstance(GUIInterface* instance);

    /// @brief 析构函数
    virtual ~GUIInterface() = default;

    /// @brief 编辑对象
    /// @details ~
    /// @param object 要编辑的对象
    /// @return errc_t 错误码
    virtual errc_t editObject(Object* object);

    /// @brief 选择对象
    /// @details ~
    /// @param typeName 对象类型名
    /// @return Object* 选择的对象指针
    virtual Object* selectObject(StringView typeName);

    /// @brief 翻译文本
    /// @details ~
    /// @param msgctxt 上下文
    /// @param msgid 消息ID
    /// @return const char* 翻译后的文本
    virtual const char* translate(const char* msgctxt, const char* msgid);
};



/*! @} */

AST_NAMESPACE_END
