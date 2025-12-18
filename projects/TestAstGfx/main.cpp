#include "AstGfx/AstGfxAPI.hpp"
#include "AstGfx/GfxSolarSystem.hpp"
#include <iostream>

AST_USING_NAMESPACE

void test()
{
    // 初始化可视化系统
    std::cout << "正在初始化太阳系可视化系统..." << std::endl;
    if (!AstGfxInitialize()) {
        std::cerr << "初始化可视化系统失败!" << std::endl;
        return;
    }
    
    // 设置背景颜色为深蓝色
    std::cout << "设置背景颜色..." << std::endl;
    AstGfxSetBackgroundColor(0.0f, 0.0f, 0.1f, 1.0f);
    
    // 获取太阳系对象
    GfxSolarSystem* solarSystem = AstGfxAPI::instance().getSolarSystem();
    if (!solarSystem) {
        std::cerr << "获取太阳系对象失败!" << std::endl;
        return;
    }
    
    // 创建完整的太阳系
    std::cout << "正在创建太阳系..." << std::endl;
    solarSystem->createCompleteSolarSystem();
    
    // 运行可视化系统
    std::cout << "正在运行太阳系可视化..." << std::endl;
    std::cout << "按ESC键退出程序。" << std::endl;
    int result = AstGfxRun();
    
    // 检查运行结果
    if (result != 0) {
        std::cerr << "可视化系统运行异常，返回值: " << result << std::endl;
        return;
    }
    
    std::cout << "太阳系可视化测试完成。" << std::endl;
}

int main()
{
    test();
    return 0;
}