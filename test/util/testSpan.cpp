///
/// @file      testSpan.cpp
/// @brief     Span 单元测试
/// @details   测试行为对标 std::span：构造、元素访问、迭代器、容量查询、
///            子视图以及非拥有语义。
/// @author    axel
/// @date      2026-10-18
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

#include "ast/Span.hpp"
#include "ast/AstTestMacro.h"

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <vector>

AST_USING_NAMESPACE


// 供隐式转换测试使用的接收 Span 的函数
static int sumSpan(Span<const int> s)
{
    int sum = 0;
    for (Span<const int>::iterator it = s.begin(); it != s.end(); ++it)
        sum += *it;
    return sum;
}

static int sumFixedSpan4(Span<const int, 4> s)
{
    int sum = 0;
    for (Span<const int, 4>::iterator it = s.begin(); it != s.end(); ++it)
        sum += *it;
    return sum;
}


// ================================================================
// 1. 类型别名与 extent
// ================================================================

TEST(SpanTest, TypeAliases)
{
    using S = Span<int>;

    EXPECT_TRUE((std::is_same<S::element_type, int>::value));
    EXPECT_TRUE((std::is_same<S::value_type, int>::value));
    EXPECT_TRUE((std::is_same<S::index_type, std::size_t>::value));
    EXPECT_TRUE((std::is_same<S::difference_type, std::ptrdiff_t>::value));
    EXPECT_TRUE((std::is_same<S::pointer, int*>::value));
    EXPECT_TRUE((std::is_same<S::reference, int&>::value));
    EXPECT_TRUE((std::is_same<S::const_pointer, const int*>::value));
    EXPECT_TRUE((std::is_same<S::const_reference, const int&>::value));
    EXPECT_TRUE((std::is_same<S::iterator, int*>::value));
    EXPECT_TRUE((std::is_same<S::const_iterator, const int*>::value));
    EXPECT_TRUE((std::is_same<S::reverse_iterator,
                              std::reverse_iterator<int*> >::value));
    EXPECT_TRUE((std::is_same<S::const_reverse_iterator,
                              std::reverse_iterator<const int*> >::value));
}

TEST(SpanTest, ConstElementTypeAliases)
{
    using S = Span<const int>;

    EXPECT_TRUE((std::is_same<S::element_type, const int>::value));
    EXPECT_TRUE((std::is_same<S::value_type, int>::value));  // remove_cv
    EXPECT_TRUE((std::is_same<S::pointer, const int*>::value));
    EXPECT_TRUE((std::is_same<S::reference, const int&>::value));
    EXPECT_TRUE((std::is_same<S::const_pointer, const int*>::value));
    EXPECT_TRUE((std::is_same<S::const_reference, const int&>::value));
    EXPECT_TRUE((std::is_same<S::iterator, const int*>::value));
    EXPECT_TRUE((std::is_same<S::const_iterator, const int*>::value));
}

TEST(SpanTest, ExtentConstant)
{
    // extent 的类型与 std::span 一致，为 std::size_t
    EXPECT_TRUE((std::is_same<decltype(Span<int>::extent), const std::size_t>::value));

    // 动态 extent 的 span 的 extent 等于 dynamic_extent
    EXPECT_EQ(Span<int>::extent, dynamic_extent);
    EXPECT_EQ((Span<int, 4>::extent), 4u);
    EXPECT_EQ(dynamic_extent, static_cast<std::size_t>(-1));
}


// ================================================================
// 2. 默认构造
// ================================================================

TEST(SpanTest, DefaultConstruction)
{
    Span<int> s;

    EXPECT_EQ(s.data(), nullptr);
    EXPECT_EQ(s.size(), 0u);
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.begin(), s.end());
    EXPECT_EQ(s.cbegin(), s.cend());
    EXPECT_EQ(s.rbegin(), s.rend());
    EXPECT_EQ(s.crbegin(), s.crend());
}

