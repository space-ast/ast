///
/// @file      ReportStyleLoader.cpp
/// @brief     报表样式加载器实现
/// @details   使用 BKVParser 解析 ReportStyle 文件（.rst/.rsg），
///            将解析结果填充到 DataReport 结构中。
/// @author    axel
/// @date      2026-07-02
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

#include "ReportStyleLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstReport/DataReport.hpp"
#include "AstUtil/ParseFormat.hpp"

AST_NAMESPACE_BEGIN

// ---- 前向声明内部辅助函数 ----

static errc_t _aLoadClassId(BKVParser& parser, DataReport& report);
static errc_t _aLoadHeader(BKVParser& parser, DataReport& report);
static errc_t _aLoadSection(BKVParser& parser, DataSection& section);
static errc_t _aLoadLine(BKVParser& parser, DataLine& line);
static errc_t _aLoadElement(BKVParser& parser, DataElement& element);
static errc_t _aLoadUnits(BKVParser& parser, DataUnits& units);
static Color  _aParseStyleColor(const ValueView& value);


// ---- 颜色解析 ----

/// @brief 解析颜色值
/// @details 支持两种格式：
///          - #RRGGBB 或 #RRGGBBAA 十六进制字符串（通过 aParseColor）
///          - 0~15 整型调色板索引
static Color _aParseStyleColor(const ValueView& value)
{
    StringView str = value.toStringView();
    if (str.empty())
        return Color(0xFF000000);  // 默认黑色

    // 尝试十六进制颜色字符串
    if (str.size() > 0 && str[0] == '#')
    {
        Color c;
        errc_t err = value.toColor(c);
        if (err == eNoError)
            return c;
    }

    // 尝试整型调色板索引
    int index = value.toInt();

    // 经典 16 色调色板
    static const color_t kPalette[16] = {
        0xFFFFFFFF,  //  0 = White
        0xFFFF0000,  //  1 = Red
        0xFF00FF00,  //  2 = Green
        0xFF0000FF,  //  3 = Blue
        0xFF00FFFF,  //  4 = Cyan
        0xFFFFFF00,  //  5 = Yellow
        0xFFFF00FF,  //  6 = Magenta
        0xFF000000,  //  7 = Black
        0xFF808080,  //  8 = Gray
        0xFFFF8080,  //  9 = Light Red
        0xFF80FF80,  // 10 = Light Green
        0xFF8080FF,  // 11 = Light Blue
        0xFF80FFFF,  // 12 = Light Cyan
        0xFFFFFF80,  // 13 = Light Yellow
        0xFFFF80FF,  // 14 = Light Magenta
        0xFF404040,  // 15 = Dark Gray
    };

    if (index >= 0 && index < 16)
        return Color(kPalette[index]);

    // 高位值可能是原始 0xRRGGBB（旧格式）
    if (index >= 0x100000 && static_cast<unsigned>(index) <= 0xFFFFFF)
        return Color((static_cast<uint32_t>(index) << 8) | 0xFF);

    aWarning("unknown color value '%.*s', using black",
             static_cast<int>(str.size()), str.data());
    return Color(0xFF000000);
}


// ---- 枚举解析 ----

/// @brief 解析 StyleType
/// @details 文件值: 0=Tabular, 1=Graph2D, 2=Fixed, 3=Graph3D
static EStyleType _aParseStyleType(int value)
{
    switch (value)
    {
        case 0:  return EStyleType::eTabular;
        case 1:  return EStyleType::eGraph2D;
        case 2:  return EStyleType::eFixed;
        case 3:  return EStyleType::eGraph3D;
        default: return EStyleType::eTabular;
    }
}

/// @brief 解析 DataType
/// @details 文件值: 0=Float, 1=Int, 2=String, 3=DateTime
static EDataType _aParseDataType(int value)
{
    switch (value)
    {
        case 0:  return EDataType::eFloat;
        case 1:  return EDataType::eInt;
        case 2:  return EDataType::eString;
        case 3:  return EDataType::eDateTime;
        default: return EDataType::eFloat;
    }
}

