#include "AstMath/ODE.hpp"
#include <iostream>
#include <cmath>

AST_USING_NAMESPACE


/// @brief 简单的指数衰减ODE：dy/dt = -y
class ExponentialDecayODE : public ODE {
public:
    err_t evaluate(const double* y, double* ydot, double t) override {
        ydot[0] = -y[0];  // dy/dt = -y
        return eNoError;
    }
    
    int getDimension() const override {
        return 1;
    }
};

/// @brief 简谐振动ODE：d²x/dt² = -ω²x
class HarmonicOscillatorODE : public ODE {
private:
    double omega_;  // 角频率
    
public:
    HarmonicOscillatorODE(double omega = 1.0) : omega_(omega) {}
    
    err_t evaluate(const double* y, double* ydot, double t) override {
        // y[0] = x, y[1] = dx/dt
        // dx/dt = v
        // dv/dt = -ω²x
        ydot[0] = y[1];           // dx/dt = v
        ydot[1] = -omega_ * omega_ * y[0];  // dv/dt = -ω²x
        return eNoError;
    }
    
    int getDimension() const override {
        return 2;
    }
};

int main() {
    std::setlocale(LC_ALL, ".UTF-8");
    std::cout << "=== ODE积分器基本使用示例 ===" << std::endl;
    
    // 示例1：指数衰减方程
    std::cout << "\n1. 指数衰减方程 (dy/dt = -y)" << std::endl;
    std::cout << "   解析解: y(t) = y0 * exp(-t)" << std::endl;
    
    ExponentialDecayODE expODE;
    
    // 使用RK4积分器
    RK4 rk4;
    rk4.initialize(expODE);
    
    double t0 = 0.0;
    double tf = 2.0;
    double y0[1] = {1.0};  // y(0) = 1
    double yf_rk4[1]{y0[0]};
    double t = t0;
    err_t result = rk4.integrate(expODE, yf_rk4, t, tf);
    if (result == eNoError) {
        double exact = y0[0] * std::exp(-tf);
        double error = std::abs(yf_rk4[0] - exact);
        std::cout << "   RK4结果: y(" << tf << ") = " << yf_rk4[0] << std::endl;
        std::cout << "   解析解: y(" << tf << ") = " << exact << std::endl;
        std::cout << "   误差: " << error << std::endl;
    }
    
    // 使用RKF45积分器
    RKF45 rkf45;
    rkf45.setMaxAbsErr(1e-8);
    rkf45.setMaxRelErr(1e-8);
    rkf45.initialize(expODE);
    
    double yf_rkf45[1]{y0[0]};
    t = t0;
    result = rkf45.integrate(expODE, yf_rkf45, t, tf);
    if (result == eNoError) {
        double exact = y0[0] * std::exp(-tf);
        double error = std::abs(yf_rkf45[0] - exact);
        std::cout << "   RKF45结果: y(" << tf << ") = " << yf_rkf45[0] << std::endl;
        std::cout << "   解析解: y(" << tf << ") = " << exact << std::endl;
        std::cout << "   误差: " << error << std::endl;
    }
    
    // 示例2：简谐振动方程
    std::cout << "\n2. 简谐振动方程 (d²x/dt² = -ω²x)" << std::endl;
    std::cout << "   解析解: x(t) = A*cos(ωt + φ)" << std::endl;
    
    double omega = 2.0;  // 角频率
    HarmonicOscillatorODE harmonicODE(omega);
    
    // 初始条件：x(0)=1, v(0)=0
    double y0_harmonic[2] = {1.0, 0.0};
    double yf_harmonic[2]{y0_harmonic[0], y0_harmonic[1]};
    
    // 使用RKF45积分
    rkf45.initialize(harmonicODE);
    t = t0;
    result = rkf45.integrate(harmonicODE, yf_harmonic, t, tf);
    
    if (result == eNoError) {
        double exact_x = std::cos(omega * tf);  // x(t) = cos(ωt)
        double exact_v = -omega * std::sin(omega * tf);  // v(t) = -ωsin(ωt)
        
        std::cout << "   RKF45结果:" << std::endl;
        std::cout << "   位置 x(" << tf << ") = " << yf_harmonic[0] << std::endl;
        std::cout << "   速度 v(" << tf << ") = " << yf_harmonic[1] << std::endl;
        std::cout << "   解析解:" << std::endl;
        std::cout << "   位置 x(" << tf << ") = " << exact_x << std::endl;
        std::cout << "   速度 v(" << tf << ") = " << exact_v << std::endl;
        std::cout << "   位置误差: " << std::abs(yf_harmonic[0] - exact_x) << std::endl;
        std::cout << "   速度误差: " << std::abs(yf_harmonic[1] - exact_v) << std::endl;
    }
    
    std::cout << "\n=== 示例完成 ===" << std::endl;
    
    return 0;
}