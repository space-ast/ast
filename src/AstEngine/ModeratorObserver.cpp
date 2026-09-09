///
/// @file      ModeratorObserver.cpp
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

#include "ModeratorObserver.hpp"
#include "ModeratorObserverMultiplexer.hpp"
#include <algorithm>

AST_NAMESPACE_BEGIN

ModeratorObserver::~ModeratorObserver()
{
    // 先搬空自身再逐一注销：removeObserver 会调用 removeSource(this)，
    // 此时 sources_ 已空、find 不到 → 无副作用，同时避免遍历中修改容器的迭代器失效。
    auto sources = std::move(sources_);
    for (auto& src : sources)
    {
        src->removeObserver(this);
    }
}

void ModeratorObserver::addSource(ModeratorObserverMultiplexer* src)
{
    sources_.push_back(src);
}

void ModeratorObserver::removeSource(ModeratorObserverMultiplexer* src)
{
    auto it = std::find(sources_.begin(), sources_.end(), src);
    if (it != sources_.end())
        sources_.erase(it);
}

AST_NAMESPACE_END
