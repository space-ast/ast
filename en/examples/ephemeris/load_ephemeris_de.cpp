// Full source code for en/examples/ephemeris/load_ephemeris_de.md

#include "ast/JplDe.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Vector.hpp"
#include <iostream>
#include <iomanip>
#include <clocale>

AST_USING_NAMESPACE

int main()
{
    // Create a JplDe instance and load a JPL DE ephemeris file (replace with the actual path)
    JplDe de;
    errc_t err = de.open("./data/SolarSystem/plneph.430");
    if (err != eNoError) {
        std::cerr << "Failed to load the DE ephemeris file" << std::endl;
        return -1;
    }

    // Get the ephemeris time interval
    TimeInterval interval;
    de.getInterval(interval);
    std::cout << "ephemeris time span: " << std::endl;
    std::cout << "  start: " << interval.getStart().toString() << std::endl;
    std::cout << "  end:   " << interval.getStop().toString() << std::endl;

    // Query the body state at 2000-01-01 12:00:00 UTC
    TimePoint time = TimePoint::FromUTC(2000, 1, 1, 12, 0, 0.0);

    Vector3d pos, vel;

    // Position and velocity of the Earth relative to the solar system barycenter
    err = de.getPosVelICRF(time, JplDe::eEarth, JplDe::eSSBarycenter, pos, vel);
    if (err != eNoError) {
        std::cerr << "Failed to get the Earth position and velocity, error code: " << err << std::endl;
    } else {
        std::cout << "\nEarth state relative to the solar system barycenter (ICRF):" << std::endl;
        std::cout << "  position [m]:   " << std::setprecision(6)
                  << pos.x() << "  " << pos.y() << "  " << pos.z() << std::endl;
        std::cout << "  velocity [m/s]: " << std::setprecision(6)
                  << vel.x() << "  " << vel.y() << "  " << vel.z() << std::endl;
    }

    // Position of the Moon relative to the Earth, used to compute the Earth-Moon distance
    err = de.getPosVelICRF(time, JplDe::eMoon, JplDe::eEarth, pos, vel);
    if (err != eNoError) {
        std::cerr << "Failed to get the Moon position and velocity, error code: " << err << std::endl;
    } else {
        std::cout << "\nMoon position relative to the Earth [m]:" << std::endl;
        std::cout << "  X: " << std::setprecision(6) << pos.x() << std::endl;
        std::cout << "  Y: " << std::setprecision(6) << pos.y() << std::endl;
        std::cout << "  Z: " << std::setprecision(6) << pos.z() << std::endl;
        std::cout << "  Earth-Moon distance: " << std::setprecision(6) << pos.norm() << " m" << std::endl;
    }

    de.close();

    return 0;
}
