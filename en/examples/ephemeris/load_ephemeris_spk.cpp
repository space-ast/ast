// Full source code for en/examples/ephemeris/load_ephemeris_spk.md

#include "ast/JplSpk.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Vector.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

AST_USING_NAMESPACE

int main()
{
    // Create a JplSpk instance and load an SPK ephemeris file (replace with the actual path)
    JplSpk spk;
    errc_t err = spk.open("./data/kernels/spk/de430.bsp");
    if (err != eNoError) {
        std::cerr << "Failed to load the SPK ephemeris file" << std::endl;
        return -1;
    }

    // Get the ephemeris time interval for the Earth (NAIF ID 399)
    TimeInterval interval;
    err = spk.getInterval(399, interval);
    if (err != eNoError) {
        std::cerr << "Failed to get the ephemeris time interval, error code: " << err << std::endl;
    } else {
        std::cout << "ephemeris time span: " << std::endl;
        std::cout << "  start: " << interval.getStart().toString() << std::endl;
        std::cout << "  end:   " << interval.getStop().toString() << std::endl;
    }

    // List the bodies in the file
    std::vector<std::string> names;
    err = spk.getBodyNames(names);
    if (err != eNoError) {
        std::cerr << "Failed to get the body list, error code: " << err << std::endl;
    } else {
        std::cout << "\nbodies in the file:" << std::endl;
        for (const auto& name : names) {
            std::cout << "  " << name << std::endl;
        }
    }

    // Query the body state at 2000-01-01 12:00:00 UTC
    TimePoint time = TimePoint::FromUTC(2000, 1, 1, 12, 0, 0.0);

    Vector3d pos, vel;

    // Position and velocity of the Earth relative to the Earth-Moon barycenter (NAIF ID 3)
    err = JplSpk::getPosVelICRF(time, 399, 3, pos, vel);
    if (err != eNoError) {
        std::cerr << "Failed to get the Earth position and velocity, error code: " << err << std::endl;
    } else {
        std::cout << "\nEarth state relative to the Earth-Moon barycenter (ICRF):" << std::endl;
        std::cout << "  position [m]:   " << std::setprecision(6)
                  << pos.x() << "  " << pos.y() << "  " << pos.z() << std::endl;
        std::cout << "  velocity [m/s]: " << std::setprecision(6)
                  << vel.x() << "  " << vel.y() << "  " << vel.z() << std::endl;
    }

    // Position of the Moon relative to the Earth-Moon barycenter; subtract the Earth position to get the Moon position relative to the Earth
    Vector3d earthPos, earthVel;
    Vector3d moonPos, moonVel;

    errc_t errEarth = JplSpk::getPosVelICRF(time, 399, 3, earthPos, earthVel);
    errc_t errMoon = JplSpk::getPosVelICRF(time, 301, 3, moonPos, moonVel);
    if (errEarth != eNoError || errMoon != eNoError) {
        std::cerr << "Failed to get the Earth and Moon position and velocity, error codes: " << errEarth << ", " << errMoon << std::endl;
    } else {
        Vector3d moonRelEarth = moonPos - earthPos;
        std::cout << "\nMoon position relative to the Earth [m]:" << std::endl;
        std::cout << "  X: " << std::setprecision(6) << moonRelEarth.x() << std::endl;
        std::cout << "  Y: " << std::setprecision(6) << moonRelEarth.y() << std::endl;
        std::cout << "  Z: " << std::setprecision(6) << moonRelEarth.z() << std::endl;
        std::cout << "  Earth-Moon distance: " << std::setprecision(6) << moonRelEarth.norm() << " m" << std::endl;
    }

    spk.close();

    return 0;
}
