///
/// @file      UiCommandSummary.cpp
/// @brief     任务命令概要显示控件实现 — Section 管道架构
/// @author    axel
/// @date      2026-06-11
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

#include "UiCommandSummary.hpp"
#include "AstCore/Segment.hpp"
#include "AstCore/Command.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/State.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/JulianDate.hpp"
#include "AstCore/DateTime.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/TimeScale.hpp"
#include "AstCore/TimeSystem.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Constants.hpp"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QFont>
#include <QDateTime>
#include <cmath>
#include <algorithm>

AST_NAMESPACE_BEGIN

using namespace units;

namespace {

/// 将 DateTime 格式化为 QString
QString dateTimeToString(const DateTime& dttm, int precision = 8)
{
    std::string str;
    aDateTimeFormatGregorian(dttm, str, precision);
    return QString::fromStdString(str);
}

/// 将 JulianDate 格式化为 12 位小数精度的字符串
QString jdToString(const JulianDate& jd)
{
    return QString::number(jd.impreciseDay(), 'f', 12);
}

} // namespace

// ============================================================================
// 构造
// ============================================================================

UiCommandSummary::UiCommandSummary(Object* object, QWidget* parent)
    : UiCommandSummary(parent)
{
    setCommand(aobject_cast<Command*>(object));
}

UiCommandSummary::UiCommandSummary(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiCommandSummary::~UiCommandSummary() = default;

void UiCommandSummary::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    textEdit_ = new QTextEdit(this);
    textEdit_->setReadOnly(true);
    textEdit_->setFont(QFont("Courier New", 10));
    textEdit_->setLineWrapMode(QTextEdit::NoWrap);

    layout->addWidget(textEdit_);
}

// ============================================================================
// 公共接口
// ============================================================================

void UiCommandSummary::setCommand(Command* command)
{
    if (!command)
        return;
    setObject(command);
    refresh();
}

Command* UiCommandSummary::getCommand() const
{
    return aobject_cast<Command*>(getObject());
}

void UiCommandSummary::refresh()
{
    textEdit_->clear();
    auto* cmd = getCommand();
    if (!cmd)
    {
        textEdit_->setPlainText(tr("(无命令)"));
        return;
    }

    textEdit_->setPlainText(generateSummary());
}

// ============================================================================
// 段上下文
// ============================================================================

SegmentContext UiCommandSummary::buildContext() const
{
    SegmentContext ctx;

    auto* cmd = getCommand();
    ctx.seg = aobject_cast<Segment*>(cmd);
    if (!ctx.seg)
        return ctx;

    ctx.outputState = ctx.seg->getOutputState();
    if (!ctx.outputState)
        return ctx;

    ctx.orbitState = ctx.outputState->getOrbitState();
    if (!ctx.orbitState)
        return ctx;

    ctx.frame = ctx.orbitState->getFrame();
    ctx.stateEpoch = ctx.orbitState->getStateEpoch_TimePoint();

    if (ctx.orbitState->getState(ctx.cart) != eNoError)
        return ctx; // cart 无效，但上下文仍然部分可用

    double gm = ctx.frame ? ctx.frame->getGM() : kEarthGrav;
    ctx.moe = aCartToModOrbElem(ctx.cart.pos(), ctx.cart.vel(), gm);

    return ctx;
}

// ============================================================================
// 管道入口
// ============================================================================