TEST(SpanTest, DefaultConstructionWithZeroExtent)
{
    // std::span<T, 0> 可以默认构造
    Span<int, 0> s;

    EXPECT_EQ(s.data(), nullptr);
    EXPECT_EQ(s.size(), 0u);
    EXPECT_TRUE(s.empty());
}

TEST(SpanTest, DefaultConstructionConstraint)
{
    // 固定非零 extent 的 span 不能默认构造
    EXPECT_TRUE((std::is_default_constructible<Span<int> >::value));
    EXPECT_TRUE((std::is_default_constructible<Span<int, 0> >::value));
    EXPECT_FALSE((std::is_default_constructible<Span<int, 4> >::value));
}


// ================================================================
// 3. 从 C 数组构造
// ================================================================

TEST(SpanTest, FromCArray)
{
    int arr[5] = {1, 2, 3, 4, 5};
    Span<int> s(arr);

    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s.size_bytes(), sizeof(arr));
    EXPECT_EQ(s.data(), arr);
    EXPECT_FALSE(s.empty());

    EXPECT_EQ(s[0], 1);
    EXPECT_EQ(s[4], 5);
    EXPECT_EQ(s.front(), 1);
    EXPECT_EQ(s.back(), 5);

    // 非拥有视图：通过 Span 写入即可见原数组
    s[2] = 30;
    EXPECT_EQ(arr[2], 30);

    // 反向亦然
    arr[0] = 10;
    EXPECT_EQ(s[0], 10);
}

TEST(SpanTest, FromConstCArray)
{
    const int arr[3] = {7, 8, 9};
    Span<const int> s(arr);

    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s.data(), arr);
    EXPECT_EQ(s[0], 7);
    EXPECT_EQ(s.front(), 7);
    EXPECT_EQ(s.back(), 9);

    int sum = 0;
    for (const int& x : s)
        sum += x;
    EXPECT_EQ(sum, 24);
}


// ================================================================
// 4. 从指针构造
// ================================================================

TEST(SpanTest, FromPointerAndCount)
{
    int arr[] = {1, 2, 3, 4, 5};
    Span<int> s(arr, 3);

    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s.data(), arr);
    EXPECT_EQ(s[0], 1);
    EXPECT_EQ(s.back(), 3);

    // 允许指向非开头的位置
    Span<int> tail(arr + 2, 3);
    EXPECT_EQ(tail.data(), arr + 2);
    EXPECT_EQ(tail[0], 3);
    EXPECT_EQ(tail[2], 5);

    // count 为 0 时保留指针
    Span<int> empty(arr, 0);
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.data(), arr);

    // 非 const 指针可以构造 const 元素类型的 span
    Span<const int> cs(arr, 2);
    EXPECT_EQ(cs.size(), 2u);
    EXPECT_EQ(cs.data(), arr);
}

TEST(SpanTest, FromPointerRange)
{
    int arr[] = {1, 2, 3, 4, 5};
    Span<int> s(arr, arr + 5);

    EXPECT_EQ(s.size(), 5u);
    EXPECT_EQ(s.data(), arr);
    EXPECT_EQ(s[4], 5);

    // 空区间
    Span<int> empty(arr + 2, arr + 2);
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.data(), arr + 2);

    // const 指针区间
    const int* first = arr;
    Span<const int> cs(first, first + 3);
    EXPECT_EQ(cs.size(), 3u);
    EXPECT_EQ(cs.back(), 3);
}


// ================================================================
// 5. 从标准容器构造
// ================================================================

TEST(SpanTest, FromVector)
{
    std::vector<int> v = {10, 20, 30};
    Span<int> s(v);

    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s.data(), v.data());
    EXPECT_EQ(s[1], 20);

    // 非拥有：双向可见
    s[0] = 11;
    EXPECT_EQ(v[0], 11);
    v[2] = 33;
    EXPECT_EQ(s[2], 33);

    std::vector<int> empty;
    Span<int> es(empty);
    EXPECT_TRUE(es.empty());
    EXPECT_EQ(es.size(), 0u);
}

