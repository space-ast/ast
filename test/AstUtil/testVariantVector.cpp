///
/// @file      testVariantVector.cpp
/// @brief     VariantVector 单元测试（gtest）
/// @details
/// @author    axel
/// @date      2026-07-06
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

#include "ast/VariantVector.hpp"

#include <gtest/gtest.h>
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

class VariantVectorTest : public ::testing::Test
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

TEST_F(VariantVectorTest, DefaultConstruction)
{
    VariantVector v;

    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_FALSE(v.hasType());
    EXPECT_EQ(v.elementSize(), 0u);
    EXPECT_EQ(v.ops(), nullptr);
    EXPECT_EQ(v.data(), nullptr);
}


// ================================================================
// 2. resize 基础类型
// ================================================================

TEST_F(VariantVectorTest, ResizeDouble)
{
    VariantVector v;
    v.resize<double>(5);

    EXPECT_EQ(v.size(), 5u);
    EXPECT_TRUE(v.hasType());
    EXPECT_EQ(v.elementSize(), sizeof(double));
    EXPECT_NE(v.ops(), nullptr);

    v.as<double>()[0] = 1.5;
    v.as<double>()[1] = 2.5;
    v.as<double>()[2] = 3.5;
    v.as<double>()[3] = 4.5;
    v.as<double>()[4] = 5.5;

    EXPECT_DOUBLE_EQ(v.as<double>()[0], 1.5);
    EXPECT_DOUBLE_EQ(v.as<double>()[4], 5.5);
}

TEST_F(VariantVectorTest, ResizeInt)
{
    VariantVector v;
    v.resize<int>(3);

    v.as<int>()[0] = 10;
    v.as<int>()[1] = 20;
    v.as<int>()[2] = 30;

    EXPECT_EQ(v.as<int>()[0], 10);
    EXPECT_EQ(v.as<int>()[1], 20);
    EXPECT_EQ(v.as<int>()[2], 30);
}


// ================================================================
// 3. resize(n, value) 填充
// ================================================================

TEST_F(VariantVectorTest, ResizeWithValue)
{
    VariantVector v;
    v.resize<double>(4, 3.14);

    EXPECT_EQ(v.size(), 4u);
    EXPECT_DOUBLE_EQ(v.as<double>()[0], 3.14);
    EXPECT_DOUBLE_EQ(v.as<double>()[3], 3.14);
}


// ================================================================
// 4. at() 边界检查
// ================================================================

TEST_F(VariantVectorTest, AtOutOfRange)
{
    VariantVector v;
    v.resize<int>(3);

    EXPECT_NO_THROW(v.at<int>(0));
    EXPECT_NO_THROW(v.at<int>(2));
    EXPECT_THROW(v.at<int>(3), std::out_of_range);
    EXPECT_THROW(v.at<int>(100), std::out_of_range);
}

TEST_F(VariantVectorTest, AtReadWrite)
{
    VariantVector v;
    v.resize<double>(3);
    v.at<double>(0) = 1.0;
    v.at<double>(1) = 2.0;
    v.at<double>(2) = 3.0;

    EXPECT_DOUBLE_EQ(v.at<double>(0), 1.0);
    EXPECT_DOUBLE_EQ(v.at<double>(1), 2.0);
    EXPECT_DOUBLE_EQ(v.at<double>(2), 3.0);
}


// ================================================================
// 5. pushBack 扩容
// ================================================================

TEST_F(VariantVectorTest, PushBackPOD)
{
    VariantVector v;

    for (int i = 0; i < 100; ++i)
        v.pushBack<int>(i * 2);

    EXPECT_EQ(v.size(), 100u);
    EXPECT_GE(v.capacity(), 100u);
    EXPECT_EQ(v.as<int>()[0], 0);
    EXPECT_EQ(v.as<int>()[50], 100);
    EXPECT_EQ(v.as<int>()[99], 198);
}

