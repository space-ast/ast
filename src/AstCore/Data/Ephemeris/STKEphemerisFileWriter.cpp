///
/// @file      STKEphemerisFileWriter.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-05
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "STKEphemerisFileWriter.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/DateTime.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/EphemerisLagrangeVar.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/Span.hpp"
#include "AstMath/Vector.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

namespace{

/// @brief 写入位置速度序列
/// @param file 文件指针
/// @param times 时间序列
/// @param positions 位置序列
/// @param velocities 速度序列
/// @return 错误码
errc_t writePosVel(
    std::FILE& file,
    const Span<const double> times,
    const Span<const Vector3d> positions,
    const Span<const Vector3d> velocities
)
{
    size_t size = times.size();
    if(size != positions.size() || size != velocities.size())
    {
        aWarning("times, positions, and velocities must have the same size");
        return eErrorInvalidParam;
    }
    for(size_t i = 0; i < size; ++i)
    {
        const double time = times[i];
        const Vector3d& pos = positions[i];
        const Vector3d& vel = velocities[i];
        std::fprintf(&file, "% .16e % .16e % .16e % .16e % .16e % .16e % .16e\n", 
            time, pos.x(), pos.y(), pos.z(), vel.x(), vel.y(), vel.z());
    }
    return eNoError;
}

using ECoordinate = STKEphemerisFileWriter::ECoordinate;


ECoordinate getCoordinateType(Body& body, Axes* axes)
{
    if(axes == aAxesICRF())
        return ECoordinate::eICRF;
    else if(axes == aAxesJ2000())
        return ECoordinate::eJ2000;
    else if(axes == body.getAxesInertial())
        return ECoordinate::eInertial;
    else if(axes == body.getAxesFixed())
        return ECoordinate::eFixed;
    else
        return ECoordinate::eInertial;
}

std::string getCoordinateStr(ECoordinate coordinate)
{
    switch (coordinate)
    {
    case ECoordinate::eICRF:
        return "ICRF";
    case ECoordinate::eJ2000:
        return "J2000";
    case ECoordinate::eInertial:
        return "Inertial";
    case ECoordinate::eFixed:
        return "Fixed";
    default:
        return "Inertial";
    }
}


Frame* getSTKFrame(Body& body, ECoordinate coordinate)
{
    switch (coordinate)
    {
    case ECoordinate::eICRF:
        return body.getFrameICRF();
    case ECoordinate::eJ2000:
        return body.getFrameJ2000();
    case ECoordinate::eInertial:
        return body.getFrameInertial();
    case ECoordinate::eFixed:
        return body.getFrameFixed();
    }
    return body.getFrameInertial();
}

/// @brief 获取STK星历的有效参考系
/// @details 根据所配置的输出天体与坐标系统，得到有效的参考系；
///          如果没有配置，则回退到星历自身参考系对应的天体和轴系。
/// @param frame 原坐标系
/// @param body  天体
/// @param coordinate 坐标系统
Frame* getValidSTKEphemerisFrame(Frame* frame, Body*& body, ECoordinate& coordinate)
{
    // 星历自身参考系的中心天体（无中心天体的坐标系可能返回 null）
    Body* ephBody = frame->getBody();
    // 用于在回退到星历参考系时记录其轴系，以便后续由 eAuto 反推坐标系统
    Axes* coordinateAxes = nullptr;

    if(!body)
    {
        // 用户未指定天体：优先采用星历参考系对应的天体
        if(ephBody)
        {
            body = ephBody;
            // 同时记录星历参考系的轴系，供 eAuto 坐标系统推断使用
            coordinateAxes = frame->getAxes();
        }
        else
        {
            // 星历参考系无中心天体时，回退为地球
            body = aGetEarth();
            assert(body != nullptr);
        }
    }

    // 仅当用户未显式设定坐标系统(eAuto)且确实取到了星历轴系时，才由轴系类型反推坐标系统；
    // 否则保持用户设定的 coordinate（或维持 eAuto 交由 getSTKFrame 走默认分支）
    if(coordinate == ECoordinate::eAuto && coordinateAxes)
    {
        coordinate = getCoordinateType(*body, coordinateAxes);
    }

    // 由 (body, coordinate) 解析并返回实际采样 Frame。
    // 该 Frame 与读取器按 (CentralBody, CoordinateSystem) 重建的 Frame 一致，从而保证写读往返正确。
    return getSTKFrame(*body, coordinate);
}
}

