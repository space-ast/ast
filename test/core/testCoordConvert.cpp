#include "ast/CoordConvert.hpp"
#include "ast/LatLonRad.hpp"
#include "ast/Vector.hpp"
#include "ast/Constants.hpp"
#include "ast/AstTestMacro.h"
#include <cmath>

AST_USING_NAMESPACE

// 测试直角坐标转纬度经度半径坐标
TEST(CoordConvertTest, CartToLLR)
{
    // +X 轴：经度 0，纬度 0
    {
        LatLonRad llr;
        aCartToLLR({1.0, 0.0, 0.0}, llr);
        EXPECT_DOUBLE_EQ(llr.radius(), 1.0);
        EXPECT_DOUBLE_EQ(llr.longitude(), 0.0);
        EXPECT_DOUBLE_EQ(llr.latitude(), 0.0);
    }

    // +Y 轴：经度 pi/2
    {
        LatLonRad llr;
        aCartToLLR({0.0, 2.0, 0.0}, llr);
        EXPECT_DOUBLE_EQ(llr.radius(), 2.0);
        EXPECT_DOUBLE_EQ(llr.longitude(), kPI / 2.0);
        EXPECT_DOUBLE_EQ(llr.latitude(), 0.0);
    }

    // +Z 轴：纬度 pi/2，经度 0
    {
        LatLonRad llr;
        aCartToLLR({0.0, 0.0, 3.0}, llr);
        EXPECT_DOUBLE_EQ(llr.radius(), 3.0);
        EXPECT_DOUBLE_EQ(llr.longitude(), 0.0);
        EXPECT_DOUBLE_EQ(llr.latitude(), kPI / 2.0);
    }

    // -X 轴：经度为 pi（而不是 -pi）
    {
        LatLonRad llr;
        aCartToLLR({-1.0, 0.0, 0.0}, llr);
        EXPECT_DOUBLE_EQ(llr.radius(), 1.0);
        EXPECT_NEAR(llr.longitude(), kPI, 1e-15);
        EXPECT_DOUBLE_EQ(llr.latitude(), 0.0);
    }

    // 等分体对角线：经度 45°，纬度 asin(1/sqrt(3))
    {
        LatLonRad llr;
        aCartToLLR({1.0, 1.0, 1.0}, llr);
        EXPECT_NEAR(llr.radius(), std::sqrt(3.0), 1e-15);
        EXPECT_NEAR(llr.longitude(), kPI / 4.0, 1e-15);
        EXPECT_NEAR(llr.latitude(), std::asin(1.0 / std::sqrt(3.0)), 1e-15);
    }

    // 零向量：经度、纬度均为 0
    {
        LatLonRad llr;
        aCartToLLR({0.0, 0.0, 0.0}, llr);
        EXPECT_DOUBLE_EQ(llr.radius(), 0.0);
        EXPECT_DOUBLE_EQ(llr.longitude(), 0.0);
        EXPECT_DOUBLE_EQ(llr.latitude(), 0.0);
    }
}

GTEST_MAIN()
