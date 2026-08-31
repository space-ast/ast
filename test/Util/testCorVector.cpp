///
/// @file      testCorVector.cpp
/// @brief     CorVector 单元测试（gtest）
/// @details
/// @author    axel
/// @date      2026-08-26
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

#include "ast/CorVector.hpp"
#include "ast/Test.h"

#include <string>
#include <vector>

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
// 测试夹具
// ================================================================

class CorVectorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Tracked::reset();
    }
};


// ================================================================
// 1. 默认构造 / 空状态
// ================================================================

TEST_F(CorVectorTest, DefaultConstruction)
{
    CorVector<int> v;

    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_EQ(v.data(), nullptr);
    EXPECT_TRUE(v.is_borrowed());   // 无存储：内部语义为"借用"状态
    EXPECT_FALSE(v.is_owned());
}


// ================================================================
// 2. 带大小构造
// ================================================================

TEST_F(CorVectorTest, SizeConstruction)
{
    CorVector<int> v(5);

    EXPECT_EQ(v.size(), 5u);
    EXPECT_GE(v.capacity(), 5u);
    EXPECT_TRUE(v.is_owned());
}

TEST_F(CorVectorTest, SizeValueConstruction)
{
    CorVector<int> v(4, 7);

    EXPECT_EQ(v.size(), 4u);
    for (size_t i = 0; i < 4; ++i)
        EXPECT_EQ(v[i], 7);
}


// ================================================================
// 3. 迭代器/初始化列表/区间构造
// ================================================================

TEST_F(CorVectorTest, InitializerListConstruction)
{
    CorVector<int> v{1, 2, 3, 4, 5};

    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[4], 5);
}

TEST_F(CorVectorTest, RangeConstruction)
{
    std::vector<int> src = {10, 20, 30};
    CorVector<int> v(src.begin(), src.end());

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[2], 30);
}


// ================================================================
// 4. 拷贝构造 / 拷贝赋值（均为深拷贝）
// ================================================================

TEST_F(CorVectorTest, CopyConstructor)
{
    CorVector<int> a{1, 2, 3};
    CorVector<int> b(a);

    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b[0], 1);

    a[0] = 99;
    EXPECT_EQ(b[0], 1);   // 深拷贝：修改 a 不影响 b
}

TEST_F(CorVectorTest, CopyAssignment)
{
    CorVector<int> a{1, 2, 3};
    CorVector<int> b;
    b = a;

    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b[2], 3);

    a[0] = 99;
    EXPECT_EQ(b[0], 1);   // 深拷贝

    b = b;                // 自赋值不崩溃
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b[1], 2);
}

TEST_F(CorVectorTest, CopyAssignmentToNonEmpty)
{
    CorVector<int> a{1, 2, 3, 4, 5};
    CorVector<int> b{9, 9};
    b = a;

    EXPECT_EQ(b.size(), 5u);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[4], 5);
}


// ================================================================
// 5. 移动构造 / 移动赋值
// ================================================================

TEST_F(CorVectorTest, MoveConstructor)
{
    CorVector<int> a{1, 2, 3};
    CorVector<int> b(std::move(a));

    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b[2], 3);
    EXPECT_EQ(a.size(), 0u);   // 移动后源对象重置为空
}

TEST_F(CorVectorTest, MoveAssignment)
{
    CorVector<int> a{1, 2, 3};
    CorVector<int> b{9, 9};
    b = std::move(a);

    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(a.size(), 0u);
}

TEST_F(CorVectorTest, SelfMoveAssignment)
{
    CorVector<int> a{1, 2, 3};
    a = std::move(a);          // 自移动赋值不应崩溃

    a = CorVector<int>();
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.size(), 0u);
}


// ================================================================
// 6. 元素访问
// ================================================================

TEST_F(CorVectorTest, SubscriptReadWrite)
{
    CorVector<int> v{5, 6, 7};
    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(v[2], 7);

    v[1] = 100;
    EXPECT_EQ(v[1], 100);
}

