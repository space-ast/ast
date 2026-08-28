///
/// @file      SGP4.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-18
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

#include "SGP4.hpp"
#include "SGP4.h"
#include "AstUtil/Constants.h"
#include "AstUtil/Logger.hpp"
#include "AstMath/Vector.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include <algorithm>
#include <cmath>

AST_NAMESPACE_BEGIN

/// @brief 从 TLE 对象初始化 elsetrec（通过 sgp4init）
/// @param tle       输入的 TLE 轨道根数对象
/// @param satrec    输出的 elsetrec，供 sgp4() 预报使用
/// @param opsmode   运行模式：'i'=improved(推荐), 'a'=afspc
/// @param whichconst 重力常数模型：wgs72old / wgs72 / wgs84
/// @return true 成功, false 失败
/// @note  TLE 中的角度字段（inclination_ 等）应为弧度值；
///        meanMotion_ 应为 rad/s（即 rev/day × 2π/86400）
static bool initFromTLE(const TLE& tle, elsetrec& satrec,
                        gravconsttype whichconst = wgs72,
                        char opsmode = 'i'
                        )
{
    memset(&satrec, 0, sizeof(satrec));

    // ---- 1. 卫星编号 ----
    // 从 TLE 第1行提取 5 字符卫星编号
    char satn[6] = "     ";
    if (!tle.lines().line1().empty() && tle.lines().line1().size() >= 7)
    {
        std::copy_n(tle.lines().line1().c_str() + 2, 5, satn);
        satn[5] = '\0';
    }
    // 去除尾部空格
    for (int i = 4; i >= 0; --i)
    {
        if (satn[i] == ' ') satn[i] = '\0';
        else break;
    }

    // ---- 2. 历元转换：TimePoint → JD → epochForSgp4 (JD-2433281.5) ----
    JulianDate jdUTC;
    aTimePointToUTC(tle.epochTime(), jdUTC);
    double epochForSgp4 = (jdUTC.day() - 2433281.5) + jdUTC.dayFractional();

    // ---- 3. 单位转换 ----
    const double xpdotp  = 1440.0 / kTwoPI;   // ≈ 229.183

    double bstar = tle.bstar();
    double ndot  = tle.meanMotionDot() / (xpdotp * 1440.0);
    double nddot = tle.motionDotDot() / (xpdotp * 1440.0 * 1440.0);
    double ecco  = tle.eccentricity();
    double argpo = tle.argOfPerigee();            // 已为 rad
    double inclo = tle.inclination();             // 已为 rad
    double mo    = tle.meanAnomaly();             // 已为 rad
    double nodeo = tle.rightAscenOfNode();        // 已为 rad
    // meanMotion() 返回 rad/s (= rev/day × 2π/86400)，需转为 rad/min
    double no_kozai = tle.meanMotion() * 60.0;

    // ---- 4. 调用 sgp4init ----
    bool ok = SGP4Funcs::sgp4init(
        whichconst, opsmode, satn, epochForSgp4,
        bstar, ndot, nddot,
        ecco, argpo, inclo, mo, no_kozai,
        nodeo, satrec);

    // ---- 5. 设置历元（sgp4init 不填充 jdsatepoch/jdsatepochF，需手动设置） ----
    // 这些字段用于 getPosVel 中计算 tsince（从历元到目标时间的分钟数）
    if (ok)
    {
        satrec.jdsatepoch  = jdUTC.day();
        satrec.jdsatepochF = jdUTC.dayFractional();
    }
    return ok;
}

// @todo 测试SGP4对于闰秒的支持

SGP4::SGP4()
    : storage_()
{
    static_assert(sizeof(elsetrec) <= sizeof(storage_), "storage_ too small");
    new (&storage_) elsetrec{};
}

SGP4::SGP4(const TLE &tle)
    : SGP4()
{
    setTLE(tle);
}

SGP4::SGP4(const TLELines &lines)
    : SGP4()
{
    setTLE(lines);
}


SGP4::~SGP4()
{
    elsetrec& rec = elementSet();
    rec.~elsetrec();
}

Frame *SGP4::getFrame() const
{
    return aFrameTEME();
}

errc_t SGP4::getPos(const TimePoint &tp, Vector3d &pos) const
{
    Vector3d vel;
    return getPosVel(tp, pos, vel);
}

errc_t SGP4::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    elsetrec& satrec = elementSet();

    // 获取目标时间的 UTC 儒略日
    JulianDate jdUTC;
    aTimePointToUTC(tp, jdUTC);

    // tsince = 从 TLE 历元到目标时间的分钟数
    double tsince = ((jdUTC.day() - satrec.jdsatepoch) + (jdUTC.dayFractional() - satrec.jdsatepochF)) * 1440.0;

    bool ok = SGP4Funcs::sgp4(satrec, tsince, pos.data(), vel.data());
    if (!ok)
    {
        return eError;
    }

    // Vallado sgp4 输出：位置 km，速度 km/s；项目内部使用 m 和 m/s
    pos = pos * 1000.0;
    vel = vel * 1000.0;
    return eNoError;
}

errc_t SGP4::getInterval(TimeInterval &interval) const
{
    // SGP4 理论上可在任意时间调用（精度随远离历元而降低）
    // 返回无限区间
    interval.setWhole();
    return eNoError;
}

void SGP4::setTLE(const TLE &tle)
{
    bool flag = initFromTLE(tle, elementSet(), wgs72);
    if (!flag)
    {
        aError("failed to init from TLE");
    }
}

void SGP4::setTLE(const TLELines &lines)
{
    TLE tle = TLE::FromLines(lines);
    setTLE(tle);
}

elsetrec &SGP4::elementSet() const
{
    return reinterpret_cast<struct elsetrec&>(this->storage_);
}

int SGP4::getError() const
{
    return elementSet().error;
}

AST_NAMESPACE_END
