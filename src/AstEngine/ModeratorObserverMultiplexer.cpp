///
/// @file      ModeratorObserverMultiplexer.cpp
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

#include "ModeratorObserverMultiplexer.hpp"
#include <algorithm>

AST_NAMESPACE_BEGIN

ModeratorObserverMultiplexer::~ModeratorObserverMultiplexer()
{
    // 复用器先析构时，切断每个子订阅者指向自身的反向引用，
    // 否则子订阅者后析构仍会访问已析构的复用器（悬垂）。
    for (auto* o : observers_)
        o->removeSource(this);
}

void ModeratorObserverMultiplexer::addObserver(ModeratorObserver* observer)
{
    if (observer != nullptr)
    {
        observer->addSource(this);
        observers_.push_back(observer);
    }
}

void ModeratorObserverMultiplexer::removeObserver(ModeratorObserver* observer)
{
    auto it = std::find(observers_.begin(), observers_.end(), observer);
    if (it != observers_.end())
    {
        (*it)->removeSource(this);   // 解除双向注册
        observers_.erase(it);
    }
}

void ModeratorObserverMultiplexer::clear()
{
    for (auto* o : observers_)
        o->removeSource(this);
    observers_.clear();
}

void ModeratorObserverMultiplexer::onObjectAdded(Object& obj)
{
    for (auto* o : observers_)
        o->onObjectAdded(obj);
}

void ModeratorObserverMultiplexer::onBeforeObjectRemove(Object& obj)
{
    for (auto* o : observers_)
        o->onBeforeObjectRemove(obj);
}

void ModeratorObserverMultiplexer::onObjectRemoved(Object& obj)
{
    for (auto* o : observers_)
        o->onObjectRemoved(obj);
}

void ModeratorObserverMultiplexer::onBeforeObjectRename(Object& obj, StringView oldName, StringView newName)
{
    for (auto* o : observers_)
        o->onBeforeObjectRename(obj, oldName, newName);
}

void ModeratorObserverMultiplexer::onObjectRenamed(Object& obj, StringView oldName, StringView newName)
{
    for (auto* o : observers_)
        o->onObjectRenamed(obj, oldName, newName);
}

AST_NAMESPACE_END