TEST(SpanTest, FromConstVector)
{
    std::vector<int> v = {10, 20, 30};
    const std::vector<int>& cv = v;

    Span<const int> s(cv);
    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s.data(), v.data());
    EXPECT_EQ(s[0], 10);
    EXPECT_EQ(s.back(), 30);

    // 非 const 容器同样可以构造 const 元素类型的 span
    Span<const int> s2(v);
    EXPECT_EQ(s2.size(), 3u);
    EXPECT_EQ(s2.data(), v.data());
}

TEST(SpanTest, FromStdArray)
{
    std::array<int, 4> a = {{1, 2, 3, 4}};
    Span<int> s(a);

    EXPECT_EQ(s.size(), 4u);
    EXPECT_EQ(s.data(), a.data());
    EXPECT_EQ(s.back(), 4);

    s[3] = 40;
    EXPECT_EQ(a[3], 40);

    const std::array<int, 4> ca = {{5, 6, 7, 8}};
    Span<const int> cs(ca);
    EXPECT_EQ(cs.size(), 4u);
    EXPECT_EQ(cs.data(), ca.data());
    EXPECT_EQ(cs[3], 8);
}

TEST(SpanTest, FromInitializerList)
{
    // initializer_list 的元素是 const 的，因此只能构造 const 元素类型的 span。
    std::initializer_list<int> il = {1, 2, 3, 4};
    Span<const int> s(il);

    EXPECT_EQ(s.size(), 4u);
    EXPECT_EQ(s[0], 1);
    EXPECT_EQ(s.back(), 4);

    int sum = 0;
    for (const int& x : s)
        sum += x;
    EXPECT_EQ(sum, 10);

}

TEST(SpanTest, ImplicitConversionRules)
{
    // 与 std::span 一致：动态 extent 的构造函数不是 explicit，
    // 固定 extent 从动态长度来源构造时才是 explicit。
    using Vec = std::vector<int>;
    using Arr = std::array<int, 4>;

    // 动态 extent：容器/数组可隐式转换
    EXPECT_TRUE((std::is_convertible<Vec&, Span<int> >::value));
    EXPECT_TRUE((std::is_convertible<const Vec&, Span<const int> >::value));
    EXPECT_TRUE((std::is_convertible<Arr&, Span<int> >::value));
    EXPECT_TRUE((std::is_convertible<const Arr&, Span<const int> >::value));
    EXPECT_TRUE((std::is_convertible<int (&)[4], Span<int> >::value));

    // 固定 extent + 动态长度来源：只能显式构造
    EXPECT_FALSE((std::is_convertible<Vec&, Span<int, 4> >::value));
    EXPECT_FALSE((std::is_convertible<const Vec&, Span<const int, 4> >::value));
    EXPECT_TRUE((std::is_constructible<Span<int, 4>, Vec&>::value));
    EXPECT_TRUE((std::is_constructible<Span<int, 4>, const Vec&>::value));

    // 固定 extent + 长度在编译期已知且匹配：可隐式构造
    EXPECT_TRUE((std::is_convertible<Arr&, Span<int, 4> >::value));
    EXPECT_TRUE((std::is_convertible<const Arr&, Span<const int, 4> >::value));
    EXPECT_TRUE((std::is_convertible<int (&)[4], Span<int, 4> >::value));

    // 长度不匹配时构造函数被约束掉
    EXPECT_FALSE((std::is_constructible<Span<int, 4>, std::array<int, 5>&>::value));
    EXPECT_FALSE((std::is_constructible<Span<int, 4>, int (&)[5]>::value));

    // initializer_list 不能隐式转成 span
    EXPECT_FALSE((std::is_convertible<std::initializer_list<int>, Span<const int> >::value));
    EXPECT_FALSE((std::is_convertible<std::initializer_list<int>, Span<const int, 3> >::value));
}

