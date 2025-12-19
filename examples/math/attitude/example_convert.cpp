#include "AstCore/AttitudeConvert.hpp"
#include "AstCore/Euler.hpp"
#include "AstCore/Quaternion.hpp"
#include <iostream>

int main() {
    AST_USING_NAMESPACE
    // 初始欧拉角 (滚转, 俯仰, 偏航)
    Euler initial_euler{0.3, 0.2, 0.1};
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
        std::cout << "转换到矩阵完成 " << std::endl;
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