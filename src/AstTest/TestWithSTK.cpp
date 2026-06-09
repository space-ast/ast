///
/// @file      TestWithSTK.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-20
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "TestWithSTK.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstSim/Mover.hpp"
#include "AstLoader/MoverLoader.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/ColoredPrint.hpp"
#include "AstUtil/StringView.hpp"
#include "AstMath/UnarySolver.hpp"  // for aIsClose
#include "AstMath/Vector.hpp"


AST_NAMESPACE_BEGIN

errc_t aTestWithEmptyEphemeris(Mover& mover)
{
    ast_printf("test with empty ephemeris\n");
    int rc;
    auto motion = mover.getMotionProfile();
    if(motion == nullptr)
    {
        ast_printf("motion profile is null\n");
        return eErrorParse;
    }

    ScopedPtr<Ephemeris> ephem_spec_generated;
    rc = motion->makeEphemerisSpec(ephem_spec_generated);
    if(rc != eNoError)
    {
        ast_printf("failed to make specific ephemeris\n");
        return rc;
    }
    ScopedPtr<Ephemeris> ephem_simple_generated;
    rc = motion->makeEphemerisSimple(ephem_simple_generated);
    if(rc != eNoError)
    {
        ast_printf("failed to make simple ephemeris\n");
        return rc;
    }
    TimeInterval interval{};
    rc = ephem_simple_generated->getInterval(interval);
    if(rc != eNoError)
    {
        ast_printf("failed to get interval\n");
        return rc;
    }
    ast_printf("interval: %s\n", interval.toString().c_str());
    auto eci = aFrameECI();
    for(auto& tp: interval.discrete(60))
    {
        ast_printf("tp: %s\n", tp.toString().c_str());
        Vector3d posEph, velEph;
        Vector3d posSpc, velSpc;
        rc = ephem_spec_generated->getPosVelIn(eci, tp, posSpc, velSpc);
        if(rc != eNoError)
        {
            ast_printf("failed to get pos vel in eci\n");
            return rc;
        }
        rc = ephem_simple_generated->getPosVelIn(eci, tp, posEph, velEph);
        if(rc != eNoError)
        {
            ast_printf("failed to get pos vel in eci\n");
            return rc;
        }
        ast_printf("posEph: %s, velEph: %s\n", posEph.toString().c_str(), velEph.toString().c_str());
        ast_printf("posSpc: %s, velSpc: %s\n", posSpc.toString().c_str(), velSpc.toString().c_str());
    }

    return 0;
}

class RunningStatus
{
public:
    template<typename ...Args>
    void success(Args&& ...args)
    {
        cprintf(colors::green, args...);
        printf("\n");
    }
    template<typename ...Args>
    void failed(Args&& ...args)
    {
        cprintf(colors::red, args...);
        printf("\n");
    }
    template<typename ...Args>
    void info(Args&& ...args)
    {
        ast_printf(args...);
        printf("\n");
    }
    void keyValue(const char* key, const char* value)
    {
        printf("%s: ", key);
        cprintf(colors::green, "%s\n", value);
    }
    void check(errc_t rc)
    {
        if(rc)
            failed("failed");
        else
            success("success");
    }
#ifdef assert
#   undef assert
#endif
    void assert(errc_t rc)
    {
        if(rc) throw std::runtime_error("test failed");
        check(rc);
    }
};