/// @brief 解析 LineStyle
/// @details 文件值: 0=Solid, 1=Dash, 2=Dot, 3=DashDot
static ELineStyle _aParseLineStyle(int value)
{
    switch (value)
    {
        case 0:  return ELineStyle::eSolid;
        case 1:  return ELineStyle::eDash;
        case 2:  return ELineStyle::eDot;
        case 3:  return ELineStyle::eDashDot;
        default: return ELineStyle::eSolid;
    }
}

/// @brief 解析 PointStyle
/// @details 文件值: 0=None, 1=Square, 2=Circle, 3=Diamond, 4=Triangle, 5=Cross
static EPointStyle _aParsePointStyle(int value)
{
    switch (value)
    {
        case 0:  return EPointStyle::eNone;
        case 1:  return EPointStyle::eSquare;
        case 2:  return EPointStyle::eCircle;
        case 3:  return EPointStyle::eDiamond;
        case 4:  return EPointStyle::eTriangle;
        case 5:  return EPointStyle::eCross;
        default: return EPointStyle::eNone;
    }
}

/// @brief 解析 FillPattern
/// @details 文件值: 0=None, 1=Solid
static EFillPattern _aParseFillPattern(int value)
{
    switch (value)
    {
        case 0:  return EFillPattern::eNone;
        case 1:  return EFillPattern::eSolid;
        default: return EFillPattern::eNone;
    }
}

/// @brief 解析 ExpandMethod
/// @details 文件值: 0=None, 1=All, 2=Selected
static EExpandMethod _aParseExpandMethod(int value)
{
    switch (value)
    {
        case 0:  return EExpandMethod::eNone;
        case 1:  return EExpandMethod::eAll;
        case 2:  return EExpandMethod::eSelected;
        default: return EExpandMethod::eNone;
    }
}


// ---- ClassId 块 ----

static errc_t _aLoadClassId(BKVParser& parser, DataReport& report)
{
    BKVItemView item;
    while (true)
    {
        BKVParser::EToken token = parser.getNext(item);
        if (token == BKVParser::eKeyValue)
        {
            if (aEqualsIgnoreCase(item.key(), "Class"))
            {
                report.className_ = item.value().toString();
            }
        }
        else if (token == BKVParser::eBlockEnd)
        {
            if (aEqualsIgnoreCase(item.value(), "ClassId"))
                return eNoError;
        }
        else if (token == BKVParser::eEOF)
        {
            return eNoError;
        }
    }
}


// ---- Header 块 ----

static errc_t _aLoadHeader(BKVParser& parser, DataReport& report)
{
    BKVItemView item;
    while (true)
    {
        BKVParser::EToken token = parser.getNext(item);
        if (token == BKVParser::eKeyValue)
        {
            StringView key = item.key();

            if (aEqualsIgnoreCase(key, "StyleType"))
                report.styleType_ = _aParseStyleType(item.value().toInt());
            else if (aEqualsIgnoreCase(key, "Title"))
                report.title_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "Date"))
                report.showDate_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "Name"))
                report.showName_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "DescShort"))
                report.showDescShort_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "DescLong"))
                report.showDescLong_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "YLog10"))
                report.yLog10_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "Y2Log10"))
                report.y2Log10_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "VerticalGridLines"))
                report.verticalGridLines_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "HorizontalGridLines"))
                report.horizontalGridLines_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "NumAnnotations"))
                report.numAnnotations_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "AnnotationRotation"))
                report.annotationRotation_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "BackgroundColor"))
                report.backgroundColor_ = _aParseStyleColor(item.value());
            else if (aEqualsIgnoreCase(key, "ViewableDuration"))
                report.viewableDuration_ = item.value().toDouble();
            else if (aEqualsIgnoreCase(key, "RealTimeMode"))
                report.realTimeMode_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "ReadOnlyMode"))
                report.readOnlyMode_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "DayLinesStatus"))
                report.dayLinesStatus_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "LegendStatus"))
                report.legendStatus_ = item.value().toInt();
            // NumSections — 忽略，由 Section 块数量推导
            // NumTick, NumGridX, NumGridY — 暂不存储
        }
        else if (token == BKVParser::eBlockBegin)
        {
            if (aEqualsIgnoreCase(item.value(), "PostProcessor"))
            {
                // @todo 实现 PostProcessor 加载逻辑
                _aSkipUnknownBlock(parser, "PostProcessor");
            }
        }
        else if (token == BKVParser::eBlockEnd)
        {
            if (aEqualsIgnoreCase(item.value(), "Header"))
                return eNoError;
        }
        else if (token == BKVParser::eEOF)
        {
            return eNoError;
        }
    }
}