TEST(SpanTest, ImplicitConversionInFunctionCall)
{
    std::vector<int> v = {1, 2, 3};
    std::array<int, 4> a = {{1, 2, 3, 4}};
    int arr[4] = {1, 2, 3, 4};

    // 动态 extent：容器和数组都能直接传给 Span 形参
    EXPECT_EQ(sumSpan(v), 6);
    EXPECT_EQ(sumSpan(a), 10);
    EXPECT_EQ(sumSpan(arr), 10);

    // 固定 extent：长度匹配的 std::array / C 数组可隐式绑定
    EXPECT_EQ(sumFixedSpan4(a), 10);
    EXPECT_EQ(sumFixedSpan4(arr), 10);
}



// ================================================================
// 6. 固定 extent
// ================================================================

TEST(SpanTest, FixedExtentFromArray)
{
    int arr[4] = {1, 2, 3, 4};
    Span<int, 4> s(arr);

    EXPECT_EQ(s.size(), 4u);
    EXPECT_EQ(s.size_bytes(), sizeof(arr));
    EXPECT_EQ(s.data(), arr);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.front(), 1);
    EXPECT_EQ(s.back(), 4);

    s[2] = 30;
    EXPECT_EQ(arr[2], 30);

    int sum = 0;
    for (int* it = s.begin(); it != s.end(); ++it)
        sum += *it;
    EXPECT_EQ(sum, 1 + 2 + 30 + 4);

    // 拷贝后仍指向同一段内存
    Span<int, 4> copy(s);
    EXPECT_EQ(copy.data(), arr);
    EXPECT_EQ(copy.size(), 4u);
    EXPECT_EQ(copy[2], 30);
}

TEST(SpanTest, FixedExtentFromPointerAndCount)
{
    int arr[] = {1, 2, 3, 4, 5};
    Span<int, 3> s(arr + 1, 3);

    EXPECT_EQ(s.size(), 3u);
    EXPECT_EQ(s.data(), arr + 1);
    EXPECT_EQ(s[0], 2);
    EXPECT_EQ(s.back(), 4);
}

TEST(SpanTest, FixedExtentSubviews)
{
    int arr[4] = {1, 2, 3, 4};
    Span<int, 4> s(arr);

    // std::span 的运行时计数版本 first/last/subspan 返回动态 extent 的 span
    Span<int> f = s.first(2);
    EXPECT_EQ(f.size(), 2u);
    EXPECT_EQ(f.data(), arr);
    EXPECT_EQ(f[1], 2);

    Span<int> l = s.last(1);
    EXPECT_EQ(l.size(), 1u);
    EXPECT_EQ(l.data(), arr + 3);
    EXPECT_EQ(l[0], 4);

    Span<int> sub = s.subspan(1, 2);
    EXPECT_EQ(sub.size(), 2u);
    EXPECT_EQ(sub.data(), arr + 1);
    EXPECT_EQ(sub[0], 2);
    EXPECT_EQ(sub[1], 3);
}

TEST(SpanTest, FixedExtentInitializerListConstraint)
{
    // 固定非零 extent 无法在编译期校验 initializer_list 的长度，因此不提供该
    // 构造函数，避免空列表或长度不符的列表构造出越界的 span。
    EXPECT_FALSE((std::is_constructible<Span<const int, 3>, std::initializer_list<int> >::value));
    EXPECT_TRUE((std::is_constructible<Span<const int>, std::initializer_list<int> >::value));

    // 零 extent 仍然可以接受空初始化列表
    Span<const int, 0> s({});
    EXPECT_TRUE(s.empty());
}


// ================================================================
// 7. 元素访问
// ================================================================