TEST_F(CorVectorTest, AtBounds)
{
    CorVector<int> v{5, 6, 7, 8};

    EXPECT_EQ(v.at(0), 5);
    EXPECT_EQ(v.at(3), 8);
    EXPECT_THROW(v.at(4), std::out_of_range);
    EXPECT_THROW(v.at(100), std::out_of_range);

    const CorVector<int>& cv = v;
    EXPECT_EQ(cv.at(1), 6);
    EXPECT_THROW(cv.at(4), std::out_of_range);
}

TEST_F(CorVectorTest, FrontBack)
{
    CorVector<int> v{1, 2, 3};
    EXPECT_EQ(v.front(), 1);
    EXPECT_EQ(v.back(), 3);

    const CorVector<int>& cv = v;
    EXPECT_EQ(cv.front(), 1);
    EXPECT_EQ(cv.back(), 3);
}

TEST_F(CorVectorTest, DataAccess)
{
    CorVector<int> v{1, 2, 3};
    EXPECT_NE(v.data(), nullptr);
    EXPECT_EQ(v.data(), v.begin());

    const CorVector<int>& cv = v;
    EXPECT_EQ(cv.data(), v.data());
}


// ================================================================
// 7. 迭代器
// ================================================================

TEST_F(CorVectorTest, Iterators)
{
    CorVector<int> v{1, 2, 3, 4, 5};

    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it)
        sum += *it;
    EXPECT_EQ(sum, 15);

    int rsum = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it)
        rsum += *it;
    EXPECT_EQ(rsum, 15);

    const CorVector<int>& cv = v;
    sum = 0;
    for (auto it = cv.begin(); it != cv.end(); ++it)
        sum += *it;
    EXPECT_EQ(sum, 15);
}

TEST_F(CorVectorTest, EmptyIterators)
{
    CorVector<int> v;
    EXPECT_EQ(v.begin(), v.end());
}


// ================================================================
// 8. 容量
// ================================================================

TEST_F(CorVectorTest, ReserveGrow)
{
    CorVector<int> v{1, 2, 3};
    size_t oldCap = v.capacity();

    v.reserve(100);
    EXPECT_GE(v.capacity(), 100u);
    EXPECT_GT(v.capacity(), oldCap);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[2], 3);
}

TEST_F(CorVectorTest, ReserveNoOp)
{
    CorVector<int> v{1, 2, 3};
    size_t oldCap = v.capacity();

    v.reserve(2);             // 比当前容量小
    EXPECT_EQ(v.capacity(), oldCap);
    EXPECT_EQ(v.size(), 3u);
}

TEST_F(CorVectorTest, ShrinkToFit)
{
    CorVector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
    v.resize(3);

    EXPECT_EQ(v.size(), 3u);
    EXPECT_GT(v.capacity(), 3u);

    v.shrink_to_fit();
    EXPECT_EQ(v.capacity(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[2], 3);
}

TEST_F(CorVectorTest, ShrinkToFitEmpty)
{
    CorVector<int> v{1, 2, 3};
    v.clear();
    v.shrink_to_fit();

    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_TRUE(v.empty());
}

TEST_F(CorVectorTest, MaxSize)
{
    CorVector<int> v;
    EXPECT_GT(v.max_size(), v.size());
}


// ================================================================
// 9. 修改器 — push_back / pop_back
// ================================================================

TEST_F(CorVectorTest, PushBackGrowth)
{
    CorVector<int> v;
    for (int i = 0; i < 100; ++i)
        v.push_back(i * 2);

    EXPECT_EQ(v.size(), 100u);
    EXPECT_GE(v.capacity(), 100u);
    EXPECT_EQ(v[0], 0);
    EXPECT_EQ(v[50], 100);
    EXPECT_EQ(v[99], 198);
}

TEST_F(CorVectorTest, PopBack)
{
    CorVector<int> v{1, 2, 3};
    v.pop_back();

    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
}


// ================================================================
// 10. 修改器 — resize
// ================================================================

TEST_F(CorVectorTest, ResizeGrow)
{
    CorVector<int> v{1, 2};
    v.resize(5);

    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[4], 0);       // 新元素被默认构造
}