// ---- Section 块 ----

static errc_t _aLoadSection(BKVParser& parser, DataSection& section)
{
    BKVItemView item;
    while (true)
    {
        BKVParser::EToken token = parser.getNext(item);
        if (token == BKVParser::eKeyValue)
        {
            StringView key = item.key();

            if (aEqualsIgnoreCase(key, "Name"))
                section.name_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "ClassName"))
                section.className_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "NameInTitle"))
                section.nameInTitle_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "ExpandMethod"))
                section.expandMethod_ = _aParseExpandMethod(item.value().toInt());
            else if (aEqualsIgnoreCase(key, "PropMask"))
                section.propMask_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "ShowIntervals"))
                section.showIntervals_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "NumIntervals"))
                section.numIntervals_ = item.value().toInt();
            // NumLines — 忽略，由 Line 块数量推导
        }
        else if (token == BKVParser::eBlockBegin)
        {
            if (aEqualsIgnoreCase(item.value(), "Line"))
            {
                section.lines_.emplace_back();
                _aLoadLine(parser, section.lines_.back());
            }
        }
        else if (token == BKVParser::eBlockEnd)
        {
            if (aEqualsIgnoreCase(item.value(), "Section"))
                return eNoError;
        }
        else if (token == BKVParser::eEOF)
        {
            return eNoError;
        }
    }
}


// ---- Line 块 ----

static errc_t _aLoadLine(BKVParser& parser, DataLine& line)
{
    BKVItemView item;
    while (true)
    {
        BKVParser::EToken token = parser.getNext(item);
        if (token == BKVParser::eKeyValue)
        {
            StringView key = item.key();

            if (aEqualsIgnoreCase(key, "Name"))
                line.name_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "Title"))
                line.title_ = item.value().toString();
            // NumElements — 忽略，由 Element 块数量推导
        }
        else if (token == BKVParser::eBlockBegin)
        {
            if (aEqualsIgnoreCase(item.value(), "Element"))
            {
                line.elements_.emplace_back();
                _aLoadElement(parser, line.elements_.back());
            }
        }
        else if (token == BKVParser::eBlockEnd)
        {
            if (aEqualsIgnoreCase(item.value(), "Line"))
                return eNoError;
        }
        else if (token == BKVParser::eEOF)
        {
            return eNoError;
        }
    }
}


// ---- Element 块 ----

static errc_t _aLoadElement(BKVParser& parser, DataElement& element)
{
    BKVItemView item;
    while (true)
    {
        BKVParser::EToken token = parser.getNext(item);
        if (token == BKVParser::eKeyValue)
        {
            StringView key = item.key();

            if (aEqualsIgnoreCase(key, "Name"))
                element.name_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "IsIndepVar"))
                element.isIndepVar_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "IndepVarName"))
                element.indepVarName_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "Title"))
                element.title_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "NameInTitle"))
                element.nameInTitle_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "Service"))
                element.service_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "Type"))
                element.type_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "Element"))
                element.element_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "Format"))
                element.format_ = item.value().toString();
            else if (aEqualsIgnoreCase(key, "SumAllowedMask"))
                element.sumAllowedMask_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "SummaryOnly"))
                element.summaryOnly_ = item.value().toBool();
            else if (aEqualsIgnoreCase(key, "SumRequestMask"))
                element.sumRequestMask_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "DataType"))
                element.dataType_ = _aParseDataType(item.value().toInt());
            else if (aEqualsIgnoreCase(key, "UnitType"))
                element.unitType_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "LineStyle"))
                element.lineStyle_ = _aParseLineStyle(item.value().toInt());
            else if (aEqualsIgnoreCase(key, "LineWidth"))
                element.lineWidth_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "LineColor"))
                element.lineColor_ = _aParseStyleColor(item.value());
            else if (aEqualsIgnoreCase(key, "PointStyle"))
                element.pointStyle_ = _aParsePointStyle(item.value().toInt());
            else if (aEqualsIgnoreCase(key, "PointSize"))
                element.pointSize_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "PointColor"))
                element.pointColor_ = _aParseStyleColor(item.value());
            else if (aEqualsIgnoreCase(key, "FillPattern"))
                element.fillPattern_ = _aParseFillPattern(item.value().toInt());
            else if (aEqualsIgnoreCase(key, "FillColor"))
                element.fillColor_ = _aParseStyleColor(item.value());
            else if (aEqualsIgnoreCase(key, "PropMask"))
                element.propMask_ = item.value().toInt();
            else if (aEqualsIgnoreCase(key, "UseScenUnits"))
                element.useScenUnits_ = item.value().toBool();
        }
        else if (token == BKVParser::eBlockBegin)
        {
            if (aEqualsIgnoreCase(item.value(), "Units"))
            {
                _aLoadUnits(parser, element.units_);
            }
            else
            {
                // 跳过未知块（如 Event）
                std::string blockName = item.value().toString();
                _aSkipUnknownBlock(parser, blockName);
            }
        }
        else if (token == BKVParser::eBlockEnd)
        {
            if (aEqualsIgnoreCase(item.value(), "Element"))
                return eNoError;
        }
        else if (token == BKVParser::eEOF)
        {
            return eNoError;
        }
    }
}


