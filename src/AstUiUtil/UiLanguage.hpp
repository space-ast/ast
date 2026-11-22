///
/// @file      UiLanguage.hpp
/// @brief     界面语言与多语言文本解析
/// @details   插件元数据里的多语言文本（如 {"zh":"中文","en":"English"}）按界面语言解析，
///            本文件提供语言码归一化、当前语言获取与文本解析
/// @author    axel
/// @date      2026-11-22
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
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/StringView.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 当前界面语言的主语言码（如 "zh"、"en"）
/// @details 优先级：aUiSetLanguage() 设置的值 → 环境变量 AST_UI_LANG → Qt 系统区域设置。
///          带区域的语言码会归一化到主语言（"zh-CN" → "zh"）
AST_UIUTIL_API std::string aUiCurrentLanguage();

/// @brief 设置界面语言，影响 aUiResolveText() 的默认解析结果
/// @param lang 主语言码，如 "zh"；传空串表示恢复为"环境变量 → 系统区域设置"的顺序
/// @note 宿主应把自己实际使用的界面语言告诉这里。
///       Qt 的 QLocale::system() 是系统区域设置，不等于应用安装的翻译语言，两者不一致时菜单文案会跟着系统语言走
/// @note 只影响之后的文本解析，不会触发刷新或通知；宿主切换语言后重建自己的界面即可
/// @note 只应在主线程调用
AST_UIUTIL_API void aUiSetLanguage(StringView lang);

/// @brief 解析多语言文本
/// @details 形如 {"zh":"中文","en":"English"} 的对象，也接受纯字符串。
///          查找顺序：当前语言 → "en" → 字母序第一个字符串值；都找不到返回空串
/// @param node 元数据节点
/// @param lang 主语言码，如 "zh"；为空时按上述顺序回退
AST_UIUTIL_API std::string aUiResolveText(const JsonValue& node, StringView lang);

/*! @} */

AST_NAMESPACE_END
