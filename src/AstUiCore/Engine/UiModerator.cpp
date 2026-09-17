///
/// @file      UiModerator.cpp
/// @brief     引擎总控
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

#include "UiModerator.hpp"
#include "AstEngine/Moderator.hpp"
#include "AstEngine/ModeratorObserver.hpp"

AST_NAMESPACE_BEGIN

// 引擎事件桥接适配器：非 QObject，仅在 UiModerator 内部使用。
// 通过组合而非继承的方式实现观察者，把引擎通知转发给拥有者的 Qt 信号。
class UiModerator::Observer : public ModeratorObserver
{
public:
    explicit Observer(UiModerator& owner)
        : owner_(owner)
    {
    }

    void onObjectAdded(Object& obj) override { emit owner_.objectAdded(obj); }

    void onBeforeObjectRemove(Object& obj) override { emit owner_.beforeObjectRemove(obj); }

    void onObjectRemoved(Object& obj) override { emit owner_.objectRemoved(obj); }

    void onBeforeObjectRename(Object& obj, StringView oldName, StringView newName) override
    {
        emit owner_.beforeObjectRename(obj, oldName, newName);
    }

    void onObjectRenamed(Object& obj, StringView oldName, StringView newName) override
    {
        emit owner_.objectRenamed(obj, oldName, newName);
    }

private:
    UiModerator& owner_;
};

UiModerator::UiModerator(QObject* parent)
    : QObject(parent)
    , observer_(new Observer(*this))
{
    // 向引擎总控注册观察者，接收对象增删改名事件
    Moderator::Instance().addObserver(observer_);
}

UiModerator::~UiModerator()
{
    // 主动注销；Observer 基类析构也会兜底自动注销（幂等）
    Moderator::Instance().removeObserver(observer_);
    delete observer_;
    observer_ = nullptr;
}

UiModerator& UiModerator::Instance()
{
    static UiModerator instance;
    return instance;
}

AST_NAMESPACE_END