// ---- Units 块 ----

static errc_t _aLoadUnits(BKVParser& parser, DataUnits& units)
{
    BKVItemView item;
    while (true)
    {
        BKVParser::EToken token = parser.getNext(item);
        if (token == BKVParser::eKeyValue)
        {
            StringView key = item.key();
            std::string val  = item.value().toString();

            if (aEqualsIgnoreCase(key, "DistanceUnit"))
                units.distanceUnit_ = val;
            else if (aEqualsIgnoreCase(key, "TimeUnit"))
                units.timeUnit_ = val;
            else if (aEqualsIgnoreCase(key, "DateFormat"))
                units.dateFormat_ = val;
            else if (aEqualsIgnoreCase(key, "AngleUnit"))
                units.angleUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MassUnit"))
                units.massUnit_ = val;
            else if (aEqualsIgnoreCase(key, "PowerUnit"))
                units.powerUnit_ = val;
            else if (aEqualsIgnoreCase(key, "FrequencyUnit"))
                units.frequencyUnit_ = val;
            else if (aEqualsIgnoreCase(key, "SmallDistanceUnit"))
                units.smallDistanceUnit_ = val;
            else if (aEqualsIgnoreCase(key, "LatitudeUnit"))
                units.latitudeUnit_ = val;
            else if (aEqualsIgnoreCase(key, "LongitudeUnit"))
                units.longitudeUnit_ = val;
            else if (aEqualsIgnoreCase(key, "DurationUnit"))
                units.durationUnit_ = val;
            else if (aEqualsIgnoreCase(key, "Temperature"))
                units.temperature_ = val;
            else if (aEqualsIgnoreCase(key, "SmallTimeUnit"))
                units.smallTimeUnit_ = val;
            else if (aEqualsIgnoreCase(key, "RatioUnit"))
                units.ratioUnit_ = val;
            else if (aEqualsIgnoreCase(key, "RcsUnit"))
                units.rcsUnit_ = val;
            else if (aEqualsIgnoreCase(key, "DopplerVelocityUnit"))
                units.dopplerVelocityUnit_ = val;
            else if (aEqualsIgnoreCase(key, "SARTimeResProdUnit"))
                units.sarTimeResProdUnit_ = val;
            else if (aEqualsIgnoreCase(key, "ForceUnit"))
                units.forceUnit_ = val;
            else if (aEqualsIgnoreCase(key, "PressureUnit"))
                units.pressureUnit_ = val;
            else if (aEqualsIgnoreCase(key, "SpecificImpulseUnit"))
                units.specificImpulseUnit_ = val;
            else if (aEqualsIgnoreCase(key, "PRFUnit"))
                units.prfUnit_ = val;
            else if (aEqualsIgnoreCase(key, "BandwidthUnit"))
                units.bandwidthUnit_ = val;
            else if (aEqualsIgnoreCase(key, "SmallVelocityUnit"))
                units.smallVelocityUnit_ = val;
            else if (aEqualsIgnoreCase(key, "DataRateUnit"))
                units.dataRateUnit_ = val;
            else if (aEqualsIgnoreCase(key, "Percent"))
                units.percent_ = val;
            else if (aEqualsIgnoreCase(key, "UnitTemperature"))
                units.unitTemperature_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerDistanceUnit"))
                units.missionModelerDistanceUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerTimeUnit"))
                units.missionModelerTimeUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerAltitudeUnit"))
                units.missionModelerAltitudeUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerFuelQuantityUnit"))
                units.missionModelerFuelQuantityUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerRunwayLengthUnit"))
                units.missionModelerRunwayLengthUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerBearingAngleUnit"))
                units.missionModelerBearingAngleUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerAngleOfAttackUnit"))
                units.missionModelerAngleOfAttackUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerAttitudeAngleUnit"))
                units.missionModelerAttitudeAngleUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MissionModelerGUnit"))
                units.missionModelerGUnit_ = val;
            else if (aEqualsIgnoreCase(key, "SolidAngle"))
                units.solidAngle_ = val;
            else if (aEqualsIgnoreCase(key, "RadiationDoseUnit"))
                units.radiationDoseUnit_ = val;
            else if (aEqualsIgnoreCase(key, "RadiationShieldThicknessUnit"))
                units.radiationShieldThicknessUnit_ = val;
            else if (aEqualsIgnoreCase(key, "MagneticFieldUnit"))
                units.magneticFieldUnit_ = val;
            else if (aEqualsIgnoreCase(key, "PowerFluxDensityUnit"))
                units.powerFluxDensityUnit_ = val;
            else if (aEqualsIgnoreCase(key, "SpectralDensityUnit"))
                units.spectralDensityUnit_ = val;
        }
        else if (token == BKVParser::eBlockEnd)
        {
            if (aEqualsIgnoreCase(item.value(), "Units"))
                return eNoError;
        }
        else if (token == BKVParser::eEOF)
        {
            return eNoError;
        }
    }
}