TEST_F(VariantVectorTest, PushBackString)
{
    VariantVector v;

    v.pushBack<std::string>("hello");
    v.pushBack<std::string>("world");

    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v.as<std::string>()[0], "hello");
    EXPECT_EQ(v.as<std::string>()[1], "world");
}


// ================================================================
// 6. emplaceBack
// ================================================================

TEST_F(VariantVectorTest, EmplaceBack)
{
    VariantVector v;

    v.emplaceBack<std::string>("one");
    v.emplaceBack<std::string>("two");
    v.emplaceBack<std::string>("three");

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v.as<std::string>()[0], "one");
    EXPECT_EQ(v.as<std::string>()[1], "two");
    EXPECT_EQ(v.as<std::string>()[2], "three");
}


// ================================================================
// 7. 迭代器
// ================================================================

TEST_F(VariantVectorTest, IteratorDouble)
{
    VariantVector v;
    v.resize<double>(5);
    v.as<double>()[0] = 1.0;
    v.as<double>()[1] = 2.0;
    v.as<double>()[2] = 3.0;
    v.as<double>()[3] = 4.0;
    v.as<double>()[4] = 5.0;

    double sum = 0;
    for (double* it = v.begin<double>(); it != v.end<double>(); ++it)
        sum += *it;
    EXPECT_DOUBLE_EQ(sum, 15.0);
}

TEST_F(VariantVectorTest, ConstIterator)
{
    VariantVector v;
    v.resize<int>(3);
    v.as<int>()[0] = 10;
    v.as<int>()[1] = 20;
    v.as<int>()[2] = 30;

    const VariantVector& cv = v;
    int sum = 0;
    for (const int* it = cv.begin<int>(); it != cv.end<int>(); ++it)
        sum += *it;
    EXPECT_EQ(sum, 60);
}


// ================================================================
// 8. 拷贝
// ================================================================

TEST_F(VariantVectorTest, CopyConstructor)
{
    VariantVector v1;
    v1.resize<int>(3);
    v1.as<int>()[0] = 1;
    v1.as<int>()[1] = 2;
    v1.as<int>()[2] = 3;

    VariantVector v2(v1);

    ASSERT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2.as<int>()[0], 1);
    EXPECT_EQ(v2.as<int>()[1], 2);
    EXPECT_EQ(v2.as<int>()[2], 3);

    // 深拷贝验证
    v1.as<int>()[0] = 999;
    EXPECT_EQ(v2.as<int>()[0], 1);
}

TEST_F(VariantVectorTest, CopyAssignment)
{
    VariantVector v1;
    v1.resize<int>(3);
    v1.as<int>()[0] = 1;
    v1.as<int>()[1] = 2;
    v1.as<int>()[2] = 3;

    VariantVector v2;
    v2 = v1;

    ASSERT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2.as<int>()[2], 3);

    // 自赋值
    v2 = v2;
    EXPECT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2.as<int>()[1], 2);
}

TEST_F(VariantVectorTest, CopyEmptyVector)
{
    VariantVector v1;  // 无类型
    VariantVector v2(v1);

    EXPECT_FALSE(v2.hasType());
    EXPECT_EQ(v2.size(), 0u);
}


// ================================================================
// 9. 移动
// ================================================================

TEST_F(VariantVectorTest, MoveConstructor)
{
    VariantVector v1;
    v1.resize<int>(3);
    v1.as<int>()[0] = 1;
    v1.as<int>()[1] = 2;
    v1.as<int>()[2] = 3;

    VariantVector v2(std::move(v1));

    EXPECT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2.as<int>()[2], 3);

    // v1 恢复空状态
    EXPECT_FALSE(v1.hasType());
    EXPECT_EQ(v1.size(), 0u);
}

TEST_F(VariantVectorTest, MoveAssignment)
{
    VariantVector v1;
    v1.resize<int>(3);
    v1.as<int>()[0] = 10;
    v1.as<int>()[1] = 20;
    v1.as<int>()[2] = 30;

    VariantVector v2;
    v2 = std::move(v1);

    EXPECT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2.as<int>()[0], 10);
    EXPECT_FALSE(v1.hasType());
}


