#include "AstMath/ODE.hpp"
#include "AstGlobal.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>

AST_USING_NAMESPACE

/// @brief 刚性方程示例：dy/dt = -1000(y - sin(t)) + cos(t)
class StiffODE : public ODE {
public:
    err_t evaluate(double t, const double* y, double* ydot) override {
        ydot[0] = -1000.0 * (y[0] - std::sin(t)) + std::cos(t);
        return eNoError;
    }
    
    int getDimension() const override {
        return 1;
    }
};



int main() {
    std::setlocale(LC_ALL, ".UTF-8");
    std::cout << "=== 变步长积分器自适应步长控制示例 ===" << std::endl;
    
    StiffODE stiffODE;
    
    // 测试不同的变步长积分器
    std::vector<std::pair<std::string, ODEVarStepIntegrator*>> integrators = {
        {"RKF45", new RKF45()},
        {"RKF78", new RKF78()},
        {"RKCK", new RKCK()}
    };
    
    for (auto& integrator_pair : integrators) {
        std::string name = integrator_pair.first;
        ODEVarStepIntegrator* integrator = integrator_pair.second;
        
        std::cout << "\n=== " << name << " 积分器 ===" << std::endl;
        
        // 配置积分器参数
        integrator->setMaxAbsErr(1e-14);
        integrator->setMaxRelErr(1e-12);
        integrator->initialize(stiffODE);
        
        // 初始条件
        double t0 = 0.0;
        double tf = 0.5;
        double y0[1] = {2.0};  // y(0) = 2
        
        // 使用积分器进行积分
        double yf[1];
        err_t result = integrator->integrate(stiffODE, t0, tf, y0, yf);
        
        if (result == eNoError) {
            // 获取积分统计信息
            double largest_step = integrator->getLargestStepSize();
            double smallest_step = integrator->getSmallestStepSize();
            int num_steps = integrator->getNumSteps();
            
            // 输出积分结果统计
            std::cout << "积分统计:" << std::endl;
            std::cout << "  总步数: " << num_steps << std::endl;
            std::cout << "  最小步长: " << smallest_step << std::endl;
            std::cout << "  最大步长: " << largest_step << std::endl;
            
            // 输出最终结果
            double exact = std::sin(tf) + (y0[0] - std::sin(t0)) * std::exp(-1000.0 * (tf - t0));
            double error = std::abs(yf[0] - exact);
            
            std::cout << "  数值解: y(" << tf << ") = " << yf[0] << std::endl;
            std::cout << "  解析解: y(" << tf << ") = " << exact << std::endl;
            std::cout << "  误差: " << error << std::endl;
        } else {
            std::cout << "积分失败" << std::endl;
        }
        
        delete integrator;
    }
    
    // 演示不同容差设置的影响
    std::cout << "\n=== 不同容差设置对比 ===" << std::endl;
    
    std::vector<double> tolerances = {1e-4, 1e-6, 1e-8};
    
    for (double tol : tolerances) {
        RKF45 integrator;
        integrator.setMaxAbsErr(tol);
        integrator.setMaxRelErr(tol);
        integrator.initialize(stiffODE);
        
        double t0 = 0.0;
        double tf = 0.5;
        double y0[1] = {2.0};
        double yf[1];
        
        err_t result = integrator.integrate(stiffODE, t0, tf, y0, yf);
        
        if (result == eNoError) {
            double exact = std::sin(tf) + (y0[0] - std::sin(t0)) * std::exp(-1000.0 * (tf - t0));
            double error = std::abs(yf[0] - exact);
            
            std::cout << "容差=" << tol << ": 误差=" << error 
                      << ", 步数统计=" << integrator.getNumSteps() << std::endl;
        }
    }
    
    std::cout << "\n=== 示例完成 ===" << std::endl;
    
    return 0;
}