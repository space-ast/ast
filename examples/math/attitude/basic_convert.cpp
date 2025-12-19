#include "AstCore/AttitudeConvert.hpp"
#include "AstCore/Euler.hpp"
#include "AstCore/Quaternion.hpp"
#include <iostream>


int main() {
    AST_USING_NAMESPACE 
    // 创建四元数 (绕Z轴旋转90度)
    Quaternion quat = {0.707, 0, 0, 0.707};
    
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