// ---- 公开接口 ----

/// @brief 从文件路径加载报表样式
errc_t aLoadReportStyle(StringView filepath, DataReport& report)
{
    BKVParser parser(filepath);
    if (!parser.isOpen())
    {
        aError("failed to open report style file '%.*s'",
               static_cast<int>(filepath.size()), filepath.data());
        return eErrorInvalidFile;
    }

    BKVItemView item;
    while (true)
    {
        BKVParser::EToken token = parser.getNext(item);
        if (token == BKVParser::eBlockBegin)
        {
            if (aEqualsIgnoreCase(item.value(), "ReportStyle"))
            {
                return aLoadReportStyle(parser, report);
            }
        }
        else if (token == BKVParser::eEOF || token == BKVParser::eError)
        {
            break;
        }
    }

    return eErrorParse;
}

/// @brief 从已打开的 BKVParser 加载报表样式
errc_t aLoadReportStyle(BKVParser& parser, DataReport& report)
{
    BKVItemView item;
    while (true)
    {
        BKVParser::EToken token = parser.getNext(item);
        if (token == BKVParser::eKeyValue)
        {
            if (aEqualsIgnoreCase(item.key(), "Name"))
            {
                report.name_ = item.value().toString();
            }
            // NumSections — 忽略，由 Section 块数量推导
        }
        else if (token == BKVParser::eBlockBegin)
        {
            if (aEqualsIgnoreCase(item.value(), "ClassId"))
            {
                _aLoadClassId(parser, report);
            }
            else if (aEqualsIgnoreCase(item.value(), "Header"))
            {
                _aLoadHeader(parser, report);
            }
            else if (aEqualsIgnoreCase(item.value(), "Section"))
            {
                report.sections_.emplace_back();
                _aLoadSection(parser, report.sections_.back());
            }
            else if (aEqualsIgnoreCase(item.value(), "LineAnnotations"))
            {
                // 跳过 LineAnnotations 空块（.rsg 文件末尾常见）
                _aSkipUnknownBlock(parser, "LineAnnotations");
            }
        }
        else if (token == BKVParser::eBlockEnd)
        {
            if (aEqualsIgnoreCase(item.value(), "ReportStyle"))
                return eNoError;
        }
        else if (token == BKVParser::eEOF)
        {
            return eNoError;
        }
    }
}

AST_NAMESPACE_END