// ================================================================
// 10. clear
// ================================================================

TEST_F(VariantVectorTest, Clear)
{
    VariantVector v;
    v.resize<int>(5);
    v.as<int>()[0] = 1;

    v.clear();

    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.hasType());                  // 类型保留
    EXPECT_EQ(v.elementSize(), sizeof(int));    // elementSize 仍有效

    // 可继续使用
    v.pushBack<int>(42);
    EXPECT_EQ(v.as<int>()[0], 42);
}

TEST_F(VariantVectorTest, ClearEmpty)
{
    VariantVector v;
    v.clear();  // 空向量 clear 不 crash

    EXPECT_FALSE(v.hasType());
    EXPECT_EQ(v.size(), 0u);
}


// ================================================================
// 11. shrinkToFit
// ================================================================

TEST_F(VariantVectorTest, ShrinkToFit)
{
    VariantVector v;
    v.resize<int>(100);
    v.resize<int>(10);

    EXPECT_GT(v.capacity(), 10u);
    v.shrinkToFit();

    EXPECT_EQ(v.capacity(), 10u);
    EXPECT_EQ(v.as<int>()[9], 0);  // 数据正确
}

TEST_F(VariantVectorTest, ShrinkToFitEmpty)
{
    VariantVector v;
    v.resize<int>(10);
    v.clear();
    v.shrinkToFit();

    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_TRUE(v.hasType());  // 仍保留类型
}


// ================================================================
// 12. swap
// ================================================================

TEST_F(VariantVectorTest, Swap)
{
    VariantVector v1;
    v1.resize<int>(3);
    v1.as<int>()[0] = 1;
    v1.as<int>()[1] = 2;
    v1.as<int>()[2] = 3;

    VariantVector v2;
    v2.resize<double>(2);
    v2.as<double>()[0] = 1.5;
    v2.as<double>()[1] = 2.5;

    v1.swap(v2);

    EXPECT_EQ(v1.size(), 2u);
    EXPECT_DOUBLE_EQ(v1.as<double>()[0], 1.5);
    EXPECT_EQ(v1.elementSize(), sizeof(double));

    EXPECT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2.as<int>()[0], 1);
    EXPECT_EQ(v2.elementSize(), sizeof(int));
}


// ================================================================
// 13. fromVector
// ================================================================

TEST_F(VariantVectorTest, FromVector)
{
    std::vector<double> vec = {1.0, 2.0, 3.0, 4.0, 5.0};
    VariantVector v = VariantVector::fromVector(vec);

    EXPECT_EQ(v.size(), 5u);
    EXPECT_DOUBLE_EQ(v.as<double>()[0], 1.0);
    EXPECT_DOUBLE_EQ(v.as<double>()[4], 5.0);
}


// ================================================================
// 14. 非 POD 类型（std::string）
// ================================================================

TEST_F(VariantVectorTest, NonPodString)
{
    VariantVector v;
    v.resize<std::string>(3);
    v.as<std::string>()[0] = "hello";
    v.as<std::string>()[1] = "world";
    v.as<std::string>()[2] = "!";

    EXPECT_EQ(v.as<std::string>()[0], "hello");
    EXPECT_EQ(v.as<std::string>()[1], "world");
    EXPECT_EQ(v.as<std::string>()[2], "!");

    // 拷贝含 non-POD 的向量
    VariantVector v2(v);
    EXPECT_EQ(v2.as<std::string>()[0], "hello");
    EXPECT_EQ(v2.as<std::string>()[2], "!");

    // 修改 v 不影响 v2
    v.as<std::string>()[0] = "changed";
    EXPECT_EQ(v2.as<std::string>()[0], "hello");
}


// ================================================================
// 15. Tracked 类型（生命周期跟踪）
// ================================================================

