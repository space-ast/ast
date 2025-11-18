/// @file      testAttitudeConvert.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      16.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。


#include "AstCore/AttitudeConvert.hpp"
#include "AstCore/Quaternion.hpp"
#include "AstCore/Matrix.hpp"
#include "AstCore/Euler.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE


int testQuatAnsMatrix(const Quaternion& quatInput)
{
    Quaternion quat2, quat = quatInput;
    quat.normalize();
    Matrix3d mtx;
    aQuatToMatrix(quat, mtx);
    aMatrixToQuat(mtx, quat2);
    for (size_t i = 0; i < size(quat2); i++)
    {
        ASSERT_NEAR(quat[i], quat2[i], 1e-14);
    }
    return 0;
}

int testQuatAndMatrix()
{
    {
        Matrix3d mtx = Matrix3d::Identify();
        Matrix3d mtx2;
        Quaternion quat;
        aMatrixToQuat(mtx, quat);
        aQuatToMatrix(quat, mtx2);
        ASSERT_EQ(quat.qs(), 1);
        ASSERT_EQ(quat.qx(), 0);
        ASSERT_EQ(quat.qy(), 0);
        ASSERT_EQ(quat.qz(), 0);
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                ASSERT_EQ(mtx(i,j), mtx2(i,j));
            }
        }

    }

    ASSERT_FALSE(testQuatAnsMatrix({ 1,0,0,0 }));
    ASSERT_FALSE(testQuatAnsMatrix({ 1,2,3,4 }));
    ASSERT_FALSE(testQuatAnsMatrix({ 2,-2,-1,2 }));

    return 0;
}

int main()
{
    int rc = 0;
    rc |= testQuatAndMatrix();

    return rc;
}