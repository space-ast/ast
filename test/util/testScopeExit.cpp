/// @file      testScopeExit.cpp
/// @brief
/// @details   ~
/// @author    axel
/// @date      2026-10-09
/// @copyright 版权所有 (C) 2026-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/ScopeExit.hpp"
#include "ast/AstTestMacro.h"
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

AST_USING_NAMESPACE

namespace {

    /// 可具名的函数对象，用于静态检查守卫的拷贝/移动属性
    struct Counter
    {
        int* count{nullptr};
        void operator()() const { ++(*count); }
    };

} // namespace

// 离开作用域时自动执行一次
TEST(ScopeExit, InvokesOnScopeExit)
{
    int count = 0;
    {
        auto guard = aScopeExit([&count] { ++count; });
        EXPECT_TRUE(guard.isActive());
        EXPECT_TRUE(static_cast<bool>(guard));
        EXPECT_EQ(count, 0);
    }
    EXPECT_EQ(count, 1);
}

// 提前返回时仍然执行
TEST(ScopeExit, InvokesOnEarlyReturn)
{
    int count = 0;
    auto run = [&count](bool early) {
        auto guard = aScopeExit([&count] { ++count; });
        if (early)
            return;
        ++count;
    };
    run(true);
    EXPECT_EQ(count, 1);   // 提前返回：只执行了清理动作
    run(false);
    EXPECT_EQ(count, 3);   // 正常路径：函数体与清理动作各一次
}

// 抛出异常时仍然执行
TEST(ScopeExit, InvokesOnException)
{
    int count = 0;
    try {
        auto guard = aScopeExit([&count] { ++count; });
        throw std::runtime_error("scope exit");
    } catch (const std::runtime_error&) {
    }
    EXPECT_EQ(count, 1);
}

// release() 放弃清理动作
TEST(ScopeExit, ReleaseCancels)
{
    int count = 0;
    {
        auto guard = aScopeExit([&count] { ++count; });
        guard.release();
        EXPECT_FALSE(guard.isActive());
    }
    EXPECT_EQ(count, 0);
}

// invoke() 立即执行，且总共只执行一次
TEST(ScopeExit, InvokeRunsOnce)
{
    int count = 0;
    {
        auto guard = aScopeExit([&count] { ++count; });
        guard.invoke();
        EXPECT_EQ(count, 1);
        EXPECT_FALSE(guard.isActive());
        guard.invoke();     // 已解除，应为空操作
        guard.release();    // 已解除，应为空操作
    }
    EXPECT_EQ(count, 1);
}

// 移动转移清理动作的所有权，被移动者不再执行
TEST(ScopeExit, MoveTransfersAction)
{
    int count = 0;
    {
        auto first = aScopeExit([&count] { ++count; });
        auto second = std::move(first);
        EXPECT_FALSE(first.isActive());
        EXPECT_TRUE(second.isActive());
    }
    EXPECT_EQ(count, 1);
}

// 同一作用域内的多个守卫按构造的逆序执行
TEST(ScopeExit, RunsInReverseOrder)
{
    std::vector<int> order;
    {
        auto first = aScopeExit([&order] { order.push_back(1); });
        auto second = aScopeExit([&order] { order.push_back(2); });
        EXPECT_TRUE(first.isActive() && second.isActive());
    }
    ASSERT_EQ(order.size(), 2u);
    EXPECT_EQ(order[0], 2);
    EXPECT_EQ(order[1], 1);
}

// 类型擦除用法：具名守卫可以存入容器
TEST(ScopeExit, TypeErasedInContainer)
{
    int count = 0;
    std::vector<ScopeExit<std::function<void()>>> guards;
    guards.emplace_back([&count] { ++count; });
    guards.emplace_back([&count] { count += 10; });
    EXPECT_EQ(count, 0);
    guards.clear();          // 逆序析构：先 count += 10，再 ++count
    EXPECT_EQ(count, 11);
}

// 守卫不可拷贝，但可以移动
static_assert(!std::is_copy_constructible<ScopeExit<Counter>>::value, "ScopeExit 不应可拷贝");
static_assert(std::is_move_constructible<ScopeExit<Counter>>::value, "ScopeExit 应可移动");

GTEST_MAIN()
