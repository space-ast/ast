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
#include "AstMath/AngleAxis.hpp"
#include "AstCore/Constants.h"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE


void testQuatAnsMatrix(const Quaternion& quatInput)
{
    Quaternion quat2, quat = quatInput;
    quat.normalize();
    Matrix3d mtx;
    aQuatToMatrix(quat, mtx);
    aMatrixToQuat(mtx, quat2);
    for (size_t i = 0; i < _ASTMATH size(quat2); i++)
    {
        EXPECT_NEAR(quat[i], quat2[i], 1e-14);
    }
}

TEST(AttitudeConvertTest, QuatAndMatrix)
{
    {
        Matrix3d mtx = Matrix3d::Identity();
        Matrix3d mtx2;
        Quaternion quat;
        aMatrixToQuat(mtx, quat);
        aQuatToMatrix(quat, mtx2);
        EXPECT_EQ(quat.qs(), 1);
        EXPECT_EQ(quat.qx(), 0);
        EXPECT_EQ(quat.qy(), 0);
        EXPECT_EQ(quat.qz(), 0);
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                EXPECT_EQ(mtx(i,j), mtx2(i,j));
            }
        }

    }

    testQuatAnsMatrix({ 1,0,0,0 });
    testQuatAnsMatrix({ 1,2,3,4 });
    testQuatAnsMatrix({ 2,-2,-1,2 });

}


void testEulerAndMatrix(int seq, const Euler& eulerInput)
{
    Euler euler = eulerInput;
    Euler euler2;
    Matrix3d mtx, mtx2;
    aEulerToMatrix(euler, seq, mtx);
    _aEulerToMatrix(euler, seq, mtx2);
    aMatrixToEuler(mtx, seq, euler2);
    for (int i = 0; i < 9; i++)
    {
        EXPECT_NEAR(mtx(i), mtx2(i), 1e-14);
    }
    for (int i = 0; i < _ASTMATH size(euler); i++)
    {
        EXPECT_NEAR(euler[i], euler2[i], 1e-14);
    }
}

void testEulerAndMatrix(const Euler& eulerInput)
{
    testEulerAndMatrix(123, eulerInput);
    testEulerAndMatrix(132, eulerInput);
    testEulerAndMatrix(213, eulerInput);
    testEulerAndMatrix(231, eulerInput);
    testEulerAndMatrix(312, eulerInput);
    testEulerAndMatrix(321, eulerInput);
    testEulerAndMatrix(121, eulerInput);
    testEulerAndMatrix(131, eulerInput);
    testEulerAndMatrix(212, eulerInput);
    testEulerAndMatrix(232, eulerInput);
    testEulerAndMatrix(313, eulerInput);
    testEulerAndMatrix(323, eulerInput);
}


TEST(AttitudeConvertTest, EulerAndMatrix)
{
    testEulerAndMatrix({ 0,0,0 });
    testEulerAndMatrix({ 1,0,0 });
    testEulerAndMatrix({ 1,1.2,3 });
    testEulerAndMatrix({ -1,1.1,3 });
    testEulerAndMatrix({ -1,1.2,-2 });
    testEulerAndMatrix({ -0.4,0.1,-2 });
}

TEST(AttitudeConvertTest, Example1)
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

}


TEST(AttitudeConvertTest, Example2)
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

}


TEST(AttitudeConvertTest, Example3)
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

}

TEST(AttitudeConvertTest, Example4)
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

}


