///
/// @file      testAny.cpp
/// @brief     Any 单元测试（内置 gtest 框架）
/// @details   覆盖 Any 的类型擦除语义：构造/赋值/移动、emplace、any_cast（值/引用/指针）、
///            make_any、reset、swap、bad_any_cast，以及拷贝/析构计数
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

#include "ast/Any.hpp"
#include "ast/Test.h"

#include <string>

AST_USING_NAMESPACE


// ================================================================
// Tracked — 跟踪构造/析构次数的辅助类型
// ================================================================

struct Tracked
{
    int value;
    static int ctorCount;
    static int dtorCount;
    static void reset() { ctorCount = 0; dtorCount = 0; }

    Tracked(int v = 0)          : value(v)  { ++ctorCount; }
    Tracked(const Tracked& o)   : value(o.value) { ++ctorCount; }
    Tracked(Tracked&& o) noexcept : value(o.value) { o.value = -1; ++ctorCount; }
    ~Tracked() { ++dtorCount; }
};

int Tracked::ctorCount = 0;
int Tracked::dtorCount = 0;


// ================================================================
// 1. 默认构造 / 空状态
// ================================================================

TEST(AnyTest, DefaultConstruction)
{
    Any a;

    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a.type(), typeid(void));
}

TEST(AnyTest, ResetEmptyIsNoOp)
{
    Any a;
    a.reset();
    EXPECT_FALSE(a.has_value());
}


// ================================================================
// 2. 从任意类型构造
// ================================================================

TEST(AnyTest, ConstructFromInt)
{
    Any a = 42;

    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(int));
    EXPECT_EQ(any_cast<int>(a), 42);
}

TEST(AnyTest, ConstructFromString)
{
    std::string s("hello");
    Any a(s);

    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(any_cast<std::string>(a), "hello");
}

TEST(AnyTest, ConstructFromMoveString)
{
    std::string s("world");
    Any a(std::move(s));

    EXPECT_EQ(any_cast<std::string>(a), "world");
}


// ================================================================
// 3. 拷贝 / 移动
// ================================================================

TEST(AnyTest, CopyConstructorDeepCopy)
{
    Any a = 123;
    Any b(a);

    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(any_cast<int>(b), 123);

    any_cast<int&>(b) = 999;
    EXPECT_EQ(any_cast<int>(a), 123);   // 深拷贝：改 b 不影响 a
}

TEST(AnyTest, CopyAssignmentDeepCopy)
{
    Any a = std::string("deep");
    Any b;
    b = a;

    EXPECT_EQ(any_cast<std::string>(b), "deep");
    any_cast<std::string&>(b) = "changed";
    EXPECT_EQ(any_cast<std::string>(a), "deep");

    b = b;                              // 自赋值不崩溃
    EXPECT_EQ(any_cast<std::string>(b), "changed");
}

TEST(AnyTest, CopyAssignFromEmptyMakesEmpty)
{
    Any a = 5;
    Any b;
    a = b;

    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, MoveConstructor)
{
    Any a = std::string("move");
    Any b(std::move(a));

    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(any_cast<std::string>(b), "move");
    EXPECT_FALSE(a.has_value());        // 移动后源为空
}

TEST(AnyTest, MoveAssignment)
{
    Any a = 7;
    Any b = 9;
    b = std::move(a);

    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(any_cast<int>(b), 7);
    EXPECT_FALSE(a.has_value());
}

TEST(AnyTest, SelfMoveAssignment)
{
    Any a = 3;
    a = std::move(a);                   // 自移动不崩溃
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(any_cast<int>(a), 3);
}


// ================================================================
// 4. emplace
// ================================================================

TEST(AnyTest, EmplaceReplacesValue)
{
    Any a = 1;
    a.emplace<std::string>(3, 'x');     // 用 3 个 'x' 构造

    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(any_cast<std::string>(a), "xxx");
}

TEST(AnyTest, EmplaceReturnsReference)
{
    Any a;
    std::string& r = a.emplace<std::string>("ref");
    r.push_back('!');

    EXPECT_EQ(any_cast<std::string>(a), "ref!");
}


// ================================================================
// 5. any_cast 值 / 引用 / 指针
// ================================================================

TEST(AnyTest, AnyCastValue)
{
    Any a = 99;
    EXPECT_EQ(any_cast<int>(a), 99);

    const Any& ca = a;
    EXPECT_EQ(any_cast<int>(ca), 99);
}

TEST(AnyTest, AnyCastReference)
{
    Any a = std::string("mutable");
    std::string& r = any_cast<std::string&>(a);
    r += "!!";
    EXPECT_EQ(any_cast<std::string>(a), "mutable!!");

    const Any& ca = a;
    const std::string& cr = any_cast<const std::string&>(ca);
    EXPECT_EQ(cr, "mutable!!");
}

TEST(AnyTest, AnyCastPointerNonNullAndNull)
{
    Any a = 3.14;
    Any empty;

    EXPECT_NE(any_cast<double>(&a), nullptr);
    EXPECT_EQ(any_cast<double>(&empty), nullptr);
    EXPECT_EQ(any_cast<int>(&a), nullptr);      // 类型不匹配
    EXPECT_EQ(any_cast<int>(static_cast<const Any*>(&a)), nullptr);

    double* p = any_cast<double>(&a);
    EXPECT_EQ(*p, 3.14);
}

TEST(AnyTest, AnyCastMismatchThrows)
{
    Any a = 5;
    EXPECT_THROW(any_cast<std::string>(a), bad_any_cast);
    EXPECT_THROW(any_cast<std::string>(static_cast<const Any&>(a)), bad_any_cast);
}

TEST(AnyTest, AnyCastEmptyThrows)
{
    Any empty;
    EXPECT_THROW(any_cast<int>(empty), bad_any_cast);
}


// ================================================================
// 6. make_any
// ================================================================

TEST(AnyTest, MakeAny)
{
    Any a = make_any<std::string>("built");

    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(any_cast<std::string>(a), "built");
}


// ================================================================
// 7. swap
// ================================================================

TEST(AnyTest, Swap)
{
    Any a = 1;
    Any b = std::string("two");

    a.swap(b);

    EXPECT_EQ(a.type(), typeid(std::string));
    EXPECT_EQ(any_cast<std::string>(a), "two");
    EXPECT_EQ(any_cast<int>(b), 1);
}


// ================================================================
// 8. 生命周期 — 拷贝/析构计数（验证无泄漏、副本独立）
// ================================================================

TEST(AnyTest, LifecycleCounts)
{
    Tracked::reset();

    {
        Tracked t(10);                   // +1 构造
        Any a = t;                       // 拷贝构造 → +1
        Any b(a);                        // 拷贝构造 → +1
        EXPECT_EQ(any_cast<Tracked&>(a).value, 10);   // 引用读取，不触发拷贝
        EXPECT_EQ(any_cast<Tracked&>(b).value, 10);
        EXPECT_EQ(Tracked::ctorCount, 3);   // t + a(拷贝) + b(拷贝)
    }                                    // b、a、t 逆序析构
    EXPECT_EQ(Tracked::dtorCount, 3);   // b、a 的两份副本 + t
}


// ================================================================
// main
// ================================================================

GTEST_MAIN()
