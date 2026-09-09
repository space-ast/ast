///
/// @file      ModeratorObserver.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-08
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
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


class ModeratorObserverMultiplexer; 

/// @brief 引擎事件监听者接口（观察者）
/// @details 不继承 Object/引用计数；复用器/Moderator 不拥有订阅者，
///          订阅者析构时通过 sources_ 自动从所有登记它的复用器注销。
class AST_ENGINE_API ModeratorObserver
{
public:
    /// @brief 析构时自动从所有登记它的复用器注销
    virtual ~ModeratorObserver();

    /// @brief 对象被添加后通知
    /// @param obj 被添加的对象
    virtual void onObjectAdded(Object& obj) { A_UNUSED(obj); }

    /// @brief 对象将被移除前通知
    /// @param obj 被移除的对象
    virtual void onBeforeObjectRemove(Object& obj) { A_UNUSED(obj); }

    /// @brief 对象已被移除后通知
    /// @param obj 被移除的对象
    virtual void onObjectRemoved(Object& obj) { A_UNUSED(obj); }

    /// @brief 对象将被重命名前通知
    /// @param obj 被重命名的对象
    /// @param oldName 重命名前的旧名称
    virtual void onBeforeObjectRename(Object& obj, StringView oldName, StringView newName) { A_UNUSED(obj); A_UNUSED(newName); A_UNUSED(oldName); }

    /// @brief 对象已被重命名后通知
    /// @param obj 被重命名的对象
    /// @param oldName 重命名前的旧名称
    virtual void onObjectRenamed(Object& obj, StringView oldName, StringView newName) { A_UNUSED(obj); A_UNUSED(newName); A_UNUSED(oldName); }

private:
    friend class ModeratorObserverMultiplexer;

    /// @brief 复用器登记本订阅者时调用（记录反向注册表）
    /// @param src 登记了本订阅者的复用器
    void addSource(ModeratorObserverMultiplexer* src);

    /// @brief 复用器移除本订阅者时调用（清空反向注册表对应项）
    /// @param src 移除了本订阅者的复用器
    void removeSource(ModeratorObserverMultiplexer* src);

    std::vector<ModeratorObserverMultiplexer*> sources_{};   ///< 反向注册表：登记了本订阅者的复用器
};


/*! @} */

AST_NAMESPACE_END