TEST(SpanTest, ElementAccess)
{
    int arr[] = {1, 2, 3};
    Span<int> s(arr);

    s[1] = 20;
    EXPECT_EQ(s[1], 20);
    EXPECT_EQ(arr[1], 20);

    // const Span 对象不意味着元素不可修改（与 std::span 一致）
    const Span<int> cs(arr);
    cs[0] = 10;
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(cs.front(), 10);
    EXPECT_EQ(cs.back(), 3);
}

TEST(SpanTest, ConstElementAccess)
{
    int arr[] = {1, 2, 3};
    const Span<const int> s(arr);

    EXPECT_EQ(s[0], 1);
    EXPECT_EQ(s.front(), 1);
    EXPECT_EQ(s.back(), 3);
    EXPECT_TRUE((std::is_same<decltype(s.front()), const int&>::value));

    // 子视图保持 const 元素类型
    Span<const int> sub = s.subspan(1);
    ASSERT_EQ(sub.size(), 2u);
    EXPECT_EQ(sub[0], 2);
    EXPECT_TRUE((std::is_same<decltype(sub.front()), const int&>::value));

}


// ================================================================
// 8. 迭代器
// ================================================================

TEST(SpanTest, Iterators)
{
    int arr[] = {1, 2, 3, 4, 5};
    Span<int> s(arr);

    EXPECT_EQ(std::distance(s.begin(), s.end()),
              static_cast<std::ptrdiff_t>(5));

    int sum = 0;
    for (int* it = s.begin(); it != s.end(); ++it)
        sum += *it;
    EXPECT_EQ(sum, 15);

    // 通过迭代器写入
    for (int* it = s.begin(); it != s.end(); ++it)
        *it *= 2;
    EXPECT_EQ(arr[0], 2);
    EXPECT_EQ(arr[4], 10);

    // const 迭代器只读
    int csum = 0;
    for (const int* it = s.cbegin(); it != s.cend(); ++it)
        csum += *it;
    EXPECT_EQ(csum, 30);
    EXPECT_EQ(s.cbegin(), s.begin());
    EXPECT_EQ(s.cend(), s.end());
}

TEST(SpanTest, ReverseIterators)
{
    int arr[] = {1, 2, 3};
    Span<int> s(arr);

    EXPECT_EQ(*s.rbegin(), 3);
    EXPECT_EQ(*s.crbegin(), 3);

    std::vector<int> rev(s.rbegin(), s.rend());
    ASSERT_EQ(rev.size(), 3u);
    EXPECT_EQ(rev[0], 3);
    EXPECT_EQ(rev[1], 2);
    EXPECT_EQ(rev[2], 1);

    const Span<int>& cs = s;
    std::vector<int> crev(cs.crbegin(), cs.crend());
    ASSERT_EQ(crev.size(), 3u);
    EXPECT_EQ(crev[0], 3);
    EXPECT_EQ(crev[2], 1);
}


// ================================================================
// 9. 容量
// ================================================================

TEST(SpanTest, SizeBytes)
{
    int ints[4] = {0, 0, 0, 0};
    Span<int> si(ints);
    EXPECT_EQ(si.size_bytes(), 4u * sizeof(int));

    double doubles[3] = {0.0, 0.0, 0.0};
    Span<double> sd(doubles);
    EXPECT_EQ(sd.size_bytes(), 3u * sizeof(double));

    struct Triple { int a, b, c; };
    Triple triples[2] = {{0, 0, 0}, {0, 0, 0}};
    Span<Triple> st(triples);
    EXPECT_EQ(st.size_bytes(), 2u * sizeof(Triple));

    // 空 span 的 size_bytes 为 0
    Span<int> empty;
    EXPECT_EQ(empty.size_bytes(), 0u);
}


// ================================================================
// 10. 子视图
// ================================================================

