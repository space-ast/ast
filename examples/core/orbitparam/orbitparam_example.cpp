#include "AstCore/OrbitParam.hpp"

int main() {
    AST_USING_NAMESPACE
    // 地球参数
    double earthRadius = 6371000.0;
    double gmEarth = 3.986004418e14;

    // 已知近地点高度和偏心率
    double perigeeAlt = 200000.0; // 200 km
    double eccentricity = 0.01;

    // 计算其他轨道参数
    double perigeeRad = aPeriAltToPeriRad(perigeeAlt, earthRadius);
    double apogeeAlt = aPeriAltToApoAlt(perigeeAlt, eccentricity, earthRadius);
    double semiMajorAxis = aPeriRadToSMajAx(perigeeRad, eccentricity);
    double period = aPeriRadToPeriod(perigeeRad, eccentricity, gmEarth);
    double meanMotion = aSMajAxToMeanMotn(semiMajorAxis, gmEarth);
}