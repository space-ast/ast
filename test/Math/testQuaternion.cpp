///
/// @file      testQuaternion.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-08
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
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

#include "AstMath/Quaternion.hpp"
#include "AstTest/AstTestMacro.h"

#if AST_WITH_FMT
#include "Eigen/Dense"
#endif


AST_USING_NAMESPACE


template<typename Quaternion>
void testQuaternion()
{
    Quaternion q{1,2,3,4};
    EXPECT_EQ(q.w(), 1);
    EXPECT_EQ(q.x(), 2);
    EXPECT_EQ(q.y(), 3);
    EXPECT_EQ(q.z(), 4);

    Quaternion q2{2,3,4,5};
    auto result = q * q2;

    nothing();
    
}


TEST(Quaternion, Init)
{
    testQuaternion<Quaternion>();
    #if AST_WITH_EIGEN
    testQuaternion<Eigen::Quaterniond>();
    #endif
}


GTEST_MAIN()