TEST_F(VariantVectorTest, TrackedLifecycle)
{
    Tracked::reset();

    {
        VariantVector v;
        v.resize<Tracked>(5);
        EXPECT_EQ(Tracked::ctorCount, 5);
        EXPECT_EQ(Tracked::dtorCount, 0);

        v.clear();
        EXPECT_EQ(Tracked::dtorCount, 5);

        // 使用 emplaceBack 避免临时对象，计数更可靠
        v.emplaceBack<Tracked>(100);
        EXPECT_EQ(v.as<Tracked>()[0].value, 100);
    }
    // v 离开作用域，emplaceBack 的元素被析构
    EXPECT_EQ(Tracked::dtorCount, 6);
    EXPECT_EQ(Tracked::ctorCount, Tracked::dtorCount);  // 构造与析构平衡
}


// ================================================================
// 16. elementType 查询
// ================================================================

TEST_F(VariantVectorTest, ElementType)
{
    VariantVector v;
    EXPECT_EQ(v.elementType(), typeid(void));

    v.resize<double>(1);
    EXPECT_EQ(v.elementType(), typeid(double));

    // clear 保留类型
    v.clear();
    EXPECT_EQ(v.elementType(), typeid(double));
}


// ================================================================
// 17. 反复 resize 场景
// ================================================================

TEST_F(VariantVectorTest, RepeatedResize)
{
    VariantVector v;
    v.resize<int>(10);
    for (int i = 0; i < 10; ++i) v.as<int>()[i] = i;

    v.resize<int>(20);
    for (int i = 10; i < 20; ++i) v.as<int>()[i] = i * 10;

    EXPECT_EQ(v.size(), 20u);
    EXPECT_EQ(v.as<int>()[5], 5);
    EXPECT_EQ(v.as<int>()[15], 150);

    v.resize<int>(3);  // 缩容
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v.as<int>()[2], 2);
}


// ================================================================
// 18. as<T>() 类型校验
// ================================================================

TEST_F(VariantVectorTest, AsTypeCheck)
{
    VariantVector v;
    v.resize<double>(3);

    // 正确类型
    EXPECT_NO_THROW(v.as<double>());
    double* p = v.as<double>();
    p[0] = 1.0;
    EXPECT_DOUBLE_EQ(p[0], 1.0);

    // as<T>() 在空向量上返回 nullptr（accessor 不负责建立类型）
    VariantVector v2;
    EXPECT_EQ(v2.as<int>(), nullptr);
    EXPECT_FALSE(v2.hasType());
}

// ================================================================
// 19. asSpan 视图
// ================================================================

TEST_F(VariantVectorTest, AsSpan)
{
    VariantVector v;
    v.resize<int>(5);
    v.as<int>()[0] = 10;
    v.as<int>()[1] = 20;
    v.as<int>()[2] = 30;
    v.as<int>()[3] = 40;
    v.as<int>()[4] = 50;

    Span<int> s = v.asSpan<int>();
    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s[0], 10);
    EXPECT_EQ(s[4], 50);

    // 通过 Span 修改数据影响原 VariantVector
    s[0] = 100;
    EXPECT_EQ(v.as<int>()[0], 100);

    // const Span
    const VariantVector& cv = v;
    Span<const int> cs = cv.asSpan<int>();
    EXPECT_EQ(cs.size(), 5u);
    EXPECT_EQ(cs[0], 100);
    EXPECT_EQ(cs[4], 50);

    // 迭代
    int sum = 0;
    for (const int& x : cs)
        sum += x;
    EXPECT_EQ(sum, 100 + 20 + 30 + 40 + 50);
}

TEST_F(VariantVectorTest, AsSpanEmpty)
{
    VariantVector v;
    v.resize<double>(0);  // 建立类型但 size=0

    Span<double> s = v.asSpan<double>();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
    EXPECT_EQ(s.data(), v.data());
}

// ================================================================
// 20. reserve 测试
// ================================================================