TEST(SpanTest, FirstLastSubspan)
{
    int arr[] = {1, 2, 3, 4, 5};
    Span<int> s(arr);

    Span<int> f = s.first(2);
    EXPECT_EQ(f.size(), 2u);
    EXPECT_EQ(f.data(), arr);
    EXPECT_EQ(f[0], 1);
    EXPECT_EQ(f[1], 2);

    Span<int> l = s.last(2);
    EXPECT_EQ(l.size(), 2u);
    EXPECT_EQ(l.data(), arr + 3);
    EXPECT_EQ(l[0], 4);
    EXPECT_EQ(l[1], 5);

    // subspan(offset) —— 默认取到末尾
    Span<int> sub = s.subspan(1);
    EXPECT_EQ(sub.size(), 4u);
    EXPECT_EQ(sub.data(), arr + 1);
    EXPECT_EQ(sub[0], 2);
    EXPECT_EQ(sub.back(), 5);

    // subspan(offset, count)
    Span<int> sub2 = s.subspan(1, 2);
    EXPECT_EQ(sub2.size(), 2u);
    EXPECT_EQ(sub2.data(), arr + 1);
    EXPECT_EQ(sub2[0], 2);
    EXPECT_EQ(sub2[1], 3);

    // offset == size() 是合法的，得到空视图
    Span<int> tail = s.subspan(5);
    EXPECT_TRUE(tail.empty());
    EXPECT_EQ(tail.data(), arr + 5);

    // count 为 0
    Span<int> zero = s.subspan(2, 0);
    EXPECT_TRUE(zero.empty());
    EXPECT_EQ(zero.data(), arr + 2);
}

TEST(SpanTest, SubspanWithDynamicExtent)
{
    int arr[] = {1, 2, 3, 4, 5};
    Span<int> s(arr);

    // 显式传入 dynamic_extent 与使用默认参数等价
    Span<int> sub = s.subspan(2, dynamic_extent);
    EXPECT_EQ(sub.size(), 3u);
    EXPECT_EQ(sub.data(), arr + 2);
    EXPECT_EQ(sub[0], 3);
}

TEST(SpanTest, SubviewsAreViews)
{
    int arr[] = {1, 2, 3, 4, 5};
    Span<int> s(arr);

    // 子视图与原 span 共享内存
    Span<int> sub = s.subspan(1, 3);
    sub[0] = 20;
    EXPECT_EQ(arr[1], 20);
    EXPECT_EQ(s[1], 20);

    arr[2] = 30;
    EXPECT_EQ(sub[1], 30);
}

TEST(SpanTest, NestedSubviews)
{
    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Span<int> s(arr);

    // s[2..8) -> last(4) = s[4..8) -> first(2) = s[4..6)
    Span<int> mid = s.subspan(2, 6).last(4).first(2);

    EXPECT_EQ(mid.size(), 2u);
    EXPECT_EQ(mid.data(), arr + 4);
    EXPECT_EQ(mid[0], 4);
    EXPECT_EQ(mid[1], 5);
}

TEST(SpanTest, EmptySpanSubviews)
{
    Span<int> s;

    EXPECT_TRUE(s.first(0).empty());
    EXPECT_TRUE(s.last(0).empty());
    EXPECT_TRUE(s.subspan(0).empty());
    EXPECT_TRUE(s.subspan(0, 0).empty());
    EXPECT_EQ(s.first(0).data(), nullptr);
    EXPECT_EQ(s.subspan(0).data(), nullptr);
}


// ================================================================
// 11. 编译期计数子视图：first<Count>() / last<Count>() / subspan<Offset, Count>()
// ================================================================