QString UiCommandSummary::generateSummary() const
{
    auto* cmd = getCommand();
    if (!cmd)
        return {};

    auto ctx = buildContext();

    QString text;

    // ---- 头部：时间戳 + 段类型/名称/描述 ----
    text += QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n";
    text += QString("Segment Type: %1\n").arg(QString::fromStdString(cmd->typeName()));
    text += QString("Segment Name: %1\n").arg(QString::fromStdString(cmd->getName()));

    if (ctx.seg)
    {
        text += QString("Segment Description: %1\n")
            .arg(QString::fromStdString(ctx.seg->getRepresentation()));
    }

    // ---- 段状态头部 ----
    if (!ctx.valid())
    {
        if (!ctx.seg)
            text += "\n" + tr("(非段命令，无轨道状态信息)") + "\n";
        else if (!ctx.outputState)
            text += "\n" + tr("(无输出状态)") + "\n";
        else
            text += "\n" + tr("(无轨道状态)") + "\n";
        return text;
    }

    text += "\n";
    text += "-------------------------------------\n";
    text += tr("Satellite State at End of Segment :") + "\n";
    text += "-------------------------------------\n";

    // 参考系
    QString frameName = ctx.frame
        ? QString::fromStdString(ctx.frame->getRepresentation())
        : tr("(未知坐标系)");
    text += QString("reference system: %1\n").arg(frameName);

    // 时间偏置行
    auto* inputState = ctx.seg->getInputState();
    if (inputState && inputState->getOrbitState())
    {
        TimePoint inputEpoch = inputState->getOrbitState()->getStateEpoch_TimePoint();
        double dt = ctx.stateEpoch - inputEpoch;

        JulianDate jdUTC;
        aTimePointToUTC(ctx.stateEpoch, jdUTC);
        DateTime dttmUTC;
        aJDToDateTime(jdUTC, dttmUTC);

        JulianDate jdTT;
        aTimePointToTT(ctx.stateEpoch, jdTT);
        DateTime dttmTT;
        aJDToDateTime(jdTT, dttmTT);

        text += QString("Time pass epoch: %1 sec (Epoch UTC: %2, Epoch TT: %3)\n")
            .arg(dt, 0, 'f', 8)
            .arg(dateTimeToString(dttmUTC))
            .arg(dateTimeToString(dttmTT));
    }
    else
    {
        JulianDate jdUTC;
        aTimePointToUTC(ctx.stateEpoch, jdUTC);
        DateTime dttmUTC;
        aJDToDateTime(jdUTC, dttmUTC);

        text += QString("Epoch UTC: %1\n").arg(dateTimeToString(dttmUTC));
    }

    // ---- 区块管道 ----
    auto groups = buildSectionGroups();
    for (auto& group : groups)
    {
        // 过滤不适用的区块
        std::vector<const SummarySection*> applicable;
        for (auto& sec : group)
            if (sec.applicable && sec.applicable(ctx))
                applicable.push_back(&sec);

        if (applicable.empty())
            continue;

        if (applicable.size() >= 2)
            text += renderSideBySide(*applicable[0], *applicable[1], ctx);
        else
            text += renderSingle(*applicable[0], ctx);
    }

    return text;
}

// ============================================================================
// 区块清单（子类可重写）
// ============================================================================

std::vector<SectionGroup> UiCommandSummary::buildSectionGroups() const
{
    return {
        // 时间系统表 — 独占行
        { { QStringLiteral("TimeSystems"),
            [](const SegmentContext& c) { return c.valid(); },
            &UiCommandSummary::formatTimeSystems } },

        // 直角坐标 + 球坐标 — 横向并排
        { { QStringLiteral("Cartesian"),
            [](const SegmentContext& c) { return c.valid(); },
            &UiCommandSummary::formatCartesian },
          { QStringLiteral("Spherical"),
            [](const SegmentContext& c) { return c.valid(); },
            &UiCommandSummary::formatSpherical } },

        // LLR — 独占行
        { { QStringLiteral("LLR"),
            [](const SegmentContext& c) { return c.valid(); },
            &UiCommandSummary::formatLLR } },

        // 开普勒根数 — 独占行
        { { QStringLiteral("Keplerian"),
            [](const SegmentContext& c) { return c.valid(); },
            &UiCommandSummary::formatKeplerian } },
    };
}

// ============================================================================
// 渲染器
// ============================================================================

QString UiCommandSummary::renderSingle(const SummarySection& sec, const SegmentContext& ctx)
{
    return sec.generate(ctx);
}

QString UiCommandSummary::renderSideBySide(const SummarySection& left,
                                            const SummarySection& right,
                                            const SegmentContext& ctx,
                                            int colWidth)
{
    QString leftText  = left.generate(ctx);
    QString rightText = right.generate(ctx);

    auto leftLines  = leftText.split('\n');
    auto rightLines = rightText.split('\n');
    int maxLines = std::max(leftLines.size(), rightLines.size());

    QString result;
    for (int i = 0; i < maxLines; ++i)
    {
        QString l = (i < leftLines.size())  ? leftLines[i]  : QString();
        QString r = (i < rightLines.size()) ? rightLines[i] : QString();
        result += l.leftJustified(colWidth) + "  " + r + "\n";
    }
    return result;
}

