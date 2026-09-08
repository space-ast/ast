///
/// @file      Moderator.hpp
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
#include "ModeratorObserverMultiplexer.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/


/// @brief 引擎总控类
/// @details 作为引擎总控，管理对象的增删改名等操作，并广播事件通知给已注册的观察者。
class AST_ENGINE_API Moderator
{
public:
    Moderator() = default;
    ~Moderator() = default;

    /// @brief 获取全局引擎总控实例
    /// @return Moderator& 全局单例引用
    static Moderator& Instance();

    /// @brief 注册事件监听者
    /// @param observer 观察者指针
    void addObserver(ModeratorObserver* observer) { observers_.addObserver(observer); }

    /// @brief 移除事件监听者
    /// @param observer 观察者指针
    void removeObserver(ModeratorObserver* observer) { observers_.removeObserver(observer); }

    /// @brief 添加对象
    /// @param type 对象类型名称
    /// @param parent 父作用域（可为空）
    /// @return 创建的对象指针，失败返回 nullptr
    Object* addObject(StringView type, Object* parent=nullptr);

    /// @brief 移除对象
    /// @param object 要移除的对象
    /// @return 错误码
    errc_t removeObject(Object& object);

    /// @brief 重命名对象
    /// @param object 要重命名的对象
    /// @param name 新名称
    /// @return 错误码
    errc_t renameObject(Object& object, StringView name);

private:
    ModeratorObserverMultiplexer observers_{};   ///< 事件监听者聚合（不拥有）
};


/*! @} */

AST_NAMESPACE_END
