#include "AstCore/OrbitParam.hpp"

int main() {
    AST_USING_NAMESPACE
    // 地球平均半径 (m)
    double earthRadius = 6371000.0;

    // 近地点高度转半径
    double perigeeAlt = 300000.0; // 300 km
    double perigeeRadius = aPeriAltToPeriRad(perigeeAlt, earthRadius);

    // 远地点半径转高度
    double apogeeRadius = 42164000.0; // 地球同步轨道半径
    double apogeeAlt = aApoRadToApoAlt(apogeeRadius, earthRadius);
}