// ============================================================================
// 区块：时间系统
// ============================================================================

QString UiCommandSummary::formatTimeSystems(const SegmentContext& ctx)
{
    JulianDate jdUTC, jdTAI, jdTT, jdTDB;
    aTimePointToUTC(ctx.stateEpoch, jdUTC);
    aTimePointToTAI(ctx.stateEpoch, jdTAI);
    aTimePointToTT(ctx.stateEpoch, jdTT);
    aTimePointToTDB(ctx.stateEpoch, jdTDB);

    DateTime dtUTC, dtTAI, dtTT, dtTDB;
    aJDToDateTime(jdUTC, dtUTC);
    aJDToDateTime(jdTAI, dtTAI);
    aJDToDateTime(jdTT, dtTT);
    aJDToDateTime(jdTDB, dtTDB);

    QString text;
    text += "\n";
    text += "StateEpoch               Gregorian                               Julian Date\n";
    text += "--------------------------------------------------------------------------------\n";
    text += QString("UTC:          %1           %2\n")
        .arg(dateTimeToString(dtUTC), -39)
        .arg(jdToString(jdUTC));
    text += QString("TAI:          %1           %2\n")
        .arg(dateTimeToString(dtTAI), -39)
        .arg(jdToString(jdTAI));
    text += QString("TT :          %1           %2\n")
        .arg(dateTimeToString(dtTT), -39)
        .arg(jdToString(jdTT));
    text += QString("TDB:          %1           %2\n")
        .arg(dateTimeToString(dtTDB), -39)
        .arg(jdToString(jdTDB));

    return text;
}

// ============================================================================
// 区块：直角坐标
// ============================================================================

QString UiCommandSummary::formatCartesian(const SegmentContext& ctx)
{
    QString frameName = ctx.frame
        ? QString::fromStdString(ctx.frame->getRepresentation())
        : tr("(未知)");

    double x  = ctx.cart.x();
    double y  = ctx.cart.y();
    double z  = ctx.cart.z();
    double vx = ctx.cart.vx();
    double vy = ctx.cart.vy();
    double vz = ctx.cart.vz();

    QString text;
    text += QString("Cartesian State (%1)\n").arg(frameName);
    text += "---------------------------------------\n";
    text += QString("X  = %1  m\n").arg(x,  20, 'f', 12);
    text += QString("Y  = %1  m\n").arg(y,  20, 'f', 12);
    text += QString("Z  = %1  m\n").arg(z,  20, 'f', 12);
    text += QString("VX = %1  m/s\n").arg(vx, 20, 'f', 12);
    text += QString("VY = %1  m/s\n").arg(vy, 20, 'f', 12);
    text += QString("VZ = %1  m/s\n").arg(vz, 20, 'f', 12);

    return text;
}

// ============================================================================
// 区块：球坐标
// ============================================================================

QString UiCommandSummary::formatSpherical(const SegmentContext& ctx)
{
    QString frameName = ctx.frame
        ? QString::fromStdString(ctx.frame->getRepresentation())
        : tr("(未知)");

    double r   = ctx.cart.pos().norm();
    double v   = ctx.cart.vel().norm();

    double ra  = atan2(ctx.cart.y(), ctx.cart.x()) * kRadToDeg;
    if (ra < 0) ra += 360.0;

    double dec = asin(ctx.cart.z() / r) * kRadToDeg;

    // FPA
    double rDotV = ctx.cart.pos().dot(ctx.cart.vel());
    double fpa = asin(rDotV / (r * v)) * kRadToDeg;

    // Azimuth
    Vector3d rUnit = ctx.cart.pos() / r;
    Vector3d zAxis{0.0, 0.0, 1.0};
    Vector3d east = zAxis.cross(rUnit);
    double eastNorm = east.norm();
    if (eastNorm > 1e-15)
        east = east / eastNorm;
    else
        east = Vector3d{1.0, 0.0, 0.0};
    Vector3d north = rUnit.cross(east);

    double vEast  = ctx.cart.vel().dot(east);
    double vNorth = ctx.cart.vel().dot(north);
    double azimuth = atan2(vEast, vNorth) * kRadToDeg;
    if (azimuth < 0) azimuth += 360.0;

    QString text;
    text += QString("Spherical State (%1)\n").arg(frameName);
    text += "---------------------------------------\n";
    text += QString("Right Ascension = %1 deg\n").arg(ra,      22, 'f', 0);
    text += QString("Declination     = %1 deg\n").arg(dec,     22, 'f', 0);
    text += QString("Radius          = %1 m\n").arg(r,        20, 'f', 12);
    text += QString("FPA             = %1 deg\n").arg(fpa,     22, 'f', 0);
    text += QString("Azimuth         = %1 deg\n").arg(azimuth, 20, 'f', 12);
    text += QString("Velocity        = %1 m/s\n").arg(v,       20, 'f', 12);

    return text;
}

