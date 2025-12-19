#include "AstCore/OrbitParam.hpp"

int main() {
    AST_USING_NAMESPACE
    // 地球引力参数 (m^3/s^2)
    double gmEarth = 3.986004418e14;

    // 从长半轴计算轨道周期
    double semiMajorAxis = 42164000.0; // 地球同步轨道长半轴
    double period = aSMajAxToPeriod(semiMajorAxis, gmEarth);

    // 从轨道周期计算平均角速度
    double meanMotion = aPeriodToMeanMotn(period);
}