TEST_F(VariantVectorTest, ReserveOnEmpty)
{
    VariantVector v;
    v.reserve<int>(10);

    EXPECT_TRUE(v.hasType());
    EXPECT_EQ(v.elementType(), typeid(int));
    EXPECT_EQ(v.size(), 0u);         // size 不受影响
    EXPECT_GE(v.capacity(), 10u);    // 分配了容量
    EXPECT_EQ(v.elementSize(), sizeof(int));
}

TEST_F(VariantVectorTest, ReserveGrow)
{
    VariantVector v;
    v.resize<int>(3);
    v.as<int>()[0] = 1;
    v.as<int>()[1] = 2;
    v.as<int>()[2] = 3;

    size_t oldCap = v.capacity();
    v.reserve<int>(100);

    EXPECT_GE(v.capacity(), 100u);
    EXPECT_GT(v.capacity(), oldCap);
    EXPECT_EQ(v.size(), 3u);          // size 不变
    EXPECT_EQ(v.as<int>()[0], 1);     // 原有数据保留
    EXPECT_EQ(v.as<int>()[1], 2);
    EXPECT_EQ(v.as<int>()[2], 3);
}

TEST_F(VariantVectorTest, ReserveNoOp)
{
    VariantVector v;
    v.resize<int>(10);
    size_t oldCap = v.capacity();

    v.reserve<int>(5);               // 比当前容量小
    EXPECT_EQ(v.capacity(), oldCap);  // 无变化
    EXPECT_EQ(v.size(), 10u);
}

TEST_F(VariantVectorTest, ReserveGrowNonPod)
{
    VariantVector v;
    v.resize<std::string>(3);
    v.as<std::string>()[0] = "alpha";
    v.as<std::string>()[1] = "beta";
    v.as<std::string>()[2] = "gamma";

    v.reserve<std::string>(100);

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v.as<std::string>()[0], "alpha");
    EXPECT_EQ(v.as<std::string>()[1], "beta");
    EXPECT_EQ(v.as<std::string>()[2], "gamma");
}


// ================================================================
// 21. at() const 版本
// ================================================================

TEST_F(VariantVectorTest, AtConst)
{
    VariantVector v;
    v.resize<int>(3);
    v.as<int>()[0] = 10;
    v.as<int>()[1] = 20;
    v.as<int>()[2] = 30;

    const VariantVector& cv = v;
    EXPECT_EQ(cv.at<int>(0), 10);
    EXPECT_EQ(cv.at<int>(1), 20);
    EXPECT_EQ(cv.at<int>(2), 30);
    EXPECT_THROW(cv.at<int>(3), std::out_of_range);
    EXPECT_THROW(cv.at<int>(100), std::out_of_range);
}


// ================================================================
// 22. 移动赋值到非空向量
// ================================================================

TEST_F(VariantVectorTest, MoveAssignmentToNonEmpty)
{
    VariantVector v1;
    v1.resize<int>(3);
    v1.as<int>()[0] = 1;
    v1.as<int>()[1] = 2;
    v1.as<int>()[2] = 3;

    VariantVector v2;
    v2.resize<double>(5);             // v2 已有不同类型的数据
    v2.as<double>()[0] = 99.9;

    v2 = std::move(v1);

    EXPECT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2.as<int>()[0], 1);    // v2 现在持有 v1 的数据
    EXPECT_EQ(v2.as<int>()[2], 3);
    EXPECT_FALSE(v1.hasType());       // v1 被清空
    EXPECT_EQ(v1.size(), 0u);
}


// ================================================================
// 23. 拷贝赋值：不同大小
// ================================================================

TEST_F(VariantVectorTest, CopyAssignmentDifferentSize)
{
    VariantVector v1;
    v1.resize<int>(5);
    for (int i = 0; i < 5; ++i) v1.as<int>()[i] = i * 10;

    VariantVector v2;
    v2.resize<int>(2);
    v2.as<int>()[0] = 100;
    v2.as<int>()[1] = 200;

    v2 = v1;  // 大的拷到小的

    EXPECT_EQ(v2.size(), 5u);
    EXPECT_EQ(v2.as<int>()[0], 0);
    EXPECT_EQ(v2.as<int>()[4], 40);

    // 反过来：小的拷到大的
    VariantVector v3;
    v3.resize<int>(10);
    v3 = v1;

    EXPECT_EQ(v3.size(), 5u);
    EXPECT_EQ(v3.as<int>()[4], 40);
}