TEST_F(CorVectorTest, ResizeGrowWithValue)
{
    CorVector<int> v{1, 2};
    v.resize(5, 9);

    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 9);
    EXPECT_EQ(v[4], 9);
}

TEST_F(CorVectorTest, ResizeShrink)
{
    CorVector<int> v{10, 20, 30, 40};
    v.resize(2);

    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 20);
}

TEST_F(CorVectorTest, ResizeToZero)
{
    CorVector<int> v{1, 2, 3};
    v.resize(0);

    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.empty());

    v.resize(3);              // 可复用
    EXPECT_EQ(v.size(), 3u);
}


// ================================================================
// 11. 修改器 — insert / erase
// ================================================================

TEST_F(CorVectorTest, InsertSingle)
{
    CorVector<int> v{1, 3};
    auto it = v.insert(v.begin() + 1, 2);

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(*it, 2);
}

TEST_F(CorVectorTest, InsertCount)
{
    CorVector<int> v{1, 2, 5};
    auto it = v.insert(v.begin() + 2, 2, 3);

    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 3);
    EXPECT_EQ(v[4], 5);
    EXPECT_EQ(*it, 3);
}

TEST_F(CorVectorTest, InsertRange)
{
    CorVector<int> v{1, 4, 5};
    std::vector<int> extra = {2, 3};
    auto it = v.insert(v.begin() + 1, extra.begin(), extra.end());

    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 4);
    EXPECT_EQ(v[4], 5);
    EXPECT_EQ(*it, 2);
}

TEST_F(CorVectorTest, EraseSingle)
{
    CorVector<int> v{10, 20, 30, 40};
    auto it = v.erase(v.begin() + 1);

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[1], 30);
    EXPECT_EQ(v[2], 40);
    EXPECT_EQ(*it, 30);
}

TEST_F(CorVectorTest, EraseRange)
{
    CorVector<int> v{1, 2, 3, 4, 5};
    auto it = v.erase(v.begin() + 1, v.begin() + 3);

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 4);
    EXPECT_EQ(v[2], 5);
    EXPECT_EQ(*it, 4);
}


// ================================================================
// 12. 修改器 — clear / swap / 列表赋值
// ================================================================

TEST_F(CorVectorTest, ClearKeepsCapacity)
{
    CorVector<int> v{1, 2, 3, 4, 5};
    size_t cap = v.capacity();

    v.clear();
    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.capacity(), cap);
    EXPECT_TRUE(v.is_owned());

    v.push_back(42);
    EXPECT_EQ(v[0], 42);
    EXPECT_EQ(v.capacity(), cap);   // 复用已有容量，不重新分配
}

TEST_F(CorVectorTest, Swap)
{
    CorVector<int> a{1, 2, 3};
    CorVector<int> b{9, 9};

    a.swap(b);

    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(a[0], 9);
    EXPECT_EQ(b.size(), 3u);
    EXPECT_EQ(b[0], 1);
}

TEST_F(CorVectorTest, InitializerListAssignment)
{
    CorVector<int> v{9, 9, 9};
    v = {1, 2, 3};

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[2], 3);
}


// ================================================================
// 13. 借用内存 — 共享读写（核心语义）
// ================================================================

TEST_F(CorVectorTest, BorrowShareMemory)
{
    int buf[] = {10, 20, 30};

    CorVector<int> v;
    v.borrow_from(buf, 3);
    CorVector<int> w;
    w.borrow_from(buf, 3);

    EXPECT_TRUE(v.is_borrowed());
    EXPECT_TRUE(w.is_borrowed());
    EXPECT_FALSE(v.is_owned());

    // 共享读
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(w[2], 30);

    // 共享写：修改 v 的内容反映到 w 和底层 buf
    v[0] = 999;
    EXPECT_EQ(w[0], 999);
    EXPECT_EQ(buf[0], 999);
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(w.size(), 3u);
}


