#include "AstMath/ODE.hpp"
#include "AstGlobal.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <iomanip>

AST_USING_NAMESPACE


/// @brief 范德波尔振荡器：d²x/dt² - μ(1-x²)dx/dt + x = 0
class VanDerPolOscillator : public ODE {
private:
    double mu_;  // 非线性参数
    
public:
    VanDerPolOscillator(double mu = 1.0) : mu_(mu) {}
    
    err_t evaluate(const double* y, double* ydot, double t) override {
        // y[0] = x, y[1] = dx/dt
        ydot[0] = y[1];  // dx/dt = v
        ydot[1] = mu_ * (1.0 - y[0] * y[0]) * y[1] - y[0];  // dv/dt = μ(1-x²)v - x
        return eNoError;
    }
    
    int getDimension() const override {
        return 2;
    }
};

/// @brief 性能测试结果结构
struct PerformanceResult {
    std::string integrator_name;
    double final_value;
    double error;
    long long time_ns;
    int steps;
    
    PerformanceResult(const std::string& name, double value, double err, long long time, int step_count)
        : integrator_name(name), final_value(value), error(err), time_ns(time), steps(step_count) {}
};

int main() {
    std::setlocale(LC_ALL, ".UTF-8");
    std::cout << "=== ODE积分器性能比较示例 ===" << std::endl;
    
    // 测试方程：范德波尔振荡器
    double mu = 5.0;  
    VanDerPolOscillator vdpODE(mu);
    
    // 积分参数
    double t0 = 0.0;
    double tf = 10.0;
    double y0[2] = {2.0, 0.0};  // x(0)=2, v(0)=0
    
    // 参考解（使用高精度积分器计算）
    RKF78 reference_integrator;
    reference_integrator.setMaxAbsErr(1e-12);
    reference_integrator.setMaxRelErr(1e-12);
    reference_integrator.setInitialStepSize(0.5);
    reference_integrator.initialize(vdpODE);
    
    double y_ref[2]{y0[0], y0[1]};
    double t = t0;
    err_t err = reference_integrator.integrate(vdpODE, y_ref, t, tf);
    
    printf("err = %d\n", err);
    std::cout << "参考解: x(" << tf << ") = " << y_ref[0] << ", v(" << tf << ") = " << y_ref[1] << std::endl;
    std::cout << std::endl;
    
    // 测试的积分器列表
    std::vector<std::pair<std::string, ODEIntegrator*>> integrators;
    
    // 定步长积分器
    RK4* rk4 = new RK4();
    rk4->setStepSize(1e-2);
    integrators.emplace_back("RK4", rk4);
    RK8* rk8 = new RK8();
    rk8->setStepSize(1e-2);
    integrators.emplace_back("RK8", rk8);
    RKV8* rkv8 = new RKV8();
    rkv8->setStepSize(1e-2);
    integrators.emplace_back("RKV8", rkv8);
    
    // 变步长积分器
    RKF45* rkf45 = new RKF45();
    rkf45->setMaxAbsErr(1e-6);
    rkf45->setMaxRelErr(1e-6);
    integrators.emplace_back("RKF45", rkf45);
    
    RKF56* rkf56 = new RKF56();
    rkf56->setMaxAbsErr(1e-6);
    rkf56->setMaxRelErr(1e-6);
    integrators.emplace_back("RKF56", rkf56);
    
    RKF78* rkf78 = new RKF78();
    rkf78->setMaxAbsErr(1e-6);
    rkf78->setMaxRelErr(1e-6);
    rkf78->setInitialStepSize(0.5);
    integrators.emplace_back("RKF78", rkf78);
    
    RKCK* rkck = new RKCK();
    rkck->setMaxAbsErr(1e-6);
    rkck->setMaxRelErr(1e-6);
    integrators.emplace_back("RKCK", rkck);
    
    // 性能测试结果
    std::vector<PerformanceResult> results;
    
    for (auto& integrator_pair : integrators) {
        std::string name = integrator_pair.first;
        ODEIntegrator* integrator = integrator_pair.second;
        
        std::cout << "测试积分器: " << name << std::endl;
        
        // 初始化积分器
        integrator->initialize(vdpODE);
        
        // 测量执行时间
        auto start_time = std::chrono::high_resolution_clock::now();
        
        double yf[2]{y0[0], y0[1]};
        t = t0;
        err_t result = integrator->integrate(vdpODE, yf, t, tf);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        
        if (result == eNoError) {
            // 计算误差
            double error_x = std::abs(yf[0] - y_ref[0]);
            double error_v = std::abs(yf[1] - y_ref[1]);
            double max_error = std::max(error_x, error_v);
            
            // 获取步数信息（对于变步长积分器）
            int step_count = 0;
            ODEVarStepIntegrator* var_integrator = dynamic_cast<ODEVarStepIntegrator*>(integrator);
            if (var_integrator != nullptr) {
                step_count = var_integrator->getNumSteps();
            } else {
                // 对于定步长积分器，估算步数
                step_count = static_cast<int>((tf - t0) / 0.01);  // 假设步长为0.01
            }
            
            results.emplace_back(name, yf[0], max_error, duration.count(), step_count);
            
            std::cout << "  结果: x=" << yf[0] << ", v=" << yf[1] << std::endl;
            std::cout << "  误差: " << max_error << std::endl;
            std::cout << "  时间: " << duration.count() / 1e6 << " ms" << std::endl;
            std::cout << "  步数: " << step_count << std::endl;
        } else {
            std::cout << "  积分失败" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // 输出性能比较表格
    std::cout << "=== 性能比较结果 ===" << std::endl;
    std::cout << std::setw(10) << "积分器" 
              << std::setw(15) << "位置误差" 
              << std::setw(12) << "时间(ms)" 
              << std::setw(10) << "步数" 
              << std::setw(15) << "效率指标" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& result : results) {
        // 效率指标：误差越小、时间越短越好
        double efficiency = result.error * result.time_ns / 1e6;  // 误差×时间
        
        std::cout << std::setw(10) << result.integrator_name
                  << std::setw(15) << std::scientific << std::setprecision(2) << result.error
                  << std::setw(12) << std::fixed << std::setprecision(3) << result.time_ns / 1e6
                  << std::setw(10) << result.steps
                  << std::setw(15) << std::scientific << std::setprecision(2) << efficiency
                  << std::endl;
    }
    
    // 清理内存
    for (auto& integrator_pair : integrators) {
        delete integrator_pair.second;
    }
    
    // 不同步长对定步长积分器的影响
    std::cout << "\n=== 定步长积分器步长影响 ===" << std::endl;
    
    std::vector<double> step_sizes = {0.1, 0.05, 0.01, 0.005, 0.001};
    
    for (double step : step_sizes) {
        RK4 rk4;
        rk4.initialize(vdpODE);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // 手动执行定步长积分
        double t = t0;
        double y_current[2] = {y0[0], y0[1]};
        int step_count = 0;
        
        while (t < tf) {
            double actual_step = step;
            if (t + step > tf) {
                actual_step = tf - t;
            }
            
            rk4.singleStep(vdpODE, y_current, t, actual_step);
            
            t += actual_step;
            step_count++;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        
        double error_x = std::abs(y_current[0] - y_ref[0]);
        double error_v = std::abs(y_current[1] - y_ref[1]);
        double max_error = std::max(error_x, error_v);
        
        std::cout << "步长=" << step << ": 误差=" << max_error 
                  << ", 时间=" << duration.count() / 1e6 << " ms"
                  << ", 步数=" << step_count << std::endl;
    }
    
    std::cout << "\n=== 示例完成 ===" << std::endl;
    
    return 0;
}