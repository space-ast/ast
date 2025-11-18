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


int testEulerAndMatrix(int seq, const Euler& eulerInput)
{
    Euler euler = eulerInput;
    Euler euler2;
    Matrix3d mtx, mtx2;
    aEulerToMatrix(euler, seq, mtx);
    _aEulerToMatrix(euler, seq, mtx2);
    aMatrixToEuler(mtx, seq, euler2);
    for (int i = 0; i < 9; i++)
    {
        ASSERT_NEAR(mtx(i), mtx2(i), 1e-14);
    }
    for (int i = 0; i < size(euler); i++)
    {
        ASSERT_NEAR(euler[i], euler2[i], 1e-14);
    }
    return 0;
}

int testEulerAndMatrix(const Euler& eulerInput)
{
    int rc = 0;
    rc |= testEulerAndMatrix(123, eulerInput);
    rc |= testEulerAndMatrix(132, eulerInput);
    rc |= testEulerAndMatrix(213, eulerInput);
    rc |= testEulerAndMatrix(231, eulerInput);
    rc |= testEulerAndMatrix(312, eulerInput);
    rc |= testEulerAndMatrix(321, eulerInput);
    rc |= testEulerAndMatrix(121, eulerInput);
    rc |= testEulerAndMatrix(131, eulerInput);
    rc |= testEulerAndMatrix(212, eulerInput);
    rc |= testEulerAndMatrix(232, eulerInput);
    rc |= testEulerAndMatrix(313, eulerInput);
    rc |= testEulerAndMatrix(323, eulerInput);
    return rc;
}


int testEulerAndMatrix()
{
    int rc = 0;
    rc = testEulerAndMatrix({ 0,0,0 });
    rc = testEulerAndMatrix({ 1,0,0 });
    rc = testEulerAndMatrix({ 1,1.2,3 });
    rc = testEulerAndMatrix({ -1,1.1,3 });
    rc = testEulerAndMatrix({ -1,1.2,-2 });
    rc = testEulerAndMatrix({ -0.4,0.1,-2 });
    return rc;
}

int testExample1()
{
    // 创建四元数 (绕Z轴旋转90度)
    Quaternion quat = { 0.707, 0, 0, 0.707 };

    // 四元数转旋转矩阵
    Matrix3d matrix;
    aQuatToMatrix(quat, matrix);

    std::cout << "四元数转矩阵成功" << std::endl;

    // 旋转矩阵转欧拉角 (ZYX顺序)
    Euler euler;
    aMatrixToEuler(matrix, Euler::eZYX, euler);

    std::cout << "欧拉角: " << euler.angle1() << ", "
        << euler.angle2() << ", " << euler.angle3() << std::endl;

    return 0;
}


int testExample2()
{
    {
        // 创建欧拉角 (弧度) - 滚转10°, 俯仰20°, 偏航30°
        Euler euler{ 0.1745, 0.3491, 0.5236 };

        // 转换为旋转矩阵 (ZYX顺序)
        Matrix3d rot_mat;
        euler.toMatrix(Euler::eZYX, rot_mat);

        std::cout << "欧拉角转矩阵完成" << std::endl;
    }

    {
        // 另一个作用域：转换为四元数
        Euler euler{ 0.1745, 0.3491, 0.5236 };
        Quaternion quat;
        euler.toQuat(Euler::eZYX, quat);

        std::cout << "四元数: " << quat.qs() << ", " << quat.qx()
            << ", " << quat.qy() << ", " << quat.qz() << std::endl;
    }

    return 0;
}


int testExample3()
{
    {
        Quaternion q = { 0.5, 0.5, 0.5, 0.5 };

        // 归一化
        q.normalize();

        std::cout << "归一化后范数: " << q.norm() << std::endl;
    }

    {
        // 获取归一化副本
        Quaternion q = { 1.0, 2.0, 3.0, 4.0 };
        Quaternion q_normalized = q.normalized();

        std::cout << "归一化副本范数: " << q_normalized.norm() << std::endl;
    }

    {
        // 设置为单位四元数
        Quaternion q;
        q.setIdentity();

        std::cout << "单位四元数: " << q.qs() << ", " << q.qx()
            << ", " << q.qy() << ", " << q.qz() << std::endl;
    }

    return 0;
}

int testExample4()
{
    // 初始欧拉角 (滚转, 俯仰, 偏航)
    Euler initial_euler{ 0.3, 0.2, 0.1 };
    std::cout << "初始欧拉角: " << initial_euler.angle1() << ", "
        << initial_euler.angle2() << ", " << initial_euler.angle3() << std::endl;

    {
        // 欧拉角 -> 四元数
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);
        std::cout << "转换后的四元数: " << quat.qs() << ", " << quat.qx()
            << ", " << quat.qy() << ", " << quat.qz() << std::endl;
    }

    {
        // 四元数 -> 矩阵
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);

        Matrix3d matrix;
        aQuatToMatrix(quat, matrix);
        std::cout << "转换到矩阵完成" << std::endl;
    }

    {
        // 矩阵 -> 欧拉角 (完整循环)
        Quaternion quat;
        initial_euler.toQuat(Euler::eZYX, quat);

        Matrix3d matrix;
        aQuatToMatrix(quat, matrix);

        Euler final_euler;
        aMatrixToEuler(matrix, Euler::eZYX, final_euler);

        std::cout << "最终欧拉角: " << final_euler.angle1() << ", "
            << final_euler.angle2() << ", " << final_euler.angle3() << std::endl;
    }

    return 0;
}

int main()
{
    int rc = 0;
    rc |= testQuatAndMatrix();
    rc |= testEulerAndMatrix();
    rc |= testExample1();
    rc |= testExample2();
    rc |= testExample3();
    rc |= testExample4();

    return rc;
}