// ================================================================
// 24. 迭代器：空但已设定类型
// ================================================================

TEST_F(VariantVectorTest, IteratorEmptyTyped)
{
    VariantVector v;
    v.resize<double>(0);  // 建立类型但 size=0

    EXPECT_EQ(v.begin<double>(), v.end<double>());

    const VariantVector& cv = v;
    EXPECT_EQ(cv.begin<double>(), cv.end<double>());
}


// ================================================================
// 25. fromVector 边界情况
// ================================================================

TEST_F(VariantVectorTest, FromVectorEmpty)
{
    std::vector<int> vec;
    VariantVector v = VariantVector::fromVector(vec);

    EXPECT_TRUE(v.hasType());
    EXPECT_EQ(v.elementType(), typeid(int));
    EXPECT_EQ(v.size(), 0u);
}

TEST_F(VariantVectorTest, FromVectorNonPod)
{
    std::vector<std::string> vec = {"hello", "world", "!"};
    VariantVector v = VariantVector::fromVector(vec);

    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v.as<std::string>()[0], "hello");
    EXPECT_EQ(v.as<std::string>()[1], "world");
    EXPECT_EQ(v.as<std::string>()[2], "!");

    // 深拷贝验证
    vec[0] = "changed";
    EXPECT_EQ(v.as<std::string>()[0], "hello");
}


// ================================================================
// 26. swap 边界情况
// ================================================================

TEST_F(VariantVectorTest, SwapWithEmpty)
{
    VariantVector v1;
    v1.resize<int>(3);
    v1.as<int>()[0] = 1;
    v1.as<int>()[1] = 2;
    v1.as<int>()[2] = 3;

    VariantVector v2;  // 无类型，空

    v1.swap(v2);

    EXPECT_FALSE(v1.hasType());
    EXPECT_EQ(v1.size(), 0u);

    EXPECT_EQ(v2.size(), 3u);
    EXPECT_EQ(v2.as<int>()[0], 1);
    EXPECT_EQ(v2.as<int>()[2], 3);
}

TEST_F(VariantVectorTest, SwapBothUntyped)
{
    VariantVector v1, v2;
    v1.swap(v2);

    EXPECT_FALSE(v1.hasType());
    EXPECT_FALSE(v2.hasType());
    EXPECT_EQ(v1.size(), 0u);
    EXPECT_EQ(v2.size(), 0u);
}


// ================================================================
// 27. emplaceBack 多参数构造
// ================================================================

struct MultiArg
{
    int a;
    double b;
    std::string c;

    MultiArg(int a_, double b_, const std::string& c_)
        : a(a_), b(b_), c(c_) {}
};

TEST_F(VariantVectorTest, EmplaceBackMultiArg)
{
    VariantVector v;
    v.emplaceBack<MultiArg>(1, 2.5, std::string("test"));
    v.emplaceBack<MultiArg>(3, 4.0, std::string("hello"));

    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v.as<MultiArg>()[0].a, 1);
    EXPECT_DOUBLE_EQ(v.as<MultiArg>()[0].b, 2.5);
    EXPECT_EQ(v.as<MultiArg>()[0].c, "test");
    EXPECT_EQ(v.as<MultiArg>()[1].a, 3);
    EXPECT_DOUBLE_EQ(v.as<MultiArg>()[1].b, 4.0);
    EXPECT_EQ(v.as<MultiArg>()[1].c, "hello");
}


// ================================================================
// 28. Tracked 生命周期：拷贝 / 移动 VariantVector
// ================================================================

