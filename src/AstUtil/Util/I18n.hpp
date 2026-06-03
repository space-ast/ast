///
/// @file      I18n.hpp
/// @brief     Internationalization and Localization API
/// @details   国际化和本地化接口
/// @author    axel
/// @date      2026-06-03
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
    @addtogroup 
    @{
*/

/// @brief 设置当前文本域
AST_UTIL_CAPI const char* aTextDomain(const char *domainname);


/// @brief 获取翻译文本
AST_UTIL_CAPI const char* aGetText(const char* msgid);

/// @brief 获取上下文翻译文本
/// @param msgctxt 上下文
/// @param msgid 消息ID
AST_UTIL_CAPI const char* aTranslate(const char* msgctxt, const char* msgid);

//-----------------------
// 与 Qt 兼容的函数接口
//-----------------------

/// @brief 获取翻译文本
A_ALWAYS_INLINE const char* tr(const char* msgid)
{
    return aGetText(msgid);
}

#ifndef QT_TR_NOOP
    #define QT_TR_NOOP(String) String
#endif

//-----------------------
// 与 libintl 兼容的函数接口
//-----------------------

/// @brief 获取翻译文本
A_ALWAYS_INLINE const char* gettext(const char* msgid)
{
    return aGetText(msgid);
}

/// @brief 获取上下文文本
/// The letter 'p' stands for 'particular' or 'special'.  
/// @param msgctxt 上下文
/// @param msgid 消息ID
/// @return 上下文文本
A_ALWAYS_INLINE const char* pgettext(const char* msgctxt, const char* msgid)
{
    return aTranslate(msgctxt, msgid);
}

/// @brief 翻译文本
A_ALWAYS_INLINE const char* _(const char* msgid)
{
    return aGetText(msgid);
}


// 仅用于标记翻译字符串，不进行即时翻译
#ifndef N_
    #define N_(String) String
#endif


/// @brief 设置当前文本域
A_ALWAYS_INLINE const char* textdomain(const char* domainname)
{
    return aTextDomain(domainname);
}

/*! @} */

AST_NAMESPACE_END