TEST(SpanTest, TemplateFirstLast)
{
    int arr[] = {1, 2, 3, 4, 5};
    Span<int> s(arr);

    auto f = s.first<3>();
    EXPECT_TRUE((std::is_same<decltype(f), Span<int, 3> >::value));
    EXPECT_EQ(f.extent, 3u);
    EXPECT_EQ(f.size(), 3u);
    EXPECT_EQ(f.data(), arr);
    EXPECT_EQ(f[0], 1);
    EXPECT_EQ(f[2], 3);

    auto l = s.last<2>();
    EXPECT_TRUE((std::is_same<decltype(l), Span<int, 2> >::value));
    EXPECT_EQ(l.extent, 2u);
    EXPECT_EQ(l.size(), 2u);
    EXPECT_EQ(l.data(), arr + 3);
    EXPECT_EQ(l[0], 4);
    EXPECT_EQ(l[1], 5);
}

TEST(SpanTest, TemplateSubspan)
{
    int arr[] = {0, 1, 2, 3, 4, 5};
    Span<int> s(arr);

    // 指定偏移和计数：返回固定 extent 的 span
    auto a = s.subspan<2, 3>();
    EXPECT_TRUE((std::is_same<decltype(a), Span<int, 3> >::value));
    EXPECT_EQ(a.extent, 3u);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_EQ(a.data(), arr + 2);
    EXPECT_EQ(a[0], 2);
    EXPECT_EQ(a[2], 4);

    // 省略计数：动态 extent 的 span 仍取到末尾
    auto b = s.subspan<2>();
    EXPECT_TRUE((std::is_same<decltype(b), Span<int> >::value));
    EXPECT_EQ(b.size(), 4u);
    EXPECT_EQ(b.data(), arr + 2);
    EXPECT_EQ(b.back(), 5);

    // 显式 dynamic_extent 与省略等价
    auto c = s.subspan<2, dynamic_extent>();
    EXPECT_TRUE((std::is_same<decltype(c), Span<int> >::value));
    EXPECT_EQ(c.size(), 4u);
    EXPECT_EQ(c.data(), arr + 2);
}

TEST(SpanTest, TemplateSubviewsOnFixedExtent)
{
    int arr[5] = {1, 2, 3, 4, 5};
    Span<int, 5> s(arr);

    auto f = s.first<2>();
    EXPECT_TRUE((std::is_same<decltype(f), Span<int, 2> >::value));
    EXPECT_EQ(f.size(), 2u);
    EXPECT_EQ(f.data(), arr);
    EXPECT_EQ(f[1], 2);

    auto l = s.last<2>();
    EXPECT_TRUE((std::is_same<decltype(l), Span<int, 2> >::value));
    EXPECT_EQ(l.size(), 2u);
    EXPECT_EQ(l.data(), arr + 3);
    EXPECT_EQ(l[0], 4);

    // 固定 extent 上省略计数时，返回剩余长度作为新的固定 extent
    auto rest = s.subspan<1>();
    EXPECT_TRUE((std::is_same<decltype(rest), Span<int, 4> >::value));
    EXPECT_EQ(rest.extent, 4u);
    EXPECT_EQ(rest.size(), 4u);
    EXPECT_EQ(rest.data(), arr + 1);
    EXPECT_EQ(rest.back(), 5);

    // 指定计数
    auto sub = s.subspan<1, 3>();
    EXPECT_TRUE((std::is_same<decltype(sub), Span<int, 3> >::value));
    EXPECT_EQ(sub.size(), 3u);
    EXPECT_EQ(sub.data(), arr + 1);
    EXPECT_EQ(sub[0], 2);
    EXPECT_EQ(sub[2], 4);

    // offset == extent 得到固定 extent 为 0 的空视图
    auto tail = s.subspan<5>();
    EXPECT_TRUE((std::is_same<decltype(tail), Span<int, 0> >::value));
    EXPECT_TRUE(tail.empty());
    EXPECT_EQ(tail.extent, 0u);
    EXPECT_EQ(tail.data(), arr + 5);
}

