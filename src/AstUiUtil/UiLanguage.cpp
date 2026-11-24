///
/// @file      UiLanguage.cpp
/// @brief     界面语言与多语言文本解析实现
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

#include "UiLanguage.hpp"

#include <QLocale>

#include <cctype>
#include <cstdlib>
#include <map>

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 宿主显式设置的界面语言码，空表示未设置
/// @note 只由 aUiSetLanguage() 写入、aUiCurrentLanguage() 读取，约定仅在主线程使用
std::string g_requestedLanguage;

/// @brief 归一化语言码：截断到第一个 '-' 或 '_' 之前，并转小写
/// @details "zh_CN"、"zh-CN"、"ZH" 都会归一化成 "zh"
std::string normalizeLanguage(StringView lang)
{
    std::string code(lang);
    const size_t pos = code.find_first_of("-_");
    if (pos != std::string::npos)
        code.resize(pos);
    for (char& c : code)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return code;
}

} // namespace


std::string aUiCurrentLanguage()
{
    if (!g_requestedLanguage.empty())
        return g_requestedLanguage;

    if (const char* env = std::getenv("AST_UI_LANG"))
    {
        if (*env)
            return normalizeLanguage(env);
    }

    return normalizeLanguage(QLocale::system().name().toStdString());
}


void aUiSetLanguage(StringView lang)
{
    g_requestedLanguage = normalizeLanguage(lang);
}


std::string aUiResolveText(const JsonValue& node, StringView lang)
{
    if (node.isString())
        return node.toString();

    if (!node.isObject())
        return std::string();

    const std::map<std::string, JsonValue>& object = node.getObject();
    if (object.empty())
        return std::string();

    // 显式语言码先归一化；未指定时跟随当前界面语言（aUiCurrentLanguage() 的返回值已归一化）
    const std::string want = lang.empty() ? aUiCurrentLanguage() : normalizeLanguage(lang);

    // 当前语言
    for (const auto& kv : object)
    {
        if (kv.second.isString() && normalizeLanguage(kv.first) == want)
            return kv.second.toString();
    }

    // 英文兜底
    for (const auto& kv : object)
    {
        if (kv.second.isString() && normalizeLanguage(kv.first) == "en")
            return kv.second.toString();
    }

    // 最后兜底：字母序第一个字符串值（std::map 定序，结果可复现）
    for (const auto& kv : object)
    {
        if (kv.second.isString())
            return kv.second.toString();
    }

    return std::string();
}

AST_NAMESPACE_END