errc_t STKEphemerisFileWriter::write(StringView filename, const Ephemeris &ephemeris) const
{
    TimeInterval ephInterval{}; // 星历时段
    errc_t rc = ephemeris.getInterval(ephInterval); AST_CHECK_ERRCODE(rc, "failed to get ephemeris interval");

    Body* body = this->body_;
    ECoordinate coordinate = this->coordinate_;
    Frame* ephFrame = ephemeris.getFrame(); AST_CHECK_NULLPTR(ephFrame);
    Frame* frame = getValidSTKEphemerisFrame(ephFrame, body, coordinate);

    std::string coord = getCoordinateStr(coordinate);

    // —— 采样星历 ——
    TimePoint epoch;
    std::vector<double> times;
    std::vector<Vector3d> positions;
    std::vector<Vector3d> velocities;

    Span<const double> timesSpan;
    Span<const Vector3d> positionsSpan;
    Span<const Vector3d> velocitiesSpan;

    const EphemerisLagrangeVar* lagrangeVar = aobject_cast<const EphemerisLagrangeVar*>(&ephemeris);
    if(lagrangeVar && interval_.contains(ephInterval) && step_ <=0 && frame->equals(*ephFrame))
    {
        /*
        可以直接输出原始星历的条件：
        1. 配置区间包含星历自身时段
        2. 步长为 0 或负
        3. 采样参考系与星历自身参考系一致
        */
        epoch = lagrangeVar->getEpoch();
        timesSpan = Span<const double>(lagrangeVar->getTimes());
        positionsSpan = Span<const Vector3d>(lagrangeVar->getPositions());
        velocitiesSpan = Span<const Vector3d>(lagrangeVar->getVelocities());
    }
    else
    {
        // —— 写入时段：取配置区间与星历自身区间的交集 ——
        TimeInterval interval = interval_.intersected(ephInterval);
        if(interval.isEmpty() || interval.isInf())
        {
            aWarning("interval is empty or infinite");
            return eErrorInvalidParam;
        }
        double step = step_ <= 0.0 ? 60.0 : step_;
        TimeList samples;
        interval.discretize(step, samples);
        positions.reserve(samples.size());
        velocities.reserve(samples.size());
        for(auto tp: samples)
        {
            Vector3d pos, vel;
            errc_t rc = ephemeris.getPosVelIn(frame, tp, pos, vel);
            if(rc != eNoError)
            {
                aError("failed to get pos vel at time '%s'", tp.toString().c_str());
                return rc;
            }
            positions.push_back(pos);
            velocities.push_back(vel);
        }

        epoch = samples.epoch();
        times = std::move(samples.seconds());
        timesSpan = Span<const double>(times);
        positionsSpan = Span<const Vector3d>(positions);
        velocitiesSpan = Span<const Vector3d>(velocities);
    }


    // —— 打开输出文件 ——
    std::string filenameStr(filename);
    ScopedPtr<std::FILE> file = ast_fopen(filenameStr.c_str(), "w");
    if(file == nullptr)
    {
        aError("failed to open file '%.*s'", (int)filename.size(), filename.data());
        return eErrorInvalidFile;
    }



    // 以 STK 风格格式化场景历元：22 Mar 2026 04:00:00.000000
    DateTime utc;
    aTimePointToUTC(epoch, utc);
    std::string epochStr;
    aDateTimeFormatGregorianEn(utc, epochStr, 6);

    // —— 写入头部 ——
    std::fprintf(file,
        "stk.v.11.0\n"
        "# WrittenBy   SpaceAST\n"
        "BEGIN Ephemeris\n"
        "    NumberOfEphemerisPoints\t\t%llu\n"
        "    ScenarioEpoch\t\t %s\n"
        "    InterpolationMethod\t\t Lagrange\n"
        "    InterpolationSamplesM1\t\t 5\n"
        "    CentralBody\t\t %s\n"
        "    CoordinateSystem\t\t %s\n"
        "    DistanceUnit\t\t Meters\n"
        "    EphemerisTimePosVel\t\t\n",
        (unsigned long long)timesSpan.size(),
        epochStr.c_str(),
        body->getName().c_str(),
        coord.c_str());

    // —— 写入数据行 ——
    writePosVel(*file, timesSpan, positionsSpan, velocitiesSpan);

    std::fprintf(file, "END Ephemeris\n");
    if(std::ferror(file))
    {
        return eErrorInvalidFile;
    }
    return eNoError;
}


errc_t aSaveSTKEphemeris(StringView filepath, Ephemeris &ephemeris, const TimeInterval &interval, double step)
{
    STKEphemerisFileWriter writer;
    writer.setInterval(interval);
    writer.setStep(step);
    return writer.write(filepath, ephemeris);
}

AST_NAMESPACE_END