TEST(SpanTest, TemplateSubviewZeroCount)
{
    int arr[] = {1, 2, 3};
    Span<int> s(arr);

    auto f = s.first<0>();
    EXPECT_TRUE((std::is_same<decltype(f), Span<int, 0> >::value));
    EXPECT_TRUE(f.empty());
    EXPECT_EQ(f.size(), 0u);
    EXPECT_EQ(f.data(), arr);

    auto l = s.last<0>();
    EXPECT_TRUE((std::is_same<decltype(l), Span<int, 0> >::value));
    EXPECT_TRUE(l.empty());
    EXPECT_EQ(l.data(), arr + 3);
}

TEST(SpanTest, TemplateSubviewConstElement)
{
    int arr[] = {1, 2, 3};
    const Span<const int> s(arr);

    auto f = s.first<2>();
    EXPECT_TRUE((std::is_same<decltype(f), Span<const int, 2> >::value));
    EXPECT_EQ(f.size(), 2u);
    EXPECT_EQ(f[0], 1);
    EXPECT_EQ(f[1], 2);

    auto sub = s.subspan<1>();
    EXPECT_TRUE((std::is_same<decltype(sub), Span<const int> >::value));
    EXPECT_EQ(sub.size(), 2u);
    EXPECT_EQ(sub[0], 2);
}

TEST(SpanTest, TemplateSubviewChaining)
{
    int arr[] = {0, 1, 2, 3, 4, 5};
    Span<int> s(arr);

    // first<4>() 得到 Span<int,4>，再 last<2>() 得到 Span<int,2>
    auto mid = s.first<4>().last<2>();
    EXPECT_TRUE((std::is_same<decltype(mid), Span<int, 2> >::value));
    EXPECT_EQ(mid.size(), 2u);
    EXPECT_EQ(mid.data(), arr + 2);
    EXPECT_EQ(mid[0], 2);
    EXPECT_EQ(mid[1], 3);

    // 编译期与运行时版本混用。
    // subspan<1,4>() 返回的临时 Span 只是视图、不拥有数据，
    // 它销毁后 last(2) 返回的 Span 仍指向原数组，因此安全。
    auto mixed = s.subspan<1, 4>().last(2);
    EXPECT_EQ(mixed.size(), 2u);
    EXPECT_EQ(mixed.data(), arr + 3);
}

TEST(SpanTest, TemplateSubviewIsView)
{
    int arr[] = {1, 2, 3, 4};
    Span<int> s(arr);

    auto f = s.first<2>();
    f[0] = 10;
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(s[0], 10);

    arr[1] = 20;
    EXPECT_EQ(f[1], 20);
}


// ================================================================
// 12. 非拥有语义与拷贝语义
// ================================================================

TEST(SpanTest, NonOwningView)
{
    std::vector<int> v = {1, 2, 3};
    Span<int> s(v);

    // 容器内容变化，视图随之变化（未发生重新分配）
    v[1] = 20;
    EXPECT_EQ(s[1], 20);

    // 视图写入影响容器
    s[2] = 30;
    EXPECT_EQ(v[2], 30);
}

TEST(SpanTest, CopyAndAssign)
{
    int arr[] = {1, 2, 3};
    Span<int> s(arr);

    Span<int> copy(s);
    EXPECT_EQ(copy.data(), s.data());
    EXPECT_EQ(copy.size(), s.size());
    EXPECT_EQ(copy[2], 3);

    int other[] = {9};
    Span<int> t(other, 1);
    t = s;
    EXPECT_EQ(t.data(), arr);
    EXPECT_EQ(t.size(), 3u);
    EXPECT_EQ(t.back(), 3);
}

TEST(SpanTest, TriviallyCopyable)
{
    // std::span 是可平凡拷贝的轻量视图
    EXPECT_TRUE((std::is_trivially_copyable<Span<int> >::value));
    EXPECT_TRUE((std::is_trivially_copyable<Span<const int> >::value));
    EXPECT_TRUE((std::is_trivially_copyable<Span<int, 4> >::value));
}


// ================================================================
// main
// ================================================================

GTEST_MAIN();