// ================================================================
// 14. Copy-On-Resize/Reserve — 扩容时与借用内存分离
// ================================================================

TEST_F(CorVectorTest, COWOnResizeGrow)
{
    int buf[] = {10, 20, 30};

    CorVector<int> v;
    v.borrow_from(buf, 3);
    v.resize(5);

    EXPECT_TRUE(v.is_owned());        // 已分离为自持内存
    EXPECT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[2], 30);
    EXPECT_EQ(v[3], 0);               // 新元素默认构造

    v[0] = 777;
    EXPECT_EQ(buf[0], 10);            // 原始缓冲不受影响
}

TEST_F(CorVectorTest, COWOnPushBack)
{
    int buf[] = {1, 2};

    CorVector<int> v;
    v.borrow_from(buf, 2);
    v.push_back(3);

    EXPECT_TRUE(v.is_owned());
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);

    EXPECT_EQ(buf[0], 1);             // 原始缓冲不变
    EXPECT_EQ(buf[1], 2);
}

TEST_F(CorVectorTest, COWOnReserve)
{
    int buf[] = {5, 6, 7};

    CorVector<int> v;
    v.borrow_from(buf, 3);
    v.reserve(10);

    EXPECT_TRUE(v.is_owned());
    EXPECT_EQ(v.size(), 3u);
    EXPECT_GE(v.capacity(), 10u);
    EXPECT_EQ(v[0], 5);
    EXPECT_EQ(v[2], 7);
}

TEST_F(CorVectorTest, ReserveZeroStaysBorrowed)
{
    int buf[] = {5, 6, 7};

    CorVector<int> v;
    v.borrow_from(buf, 3);
    v.reserve(0);

    EXPECT_TRUE(v.is_borrowed());     // reserve(0) 为无操作，保持借用
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 5);
}


// ================================================================
// 15. 一个借用对象分离后，另一个仍共享底层缓冲
// ================================================================

TEST_F(CorVectorTest, OneDetachOtherStillShares)
{
    int buf[] = {1, 2, 3};

    CorVector<int> a;
    a.borrow_from(buf, 3);
    CorVector<int> b;
    b.borrow_from(buf, 3);

    a.push_back(4);                   // a 分离为自持内存

    EXPECT_TRUE(a.is_owned());
    EXPECT_TRUE(b.is_borrowed());
    EXPECT_EQ(a.size(), 4u);
    EXPECT_EQ(b.size(), 3u);

    b[1] = 222;                       // b 仍写入底层 buf
    EXPECT_EQ(buf[1], 222);
    EXPECT_EQ(a[1], 2);               // a 已分离，不受影响
}


// ================================================================
// 16. 非 POD 类型支持
// ================================================================

TEST_F(CorVectorTest, NonPodString)
{
    CorVector<std::string> v;
    v.push_back("hello");
    v.push_back("world");

    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0], "hello");
    EXPECT_EQ(v[1], "world");

    CorVector<std::string> copy(v);
    copy[0] = "changed";
    EXPECT_EQ(v[0], "hello");         // 深拷贝
}


// ================================================================
// 17. 生命周期：借用 + reserve 触发的是拷贝而非移动
// ================================================================

TEST_F(CorVectorTest, BorrowReserveCopiesNotMoves)
{
    Tracked buf[2];
    buf[0].value = 10;
    buf[1].value = 20;
    Tracked::reset();                 // 在构造 buf 后再归零计数

    {
        CorVector<Tracked> v;
        v.borrow_from(buf, 2);
        v.reserve(4);                 // 复制借用数据 → 恰好 2 次拷贝构造

        EXPECT_EQ(v[0].value, 10);
        EXPECT_EQ(v[1].value, 20);
        EXPECT_EQ(Tracked::ctorCount, 2);
    }

    EXPECT_EQ(Tracked::dtorCount, 2); // v 的 2 个拷贝元素被析构
}


// ================================================================
// main
// ================================================================

GTEST_MAIN()