// ============================================================================
// 区块：LLR
// ============================================================================

QString UiCommandSummary::formatLLR(const SegmentContext& ctx)
{
    QString frameName = ctx.frame
        ? QString::fromStdString(ctx.frame->getRepresentation())
        : tr("(未知)");

    double x = ctx.cart.x();
    double y = ctx.cart.y();
    double z = ctx.cart.z();
    double r = ctx.cart.pos().norm();

    double lat = asin(z / r) * kRadToDeg;
    double lon = atan2(y, x) * kRadToDeg;
    if (lon < 0) lon += 360.0;
    if (lon > 180.0) lon -= 360.0;

    QString text;
    text += "\n";
    text += QString("LLR (%1)\n").arg(frameName);
    text += "------------------------------------------------\n";
    text += QString("Lat = %1 deg\n").arg(lat, 22, 'f', 0);
    text += QString("Lon = %1 deg\n").arg(lon, 22, 'f', 0);
    text += QString("Rad = %1 m\n").arg(r,   20, 'f', 12);

    return text;
}

// ============================================================================
// 区块：开普勒根数
// ============================================================================

QString UiCommandSummary::formatKeplerian(const SegmentContext& ctx)
{
    QString frameName = ctx.frame
        ? QString::fromStdString(ctx.frame->getRepresentation())
        : tr("(未知)");

    double sma     = ctx.moe.getSMA();
    double ecc     = ctx.moe.getEcc();
    double inc     = ctx.moe.getInc() * kRadToDeg;
    double raan    = ctx.moe.getRAAN() * kRadToDeg;
    if (raan < 0) raan += 360.0;
    double argPeri = ctx.moe.getArgPeri() * kRadToDeg;
    if (argPeri < 0) argPeri += 360.0;
    double trueA   = ctx.moe.getTrueAnomaly() * kRadToDeg;
    if (trueA < 0) trueA += 360.0;
    double meanA   = ctx.moe.getMeanAnomaly() * kRadToDeg;
    if (meanA < 0) meanA += 360.0;

    double periRad = ctx.moe.getPeriRad();
    double apoRad  = ctx.moe.getApoRad();

    QString text;
    text += "\n";
    text += QString("Keplerian State (%1)\n").arg(frameName);
    text += "----------------------------------------------------------\n";
    text += QString("SMA         = %1  m\n").arg(sma, 18, 'f', 8);
    text += QString("Ecc         = %1\n").arg(ecc, 18, 'f', 12);
    text += QString("Inc         = %1 deg\n").arg(inc, 18, 'f', 8);
    text += QString("RAAN        = %1 deg\n").arg(raan, 18, 'f', 8);
    text += QString("ArgPeri     = %1 deg\n").arg(argPeri, 18, 'f', 8);
    text += QString("TrueAnomaly = %1 deg\n").arg(trueA, 18, 'f', 8);
    text += QString("MeanAnomaly = %1 deg\n").arg(meanA, 18, 'f', 8);
    text += QString("ApoRadius   = %1  m\n").arg(apoRad, 18, 'f', 8);
    text += QString("PeriRadius  = %1  m\n").arg(periRad, 18, 'f', 8);

    return text;
}

AST_NAMESPACE_END