TEST_F(VariantVectorTest, TrackedCopyLifecycle)
{
    Tracked::reset();

    VariantVector v1;
    v1.resize<Tracked>(3);
    v1.as<Tracked>()[0].value = 10;
    v1.as<Tracked>()[1].value = 20;
    v1.as<Tracked>()[2].value = 30;

    int ctorBefore = Tracked::ctorCount;
    int dtorBefore = Tracked::dtorCount;

    {
        VariantVector v2(v1);  // 拷贝构造
        EXPECT_EQ(v2.size(), 3u);
        EXPECT_EQ(v2.as<Tracked>()[0].value, 10);
        EXPECT_EQ(v2.as<Tracked>()[1].value, 20);
        EXPECT_EQ(v2.as<Tracked>()[2].value, 30);

        // 拷贝构造了 3 个元素
        EXPECT_EQ(Tracked::ctorCount - ctorBefore, 3);
    }
    // v2 析构，3 个元素被销毁
    EXPECT_EQ(Tracked::dtorCount - dtorBefore, 3);
}

TEST_F(VariantVectorTest, TrackedMoveLifecycle)
{
    Tracked::reset();

    VariantVector v1;
    v1.resize<Tracked>(3);
    v1.as<Tracked>()[0].value = 10;
    v1.as<Tracked>()[1].value = 20;
    v1.as<Tracked>()[2].value = 30;

    int ctorBefore = Tracked::ctorCount;
    int dtorBefore = Tracked::dtorCount;

    {
        VariantVector v2(std::move(v1));
        EXPECT_EQ(v2.size(), 3u);
        EXPECT_EQ(v2.as<Tracked>()[0].value, 10);
        EXPECT_EQ(v2.as<Tracked>()[2].value, 30);

        // 移动不应拷贝/构造元素（仅转移指针所有权）
        EXPECT_EQ(Tracked::ctorCount, ctorBefore);
        EXPECT_EQ(Tracked::dtorCount, dtorBefore);
    }
    // v2 析构，3 个元素被销毁
    EXPECT_EQ(Tracked::dtorCount - dtorBefore, 3);

    // v1 已为空，离开作用域不会再次析构元素
    EXPECT_FALSE(v1.hasType());
}


// ================================================================
// 29. resize 到 0
// ================================================================

TEST_F(VariantVectorTest, ResizeToZero)
{
    VariantVector v;
    v.resize<int>(5);
    v.as<int>()[0] = 1;
    v.as<int>()[4] = 5;

    v.resize<int>(0);

    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.empty());
    EXPECT_TRUE(v.hasType());        // 类型保留
    EXPECT_EQ(v.elementType(), typeid(int));

    // 可以再次 resize 使用
    v.resize<int>(3);
    EXPECT_EQ(v.size(), 3u);
}


// ================================================================
// 30. clear 后容量复用
// ================================================================

TEST_F(VariantVectorTest, CapacityReuseAfterClear)
{
    VariantVector v;
    v.resize<int>(100);
    size_t cap = v.capacity();

    v.clear();
    EXPECT_EQ(v.size(), 0u);
    EXPECT_EQ(v.capacity(), cap);     // 容量保留

    // pushBack 应复用已分配的容量，不重新分配
    v.pushBack<int>(42);
    EXPECT_EQ(v.size(), 1u);
    EXPECT_EQ(v.capacity(), cap);
    EXPECT_EQ(v.as<int>()[0], 42);
}


// ================================================================
// 31. 自移动赋值
// ================================================================

TEST_F(VariantVectorTest, SelfMoveAssignment)
{
    VariantVector v;
    v.resize<int>(3);
    v.as<int>()[0] = 1;
    v.as<int>()[1] = 2;
    v.as<int>()[2] = 3;

    v = std::move(v);  // 自移动赋值 — 不应崩溃

    // 自移动后对象应处于合法状态（可被赋值、销毁等）
    v = VariantVector();  // 正常赋值
    EXPECT_FALSE(v.hasType());
    EXPECT_EQ(v.size(), 0u);
}


