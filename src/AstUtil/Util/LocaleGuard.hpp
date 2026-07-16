///
/// @file      LocaleGuard.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-07
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
#include <clocale>      // std::setlocale
#include <string>       // std::string
#include <stdexcept>    // std::runtime_error
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 用于切换和恢复 locale 的 RAII 类。
class LocaleGuard
{
public:
    /// @brief 构造并切换到指定的 locale。
    /// @param new_locale  要设置的 locale 名称，例如 "en_US.UTF-8" 或 "C"
    /// @param category    要修改的类别，默认为 LC_ALL（也可用 LC_CTYPE 等）
    explicit LocaleGuard(const char* new_locale, int category = LC_ALL)
        : active_(false)
        , category_(category)
    {
        // 1. 保存当前 locale 的字符串表示
        const char* old = std::setlocale(category_, nullptr);
        if (old != nullptr) {
            old_locale_ = old;   // 复制一份字符串
        }
        // 2. 尝试设置新 locale
        if (std::setlocale(category_, new_locale) != nullptr) {
            active_ = true;      // 设置成功，析构时需要恢复
        } else {
            // 设置失败，不改变当前 locale
            aError("failed to set locale '%s'", new_locale);
        }
    }


    ~LocaleGuard() {
        if (active_ && !old_locale_.empty()) {
            std::setlocale(category_, old_locale_.c_str());
        }
    }

    // 可选：提前恢复并停止自动恢复（类似 lock_guard 的 release 语义）
    void release() {
        active_ = false;
    }
    A_DISABLE_COPY(LocaleGuard);

private:
    bool active_{};           // 是否需要在析构时恢复
    int category_{};          // 影响哪个 locale 类别（LC_ALL, LC_CTYPE 等）
    std::string old_locale_{};// 保存的原 locale 字符串
};

/*! @} */

AST_NAMESPACE_END