// 测试四元数与轴角转换
TEST(AttitudeConvertTest, QuatAndAngleAxis)
{
    // 测试用例1: 单位四元数 (无旋转)
    Quaternion quat1{ 1.0, 0.0, 0.0, 0.0 };
    AngleAxis aa1;
    aQuatToAngleAxis(quat1, aa1);
    EXPECT_NEAR(aa1.angle(), 0.0, 1e-14);
    EXPECT_NEAR(aa1.axis()[0], 1.0, 1e-14);
    EXPECT_NEAR(aa1.axis()[1], 0.0, 1e-14);
    EXPECT_NEAR(aa1.axis()[2], 0.0, 1e-14);

    // 测试用例2: 绕Z轴旋转90度
    Quaternion quat2{ 0.7071067811865476, 0.0, 0.0, 0.7071067811865476 };
    AngleAxis aa2;
    aQuatToAngleAxis(quat2, aa2);
    EXPECT_NEAR(aa2.angle(), kPI / 2.0, 1e-14);
    EXPECT_NEAR(aa2.axis()[0], 0.0, 1e-14);
    EXPECT_NEAR(aa2.axis()[1], 0.0, 1e-14);
    EXPECT_NEAR(aa2.axis()[2], 1.0, 1e-14);

    // 测试用例3: 绕任意轴旋转
    Vector3d axis{1.0, 1.0, 1.0};
    axis.normalize();
    AngleAxis aa3(kPI / 3.0, axis);
    Quaternion quat3;
    aAngleAxisToQuat(aa3, quat3);
    
    AngleAxis aa3_check;
    aQuatToAngleAxis(quat3, aa3_check);
    EXPECT_NEAR(aa3_check.angle(), aa3.angle(), 1e-14);
    EXPECT_NEAR(aa3_check.axis()[0], aa3.axis()[0], 1e-14);
    EXPECT_NEAR(aa3_check.axis()[1], aa3.axis()[1], 1e-14);
    EXPECT_NEAR(aa3_check.axis()[2], aa3.axis()[2], 1e-14);

    {
        // 测试用例: 四元数实部为负
        // 将实部取负，虚部不变，表示旋转角度取反或者旋转轴取反
        // 将四元数全部取负，相当于多旋转了360度
        Quaternion quat2{ -0.7071067811865476, 0.0, 0.0, 0.7071067811865476 };
        AngleAxis aa2;
        aQuatToAngleAxis(quat2, aa2);
        EXPECT_NEAR(aa2.angle(), kPI / 2.0, 1e-14);
        EXPECT_NEAR(aa2.axis()[0], 0.0, 1e-14);
        EXPECT_NEAR(aa2.axis()[1], 0.0, 1e-14);
        EXPECT_NEAR(aa2.axis()[2], 1.0, 1e-14);
        Quaternion quat3;
        aAngleAxisToQuat(aa2, quat3);
        EXPECT_NEAR(quat3.qs(), 0.7071067811865476, 1e-14);
        EXPECT_NEAR(quat3.qx(), 0.0, 1e-14);
        EXPECT_NEAR(quat3.qy(), 0.0, 1e-14);
        EXPECT_NEAR(quat3.qz(), -0.7071067811865476, 1e-14);
    }
}

// 测试轴角与矩阵转换
TEST(AttitudeConvertTest, AngleAxisAndMatrix)
{
    // 测试用例1: 单位矩阵
    Matrix3d mtx1 = Matrix3d::Identity();
    AngleAxis aa1;
    aMatrixToAngleAxis(mtx1, aa1);
    EXPECT_NEAR(aa1.angle(), 0.0, 1e-14);
    
    Matrix3d mtx1_check;
    aAngleAxisToMatrix(aa1, mtx1_check);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            EXPECT_NEAR(mtx1(i, j), mtx1_check(i, j), 1e-14);
        }
    }

    // 测试用例2: 绕X轴旋转45度
    AngleAxis aa2(kPI / 4.0, Vector3d{1.0, 0.0, 0.0});
    Matrix3d mtx2;
    aAngleAxisToMatrix(aa2, mtx2);
    
    AngleAxis aa2_check;
    aMatrixToAngleAxis(mtx2, aa2_check);
    EXPECT_NEAR(aa2_check.angle(), aa2.angle(), 1e-14);
    EXPECT_NEAR(aa2_check.axis()[0], aa2.axis()[0], 1e-14);
    EXPECT_NEAR(aa2_check.axis()[1], aa2.axis()[1], 1e-14);
    EXPECT_NEAR(aa2_check.axis()[2], aa2.axis()[2], 1e-14);
}

// 测试欧拉角与四元数转换
TEST(AttitudeConvertTest, EulerAndQuat)
{
    // 测试用例1: 零角度欧拉角
    Euler euler1{ 0.0, 0.0, 0.0 };
    Quaternion quat1;
    err_t rc = aEulerToQuat(euler1, Euler::eXYZ, quat1);
    EXPECT_EQ(rc, 0);
    EXPECT_NEAR(quat1.qs(), 1.0, 1e-14);
    EXPECT_NEAR(quat1.qx(), 0.0, 1e-14);
    EXPECT_NEAR(quat1.qy(), 0.0, 1e-14);
    EXPECT_NEAR(quat1.qz(), 0.0, 1e-14);

    // 测试用例2: 各种旋转顺序
    Euler euler2{ 0.1, 0.2, 0.3 };
    int sequences[] = { Euler::eXYZ, Euler::eXZY, Euler::eYXZ, Euler::eYZX, Euler::eZXY, Euler::eZYX, 
                        Euler::eXYX, Euler::eXZX, Euler::eYXY, Euler::eYZY, Euler::eZXZ, Euler::eZYZ };
    
    for (int seq : sequences)
    {
        Quaternion quat;
        err_t rc = aEulerToQuat(euler2, seq, quat);
        EXPECT_EQ(rc, 0);
        
        Euler euler_check;
        rc = aQuatToEuler(quat, seq, euler_check);
        EXPECT_EQ(rc, 0);
        
        // 由于欧拉角的多解性，我们通过矩阵来验证转换的正确性
        Matrix3d mtx1, mtx2;
        aEulerToMatrix(euler2, seq, mtx1);
        aEulerToMatrix(euler_check, seq, mtx2);
        
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                EXPECT_NEAR(mtx1(i, j), mtx2(i, j), 1e-14);
            }
        }
    }
}

