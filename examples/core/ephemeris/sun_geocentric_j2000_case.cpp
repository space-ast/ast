#include "ast/RunTime.hpp"
#include "ast/TimePoint.hpp"
#include "ast/FrameTransform.hpp"
#include "ast/JplDe.hpp"
#include "ast/EOP.hpp"
#include "ast/Vector.hpp"
#include <cstdio>

AST_USING_NAMESPACE

int main()
{
    errc_t rc;
    rc = aInitialize();
    if (rc != eNoError) {
        std::printf("aInitialize failed: %d\n", (int)rc);
        return 2;
    }
    // 卸载EOP数据
    aDataContext_GetEOP()->unload();

    const TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);

    Vector3d posICRF;
    Vector3d velICRF;
    rc = aJplDeGetPosVelICRF(tp, JplDe::eSun, JplDe::eEarth, posICRF, velICRF);
    if (rc != eNoError) {
        std::printf("aJplDeGetPosVelICRF failed: %d\n", (int)rc);
        aUninitialize();
        return 3;
    }

    Vector3d posJ2000;
    Vector3d velJ2000;
    aICRFToJ2000(tp, posICRF, velICRF, posJ2000, velJ2000);

    std::printf("UTC = 2026-01-01 00:00:00\n");
    std::printf("JPL DE version = %d\n", aJplDeNum());
    std::printf("\n");

    std::printf("Sun wrt Earth, ICRF:\n");
    std::printf("  r [m]   = %.12f, %.12f, %.12f\n", posICRF.x(), posICRF.y(), posICRF.z());
    std::printf("  v [m/s] = %.12f, %.12f, %.12f\n", velICRF.x(), velICRF.y(), velICRF.z());
    std::printf("  r [km]  = %.12f, %.12f, %.12f\n", posICRF.x() / 1000.0, posICRF.y() / 1000.0, posICRF.z() / 1000.0);
    std::printf("  v [km/s]= %.12f, %.12f, %.12f\n", velICRF.x() / 1000.0, velICRF.y() / 1000.0, velICRF.z() / 1000.0);
    std::printf("\n");

    std::printf("Sun wrt Earth, J2000:\n");
    std::printf("  r [m]   = %.12f, %.12f, %.12f\n", posJ2000.x(), posJ2000.y(), posJ2000.z());
    std::printf("  v [m/s] = %.12f, %.12f, %.12f\n", velJ2000.x(), velJ2000.y(), velJ2000.z());
    std::printf("  r [km]  = %.12f, %.12f, %.12f\n", posJ2000.x() / 1000.0, posJ2000.y() / 1000.0, posJ2000.z() / 1000.0);
    std::printf("  v [km/s]= %.12f, %.12f, %.12f\n", velJ2000.x() / 1000.0, velJ2000.y() / 1000.0, velJ2000.z() / 1000.0);

    aUninitialize();
    return 0;
}
