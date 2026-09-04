///
/// @file      testAxesFrozenNullReference.cpp
/// @brief     AxesFrozen 异常场景单元测试
/// @details   复现 AxesFrozen 的 referenceAxes_ 为空（/失效）时，
///            getParent() 返回 nullptr，从而与真正的根节点 AxesRoot 无法区分的问题。
/// @author    axel
/// @date      2026-09-04
/// @copyright 版权所有 (C) 2026-present, ast项目.
///

#include "ast/AxesFrozen.hpp"
#include "ast/BuiltinAxes.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Rotation.hpp"
#include "ast/RunTime.hpp"
#include "ast/AstTestMacro.h"

AST_USING_NAMESPACE

// ============================================
// Test Fixture - 统一管理初始化
// ============================================
class AxesFrozenNullTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        aInitialize();
    }
};

// ============================================
// 现象1：构造 referenceAxes_ == nullptr 的 AxesFrozen
//        其 getParent()/getDepth() 与根节点无区别（歧义）
// ============================================
TEST_F(AxesFrozenNullTest, DegenerateFrozenAxesLooksLikeRoot)
{
    auto freezeTime = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);

    Axes* root = aAxesRoot();
    Axes* icrf = aAxesICRF();
    Axes* ecf  = aAxesECF();
    ASSERT_NE(root, nullptr);
    ASSERT_NE(icrf, nullptr);
    ASSERT_NE(ecf, nullptr);

    // 构造异常组件：referenceAxes_ 显式为空
    AxesFrozen* abnormal = AxesFrozen::New(ecf, freezeTime, nullptr);
    ASSERT_NE(abnormal, nullptr);

    // 现象1：getParent() 返回 nullptr —— 与真正的根节点 AxesRoot 无法区分
    EXPECT_EQ(abnormal->getParent(), nullptr);
    EXPECT_EQ(abnormal->getParent(), root->getParent());   // 都是 nullptr

    // 现象2：深度与根节点相同（上溯逻辑把异常节点当作"深度1的根"）
    EXPECT_EQ(abnormal->getDepth(), root->getDepth());

    // 现象3：getAncestor 在边界外同样返回 nullptr
    EXPECT_EQ(abnormal->getAncestor(0), abnormal);         // 自身
    EXPECT_EQ(abnormal->getAncestor(1), nullptr);          // 越界，与根的 getAncestor(1) 一致
}

// ============================================
// 现象2：异常节点参与变换计算
//        当前实现可能在 Axes.cpp:121 附近越界访问（崩溃或返回错误码/垃圾值）
// ============================================
TEST_F(AxesFrozenNullTest, TransformThroughUnresolvedFrozenAxes)
{
    auto freezeTime = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    auto evalTime   = TimePoint::FromUTC(2026, 6, 15, 12, 0, 0);

    Axes* icrf = aAxesICRF();
    Axes* ecf  = aAxesECF();
    ASSERT_NE(icrf, nullptr);
    ASSERT_NE(ecf, nullptr);

    AxesFrozen* abnormal = AxesFrozen::New(ecf, freezeTime, nullptr);
    ASSERT_NE(abnormal, nullptr);

    Rotation rot;
    // TODO: 修复前，路径以 getParent()==nullptr 终止，异常节点被当作根
    //       会导致逻辑出现问题，读到未初始化的栈内存（越界崩溃）。
    errc_t rc;
    rc = aAxesTransform(*abnormal, *icrf, evalTime, rot);
    EXPECT_NE(rc, eNoError);
    rc = aAxesTransform(*icrf, *abnormal, evalTime, rot);
    EXPECT_NE(rc, eNoError);
}

GTEST_MAIN()
