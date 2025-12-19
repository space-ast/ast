#include "AstCore/Quaternion.hpp"
#include <iostream>

int main() {
    AST_USING_NAMESPACE
    {
        Quaternion q = {0.5, 0.5, 0.5, 0.5};
        
        // 归一化
        q.normalize();
        
        std::cout << "归一化后范数: " << q.norm() << std::endl;
    }
    
    {
        // 获取归一化副本
        Quaternion q = {1.0, 2.0, 3.0, 4.0};
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