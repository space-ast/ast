#include "AstCore/Euler.hpp"
#include "AstCore/Quaternion.hpp"
#include <iostream>

int main() {
    AST_USING_NAMESPACE
    {
        // 创建欧拉角 (弧度) - 滚转10°, 俯仰20°, 偏航30°
        Euler euler{0.1745, 0.3491, 0.5236};
        
        // 转换为旋转矩阵 (ZYX顺序)
        Matrix3d rot_mat;
        euler.toMatrix(Euler::eZYX, rot_mat);
        
        std::cout << "欧拉角转矩阵完成" << std::endl;
    }
    {
        // 另一个作用域：转换为四元数
        Euler euler{0.1745, 0.3491, 0.5236};
        Quaternion quat;
        euler.toQuat(Euler::eZYX, quat);
        
        std::cout << "四元数: " << quat.qs() << ", " << quat.qx() 
                  << ", " << quat.qy() << ", " << quat.qz() << std::endl;
    }
}