errc_t aTestFromSTKFile_ThrowException(StringView filepath)
{
    RunningStatus status;

    status.info("loading file '%s' ... ", filepath.data());
    Mover mover;
    errc_t rc = aLoadMover(filepath, mover);
    status.assert(rc);

    status.keyValue("mover name", mover.getName().c_str());

    status.info("testing ephemeris...");

    auto ephem_readed = mover.getEphemeris();
    if(ephem_readed == nullptr)
    {
        status.failed("ephemeris is null");
        return aTestWithEmptyEphemeris(mover);
    }

    auto motion = mover.getMotionProfile();
    if(motion == nullptr)
    {
        status.failed("motion profile is null");
        return eErrorParse;
    }
   
    status.keyValue("motion type", motion->typeName().c_str());
    
    TimeInterval interval{};
    rc = ephem_readed->getInterval(interval);
    if(rc != eNoError)
    {
        status.failed("failed to get interval");
        return rc;
    }
    status.keyValue("interval", interval.toString().c_str());
    
    status.info("generating simple ephemeris...");
    SharedPtr<Ephemeris> ephem_simple_generated;
    ScopedPtr<Ephemeris> temp;
    rc = motion->makeEphemerisSimple(temp);
    ephem_simple_generated = temp.release();
    status.assert(rc);

    SharedPtr<Ephemeris> ephem_spec_generated;
    if(motion->toHPOP())
    {
        ephem_spec_generated = ephem_simple_generated;
    }
    else
    {
        status.info("generating specific ephemeris...");
        ScopedPtr<Ephemeris> temp;
        rc = motion->makeEphemerisSpec(temp);
        ephem_spec_generated = temp.release();
        status.assert(rc);
    }


    std::vector<TimePoint> timePointsToCheck;
    {
        const TimePoint stopTime = interval.getStop();
        const TimePoint startTime = interval.getStart();

        timePointsToCheck = {startTime, stopTime};
    }
    double reltol_pos = 1e-13;
    double reltol_vel = 1e-13;
    double abstol_pos = 1e-10;
    double abstol_vel = 1e-10;

    if(motion->toHPOP())
    {
        reltol_pos = 1e-10;
        reltol_vel = 1e-10;
        abstol_pos = 1e-3;
        abstol_vel = 1e-5;
    }
    auto ephem_frame = ephem_readed->getFrame();
    for(auto& tp: timePointsToCheck){
        status.keyValue("checking for timepoint", tp.toString().c_str());
        Vector3d pos, vel;
        Vector3d posExpected, velExpected;
        rc = ephem_spec_generated->getPosVelIn(ephem_frame, tp, pos, vel);
        if(rc != eNoError)
        {
            status.failed("failed to get pos vel from specific generated ephemeris");
            return rc;
        }
        rc = ephem_readed->getPosVel(tp, posExpected, velExpected);
        if(rc != eNoError)
        {
            status.failed("failed to get pos vel from readed ephemeris");
            return rc;
        }
        status.keyValue("ephemeris type", ephem_readed->typeName().c_str());
        status.info("pos        : %.15g, %.15g, %.15g", pos[0], pos[1], pos[2]);
        status.info("posExpected: %.15g, %.15g, %.15g", posExpected[0], posExpected[1], posExpected[2]);
        status.info("vel        : %.15g, %.15g, %.15g", vel[0], vel[1], vel[2]);
        status.info("velExpected: %.15g, %.15g, %.15g", velExpected[0], velExpected[1], velExpected[2]);
        for(int i = 0; i < 3; ++i)
        {
            if(!aIsClose(pos[i], posExpected[i], reltol_pos, abstol_pos))
            {
                double diff = pos[i] - posExpected[i];
                status.failed("pos[%d] - posExpected[%d]: %19.15g, at timepoint %s", i, i, diff, tp.toString().c_str());
            }
            if(!aIsClose(vel[i], velExpected[i], reltol_vel, abstol_vel))
            {
                double diff = vel[i] - velExpected[i];
                status.failed("vel[%d] - velExpected[%d]: %19.15g, at timepoint %s", i, i, diff, tp.toString().c_str());
            }
        }


        rc = ephem_simple_generated->getPosVelIn(ephem_frame, tp, pos, vel);
        if(rc != eNoError)
        {
            status.failed("failed to get pos vel from simple ephemeris");
            return rc;
        }
        status.keyValue("ephemeris type", ephem_simple_generated->typeName().c_str());
        status.info("pos        : %.15g, %.15g, %.15g", pos[0], pos[1], pos[2]);
        status.info("posExpected: %.15g, %.15g, %.15g", posExpected[0], posExpected[1], posExpected[2]);
        status.info("vel        : %.15g, %.15g, %.15g", vel[0], vel[1], vel[2]);
        status.info("velExpected: %.15g, %.15g, %.15g", velExpected[0], velExpected[1], velExpected[2]);
        for(int i = 0; i < 3; ++i)
        {
            if(!aIsClose(pos[i], posExpected[i], reltol_pos, abstol_pos))
            {
                double diff = pos[i] - posExpected[i];
                status.failed("pos[%d] - posExpected[%d]: %19.15g, at timepoint %s", i, i, diff, tp.toString().c_str());
            }
            if(!aIsClose(vel[i], velExpected[i], reltol_vel, abstol_vel))
            {
                double diff = vel[i] - velExpected[i];
                status.failed("vel[%d] - velExpected[%d]: %19.15g, at timepoint %s", i, i, diff, tp.toString().c_str());
            }
        }
    }

    status.success("end of test");
    return 0;
}

errc_t aTestFromSTKFile(StringView filepath)
{
    try{
        return aTestFromSTKFile_ThrowException(filepath);
    }catch(const std::exception& e){
        ast_printf("test failed: %s\n", e.what());
        return -1;
    }catch(...){
        return -1;
    }
}



AST_NAMESPACE_END