// ================================================================
// 32. data() 原始指针
// ================================================================

TEST_F(VariantVectorTest, DataAccess)
{
    VariantVector v;
    v.resize<int>(3);
    v.as<int>()[0] = 10;

    EXPECT_NE(v.data(), nullptr);
    EXPECT_EQ(v.data(), static_cast<void*>(v.as<int>()));

    const VariantVector& cv = v;
    EXPECT_NE(cv.data(), nullptr);
    EXPECT_EQ(cv.data(), static_cast<const void*>(cv.as<int>()));
}


// ================================================================
// 33. 多次 clear / pushBack 循环
// ================================================================

TEST_F(VariantVectorTest, MultipleClearPushBackCycles)
{
    VariantVector v;

    for (int cycle = 0; cycle < 5; ++cycle)
    {
        for (int i = 0; i < 10; ++i)
            v.pushBack<int>(i + cycle * 100);

        EXPECT_EQ(v.size(), 10u);
        EXPECT_EQ(v.as<int>()[0], cycle * 100);
        EXPECT_EQ(v.as<int>()[9], 9 + cycle * 100);

        v.clear();
        EXPECT_EQ(v.size(), 0u);
        EXPECT_TRUE(v.hasType());    // 类型在 clear 后保留
    }
}

// ================================================================
// 34. 类型不匹配 — as / asSpan / begin / end 安全返回空
// ================================================================

TEST_F(VariantVectorTest, AsTypeMismatchReturnsNull)
{
    VariantVector v;
    v.resize<double>(3);
    v.as<double>()[0] = 1.0;

    // 用错误类型访问 — 应返回 nullptr（非 crash）
    int* p = v.as<int>();
    EXPECT_EQ(p, nullptr);
}

TEST_F(VariantVectorTest, AsTypeMismatchConstReturnsNull)
{
    VariantVector v;
    v.resize<double>(3);

    const VariantVector& cv = v;
    const int* p = cv.as<int>();
    EXPECT_EQ(p, nullptr);
}

TEST_F(VariantVectorTest, AsTypeMismatchConstNoTypeReturnsNull)
{
    VariantVector v;  // 无类型
    const VariantVector& cv = v;

    const int* p = cv.as<int>();
    EXPECT_EQ(p, nullptr);
}

TEST_F(VariantVectorTest, AsSpanTypeMismatchReturnsEmpty)
{
    VariantVector v;
    v.resize<double>(3);
    v.as<double>()[0] = 1.0;
    v.as<double>()[1] = 2.0;
    v.as<double>()[2] = 3.0;

    Span<int> s = v.asSpan<int>();           // 类型不匹配
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.data(), nullptr);

    // const 版本
    const VariantVector& cv = v;
    Span<const int> cs = cv.asSpan<int>();
    EXPECT_TRUE(cs.empty());
    EXPECT_EQ(cs.data(), nullptr);
}

TEST_F(VariantVectorTest, BeginEndTypeMismatchReturnsNull)
{
    VariantVector v;
    v.resize<double>(3);

    EXPECT_EQ(v.begin<int>(), nullptr);
    EXPECT_EQ(v.end<int>(), nullptr);

    const VariantVector& cv = v;
    EXPECT_EQ(cv.begin<int>(), nullptr);
    EXPECT_EQ(cv.end<int>(), nullptr);
}

TEST_F(VariantVectorTest, AtTypeMismatchThrows)
{
    VariantVector v;
    v.resize<double>(3);

    EXPECT_THROW(v.at<int>(0), std::logic_error);

    const VariantVector& cv = v;
    EXPECT_THROW(cv.at<int>(0), std::logic_error);
}

TEST_F(VariantVectorTest, AtConstNoTypeThrows)
{
    VariantVector v;  // 无类型
    const VariantVector& cv = v;

    EXPECT_THROW(cv.at<int>(0), std::logic_error);
}

// ================================================================
// main
// ================================================================

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