// 测试特定的欧拉角序列转换函数
TEST(AttitudeConvertTest, SpecificEulerSequences)
{
    // 测试用例: 各种欧拉角序列的直接转换函数
    Euler euler{ 0.1, 0.2, 0.3 };
    
    // 测试ABC类型序列
    Matrix3d mtx123, mtx132, mtx213, mtx231, mtx312, mtx321;
    aEuler123ToMatrix(euler, mtx123);
    aEuler132ToMatrix(euler, mtx132);
    aEuler213ToMatrix(euler, mtx213);
    aEuler231ToMatrix(euler, mtx231);
    aEuler312ToMatrix(euler, mtx312);
    aEuler321ToMatrix(euler, mtx321);
    
    Euler euler123, euler132, euler213, euler231, euler312, euler321;
    aMatrixToEuler123(mtx123, euler123);
    aMatrixToEuler132(mtx132, euler132);
    aMatrixToEuler213(mtx213, euler213);
    aMatrixToEuler231(mtx231, euler231);
    aMatrixToEuler312(mtx312, euler312);
    aMatrixToEuler321(mtx321, euler321);
    
    // 验证转换的正确性
    EXPECT_NEAR(euler123.angle1(), euler.angle1(), 1e-14);
    EXPECT_NEAR(euler123.angle2(), euler.angle2(), 1e-14);
    EXPECT_NEAR(euler123.angle3(), euler.angle3(), 1e-14);
    
    EXPECT_NEAR(euler132.angle1(), euler.angle1(), 1e-14);
    EXPECT_NEAR(euler132.angle2(), euler.angle2(), 1e-14);
    EXPECT_NEAR(euler132.angle3(), euler.angle3(), 1e-14);
    
    EXPECT_NEAR(euler213.angle1(), euler.angle1(), 1e-14);
    EXPECT_NEAR(euler213.angle2(), euler.angle2(), 1e-14);
    EXPECT_NEAR(euler213.angle3(), euler.angle3(), 1e-14);
    
    EXPECT_NEAR(euler231.angle1(), euler.angle1(), 1e-14);
    EXPECT_NEAR(euler231.angle2(), euler.angle2(), 1e-14);
    EXPECT_NEAR(euler231.angle3(), euler.angle3(), 1e-14);
    
    EXPECT_NEAR(euler312.angle1(), euler.angle1(), 1e-14);
    EXPECT_NEAR(euler312.angle2(), euler.angle2(), 1e-14);
    EXPECT_NEAR(euler312.angle3(), euler.angle3(), 1e-14);
    
    EXPECT_NEAR(euler321.angle1(), euler.angle1(), 1e-14);
    EXPECT_NEAR(euler321.angle2(), euler.angle2(), 1e-14);
    EXPECT_NEAR(euler321.angle3(), euler.angle3(), 1e-14);
    
    // 测试ABA类型序列
    Matrix3d mtx121, mtx131, mtx212, mtx232, mtx313, mtx323;
    aEuler121ToMatrix(euler, mtx121);
    aEuler131ToMatrix(euler, mtx131);
    aEuler212ToMatrix(euler, mtx212);
    aEuler232ToMatrix(euler, mtx232);
    aEuler313ToMatrix(euler, mtx313);
    aEuler323ToMatrix(euler, mtx323);
    
    Euler euler121, euler131, euler212, euler232, euler313, euler323;
    aMatrixToEuler121(mtx121, euler121);
    aMatrixToEuler131(mtx131, euler131);
    aMatrixToEuler212(mtx212, euler212);
    aMatrixToEuler232(mtx232, euler232);
    aMatrixToEuler313(mtx313, euler313);
    aMatrixToEuler323(mtx323, euler323);
    
    // 验证转换的正确性
    Matrix3d mtx_check;
    
    aEuler121ToMatrix(euler121, mtx_check);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            EXPECT_NEAR(mtx121(i, j), mtx_check(i, j), 1e-14);
        }
    }
    
    aEuler131ToMatrix(euler131, mtx_check);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            EXPECT_NEAR(mtx131(i, j), mtx_check(i, j), 1e-14);
        }
    }
    
    aEuler212ToMatrix(euler212, mtx_check);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            EXPECT_NEAR(mtx212(i, j), mtx_check(i, j), 1e-14);
        }
    }
    
    aEuler232ToMatrix(euler232, mtx_check);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            EXPECT_NEAR(mtx232(i, j), mtx_check(i, j), 1e-14);
        }
    }
    
    aEuler313ToMatrix(euler313, mtx_check);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            EXPECT_NEAR(mtx313(i, j), mtx_check(i, j), 1e-14);
        }
    }
    
    aEuler323ToMatrix(euler323, mtx_check);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            EXPECT_NEAR(mtx323(i, j), mtx_check(i, j), 1e-14);
        }
    }
}


GTEST_MAIN()