#include "AstCore/OrbitParam.hpp"
#include "AstUtil/Constants.h"

int main() {
    AST_USING_NAMESPACE
    // 真近点角转偏近点角
    double trueAnomaly = 30.0 * kDegToRad;
    double eccentricity = 0.1;
    double eccentricAnomaly = aTrueToEcc(trueAnomaly, eccentricity);

    // 偏近点角转平近点角
    double meanAnomaly = aEccToMean(eccentricAnomaly, eccentricity);

    // 反向转换 - 平近点角转真近点角（迭代求解）
    double tolerance = 1e-15;
    int maxIter = 50;
    double trueAnomalyReconverted = aMeanToTrue(meanAnomaly, eccentricity, tolerance, maxIter);
}
