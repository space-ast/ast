///
/// @file      MoverLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#include "MoverLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "MotionTwoBodySax.hpp"
#include "MotionJ2AnalyticalSax.hpp"
#include "MotionJ4AnalyticalSax.hpp"
#include "MotionHPOPSax.hpp"
#include "AstCore/STKEphemerisFileParser.hpp"
#include "AstLoader/ValXMLLoader.hpp"
#include "AstLoader/MissionCommandLoader.hpp"
#include "AstScript/Value.hpp"
#include "AstSim/MotionBallistic.hpp"
#include "AstSim/MotionSimpleAscent.hpp"
#include "AstSim/MotionGreatArc.hpp"
#include "AstSim/MotionExternalEphemeris.hpp"
#include "AstSim/MotionSGP4.hpp"
#include "AstSim/MotionMissionCommand.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/StringUtil.hpp"

AST_NAMESPACE_BEGIN




errc_t _aLoadTwoBody(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    MotionTwoBodySax sax(parser, vehiclePathData);
    errc_t rc = parser.parse(sax);
    A_UNUSED(rc);
    return sax.getMotion(motionProfile);
}

errc_t _aLoadJ2Perturbation(BKVParser& parser, const VehiclePathData& VehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    MotionJ2AnalyticalSax sax(parser, VehiclePathData);
    errc_t rc = parser.parse(sax);
    A_UNUSED(rc);
    return sax.getMotion(motionProfile);
}

errc_t _aLoadJ4Perturbation(BKVParser& parser, const VehiclePathData& VehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    MotionJ4AnalyticalSax sax(parser, VehiclePathData);
    errc_t rc = parser.parse(sax);
    A_UNUSED(rc);
    return sax.getMotion(motionProfile);
}

errc_t _aLoadSGP4(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    struct {
        std::string SSCNumber_;
        std::string intlDesignator_;
        std::string commonName_;
        std::vector<TwoLineElement> elements_;
        SharedPtr<EventInterval> interval_;
        double timeStep_ = 0.0;
        TimePoint startTime_{};
        TimePoint stopTime_{};
    } data{};

    while(1)
    {
        BKVItemView item;
        BKVParser::EToken token;
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "SSCNumber")){
                data.SSCNumber_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "IntlDesignator")){
                data.intlDesignator_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "CommonName")){
                data.commonName_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "EphemSmartInterval"))
            {
                if(_aLoadEventInterval(parser, data.interval_))
                {
                    aError("failed to load ephemeris interval");
                }
            }else if(aEqualsIgnoreCase(item.key(), "StartTime")){
                data.startTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "StopTime")){
                data.stopTime_ = TimePoint::Parse(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "TwoLineElement")){
                TwoLineElement tle{};
                while(1){
                    BKVItemView tleItem;
                    BKVParser::EToken tleToken;
                    tleToken = parser.getNext(tleItem);
                    if(tleToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(tleItem.key(), "Enabled")){
                            tle.enabled_ = tleItem.value().toBool();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "Source")){
                            std::string source = tleItem.value().toString();
                            if(aEqualsIgnoreCase(source, "File")){
                                tle.source_ = ETLESource::eFile;
                            }else if(aEqualsIgnoreCase(source, "Edited")){
                                tle.source_ = ETLESource::eEdited;
                            }else if(aEqualsIgnoreCase(source, "Server")){
                                tle.source_ = ETLESource::eServer;
                            }
                        }else if(aEqualsIgnoreCase(tleItem.key(), "SwitchMethod")){
                            std::string method = tleItem.value().toString();
                            if(aEqualsIgnoreCase(method, "Epoch")){
                                tle.switch_method_ = ESwitchMethod::eEpoch;
                            }
                        }else if(aEqualsIgnoreCase(tleItem.key(), "SwitchEpoch")){
                            tle.switchEpoch_ = TimePoint::Parse(tleItem.value());
                        }else if(aEqualsIgnoreCase(tleItem.key(), "TLE")){
                            // 读取 TLE 两行数据
                            StringView line1 = parser.getLineSkipComment();
                            StringView line2 = parser.getLineSkipComment();
                            tle.tle_.line1_ = std::string(aStripAsciiWhitespace(line1));
                            tle.tle_.line2_ = std::string(aStripAsciiWhitespace(line2));
                        }else if(aEqualsIgnoreCase(tleItem.key(), "EpochTime")){
                            tle.epochTime_ = tleItem.value().toDouble();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "MeanMotionDot")){
                            tle.meanMotionDotTime_ = tleItem.value().toDouble();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "MotionDotDot")){
                            tle.motionDotDot_ = tleItem.value().toDouble();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "IEXP")){
                            tle.iexp_ = tleItem.value().toInt();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "BStar")){
                            tle.bstar_ = tleItem.value().toDouble();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "IBEXP")){
                            tle.ibexp_ = tleItem.value().toInt();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "Inclination")){
                            tle.inclination_ = tleItem.value().toAngleRad();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "RightAscenOfNode")){
                            tle.rightAscenOfNode_ = tleItem.value().toAngleRad();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "Eccentricity")){
                            tle.eccentricity_ = tleItem.value().toDouble();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "ArgOfPerigee")){
                            tle.argOfPerigee_ = tleItem.value().toAngleRad();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "MeanAnomaly")){
                            tle.meanAnomaly_ = tleItem.value().toAngleRad();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "MeanMotion")){
                            tle.meanMotion_ = tleItem.value().toDouble() * 1_revs / 1_day ;
                        }else if(aEqualsIgnoreCase(tleItem.key(), "RevNumber")){
                            tle.revNumber_ = tleItem.value().toInt();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "EphType")){
                            tle.ephType_ = tleItem.value().toInt();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "ElementNumber")){
                            tle.elementNumber_ = tleItem.value().toInt();
                        }else if(aEqualsIgnoreCase(tleItem.key(), "Classification")){
                            tle.classification_ = tleItem.value().toString()[0];
                        }
                    }else if(tleToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(tleItem.value(), "TwoLineElement")){
                            data.elements_.push_back(tle);
                            break;
                        }
                    }
                }
            }else{
                _aSkipUnknownBlock(parser, item.value());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "SGP4")){
                break;
            }
        }else if(token == BKVParser::eError || token == BKVParser::eEOF){
            break;
        }
    };

    auto sgp4 = new MotionSGP4();
    sgp4->SSCNumber_ = data.SSCNumber_;
    sgp4->intlDesignator_ = data.intlDesignator_;
    sgp4->commonName_ = data.commonName_;
    sgp4->elements_ = data.elements_;
    sgp4->setStepSize(data.timeStep_);
    TimeInterval interval(data.startTime_, data.stopTime_);
    auto fallbackInterval = EventIntervalFallback::New(data.interval_, interval);
    sgp4->setInterval(fallbackInterval);

    motionProfile = sgp4;
    return eNoError;
}

errc_t _aLoadHPOP(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    MotionHPOPSax sax(parser, vehiclePathData);
    errc_t rc = parser.parse(sax);
    A_UNUSED(rc);
    return sax.getMotion(motionProfile);
}

errc_t _aLoadSPICE(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    struct {
        std::string filename_;
        int satelliteID_{-1};
        int segmentNumber_{1};
        double stepSize_{60.0};
        bool useLTDelay_{false};
        bool use1stOrderDelay_{false};
        bool use3rdOrderDelay_{false};
        TimePoint startTime_{};
        TimePoint stopTime_{};
        SharedPtr<EventInterval> ephemSmartInterval_;
    } data;


    while(1)
    {
        BKVItemView item;
        BKVParser::EToken token;
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "FileName")){
                data.filename_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "SatelliteID")){
                data.satelliteID_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "SegmentNumber")){
                data.segmentNumber_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "StepSize")){
                data.stepSize_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "UseLTDelay")){
                data.useLTDelay_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "Use1stOrderDelay")){
                data.use1stOrderDelay_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "Use3rdOrderDelay")){
                data.use3rdOrderDelay_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "StartTime")){
                data.startTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "StopTime")){
                data.stopTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "EphemSmartInterval")){
                errc_t rc = _aLoadEventInterval(parser, data.ephemSmartInterval_);
                if(rc)
                    aError("failed to load EphemSmartInterval");
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "EVENTINTERVAL")){
                // 处理事件间隔
                while(1){
                    BKVItemView eventItem;
                    BKVParser::EToken eventToken;
                    eventToken = parser.getNext(eventItem);
                    if(eventToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(eventItem.value(), "EVENTINTERVAL")){
                            break;
                        }
                    }
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "SPICE")){
                break;
            }
        }else if(token == BKVParser::eEOF){
            return eNoError;
        }else{
            return eErrorInvalidFile;
        }
    }

    // 创建 SPICE 运动模型
    auto motionSPICE = MotionSPICE::New();
    
    // 设置 SPICE 文件路径
    motionSPICE->setSpiceFile(data.filename_);
    motionSPICE->setSpiceIndex(data.satelliteID_);
    
    // 设置步长
    motionSPICE->setStepSize(data.stepSize_);
    
    // 设置时间间隔
    auto explicitInterval = EventIntervalExplicit::New(data.startTime_, data.stopTime_);
    auto fallbackInterval = EventIntervalFallback::New(data.ephemSmartInterval_, explicitInterval);
    motionSPICE->setInterval(fallbackInterval);

    motionProfile = motionSPICE;
    
    return eNoError;
}

errc_t _aLoadBallistic(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    struct {
        TimePoint launchTime_;
        TimePoint impactTime_;
        double launchLatitude_{0.0};
        double launchLongitude_{0.0};
        double launchAltitude_{0.0};
        double impactLatitude_{0.0};
        double impactLongitude_{0.0};
        double impactAltitude_{0.0};
        double launchVelocity_{0.0};
        double launchAzimuth_{0.0};
        double launchElevation_{0.0};
        double launchDuration_{0.0};
        int launchType_{0};
        int launchControl_{0};
        double launchApogeeAlt_{0.0};
        TimePoint startTime_{};
        TimePoint stopTime_{};
    } data;

    while(1)
    {
        BKVItemView item;
        BKVParser::EToken token;
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "LaunchTime")){
                data.launchTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "ImpactTime")){
                data.impactTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "LaunchLatitude")){
                data.launchLatitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchLongitude")){
                data.launchLongitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchAltitude")){
                data.launchAltitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "ImpactLatitude")){
                data.impactLatitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "ImpactLongitude")){
                data.impactLongitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "ImpactAltitude")){
                data.impactAltitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchVelocity")){
                data.launchVelocity_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchAzimuth")){
                data.launchAzimuth_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchElevation")){
                data.launchElevation_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchDuration")){
                data.launchDuration_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchType")){
                data.launchType_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchControl")){
                data.launchControl_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchApogeeAlt")){
                data.launchApogeeAlt_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "StartTime")){
                data.startTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "StopTime")){
                data.stopTime_ = TimePoint::Parse(item.value());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Ballistic")){
                break;
            }
        }else if(token == BKVParser::eEOF){
            return eNoError;
        }else{
            return eErrorInvalidFile;
        }
    }

    // 创建弹道运动模型
    auto motionBallistic = MotionBallistic::New();
    
    // 设置弹道参数
    motionBallistic->setLaunchTime(data.launchTime_);
    motionBallistic->setImpactTime(data.impactTime_);
    motionBallistic->setLaunchPosition(data.launchLatitude_, data.launchLongitude_, data.launchAltitude_);
    motionBallistic->setImpactPosition(data.impactLatitude_, data.impactLongitude_, data.impactAltitude_);
    motionBallistic->setLaunchVelocity(data.launchVelocity_);
    motionBallistic->setLaunchAzimuth(data.launchAzimuth_);
    motionBallistic->setLaunchElevation(data.launchElevation_);
    motionBallistic->setLaunchDuration(data.launchDuration_);
    motionBallistic->setLaunchType(data.launchType_);
    motionBallistic->setLaunchControl(data.launchControl_);
    motionBallistic->setLaunchApogeeAlt(data.launchApogeeAlt_);
    
    // 设置步长
    // motionBallistic->setStepSize(1.0); // 默认步长为1秒
    // 
    // // 设置时间间隔
    // auto explicitInterval = EventIntervalExplicit::New(data.startTime_, data.stopTime_);
    // motionBallistic->setInterval(explicitInterval);

    motionProfile = motionBallistic;
    
    return eNoError;
}

errc_t _aLoadSimpleAscent(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    struct {
        TimePoint launchTime_;
        bool useScenTime_{false};
        TimePoint burnoutTime_;
        double launchLatitude_{0.0};
        double launchLongitude_{0.0};
        double launchAltitude_{0.0};
        double burnoutLatitude_{0.0};
        double burnoutLongitude_{0.0};
        double burnoutAltitude_{0.0};
        double burnoutVelocity_{0.0};
        double granularity_{5.0};
        TimePoint startTime_{};
        TimePoint stopTime_{};
    } data;

    while(1)
    {
        BKVItemView item;
        BKVParser::EToken token;
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "LaunchTime")){
                data.launchTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "UseScenTime")){
                data.useScenTime_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "BurnoutTime")){
                data.burnoutTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "LaunchLatitude")){
                data.launchLatitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchLongitude")){
                data.launchLongitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "LaunchAltitude")){
                data.launchAltitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "BurnoutLatitude")){
                data.burnoutLatitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "BurnoutLongitude")){
                data.burnoutLongitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "BurnoutAltitude")){
                data.burnoutAltitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "BurnoutVelocity")){
                data.burnoutVelocity_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "Granularity")){
                data.granularity_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "StartTime")){
                data.startTime_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "StopTime")){
                data.stopTime_ = TimePoint::Parse(item.value());
            }
        }else if(token == BKVParser::eBlockBegin)
        {
            
        }
        else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "SimpleAscent")){
                break;
            }
        }else if(token == BKVParser::eEOF){
            return eNoError;
        }else{
            aError("invalid token");
            return eErrorInvalidFile;
        }
    }

    // 创建简单上升运动模型
    auto motionSimpleAscent = MotionSimpleAscent::New();
    
    // 设置简单上升参数
    motionSimpleAscent->setLaunchTime(data.launchTime_);
    motionSimpleAscent->setUseScenTime(data.useScenTime_);
    motionSimpleAscent->setBurnoutTime(data.burnoutTime_);
    motionSimpleAscent->setLaunchPosition(data.launchLatitude_, data.launchLongitude_, data.launchAltitude_);
    motionSimpleAscent->setBurnoutPosition(data.burnoutLatitude_, data.burnoutLongitude_, data.burnoutAltitude_);
    motionSimpleAscent->setBurnoutVelocity(data.burnoutVelocity_);
    motionSimpleAscent->setGranularity(data.granularity_);
    
    // 设置步长
    // motionSimpleAscent->setStepSize(data.granularity_); // 使用配置的时间粒度作为步长
    // 
    // // 设置时间间隔
    // auto explicitInterval = EventIntervalExplicit::New(data.startTime_, data.stopTime_);
    // motionSimpleAscent->setInterval(explicitInterval);

    motionProfile = motionSimpleAscent;
    
    return eNoError;
}

errc_t _aLoadGreatArc(BKVParser& parser, const VehiclePathData& vehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    struct {
        int versionIndicator_{0};
        std::string method_;
        TimePoint timeOfFirstWaypoint_{};
        bool useScenTime_{false};
        double arcGranularity_{0.0};
        double defaultRate_{0.0};
        double defaultAltitude_{0.0};
        double defaultTurnRadius_{0.0};
        std::string altRef_;
        std::string altInterpMethod_;
        int numberOfWaypoints_{0};
        std::vector<WayPoint> waypoints_;
        SharedPtr<EventInterval> arcSmartInterval_;
    } data;

    while(1)
    {
        BKVItemView item;
        BKVParser::EToken token;
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "VersionIndicator")){
                data.versionIndicator_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "Method")){
                data.method_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "TimeOfFirstWaypoint")){
                data.timeOfFirstWaypoint_ = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "UseScenTime")){
                data.useScenTime_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "ArcGranularity")){
                data.arcGranularity_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "DefaultRate")){
                data.defaultRate_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "DefaultAltitude")){
                data.defaultAltitude_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "DefaultTurnRadius")){
                data.defaultTurnRadius_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "AltRef")){
                data.altRef_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "AltInterpMethod")){
                data.altInterpMethod_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "NumberOfWaypoints")){
                data.numberOfWaypoints_ = item.value().toInt();
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "ArcSmartInterval")){
                errc_t rc = _aLoadEventInterval(parser, data.arcSmartInterval_);
                if(rc)
                    aError("failed to load ArcSmartInterval");
            }else if(aEqualsIgnoreCase(item.value(), "Waypoints")){
                // 解析航点数据
                for(int i = 0; i < data.numberOfWaypoints_; i++)
                {
                    WayPoint waypoint{};
                    StringView line = parser.getLineSkipComment();
                    int status = sscanf(
                        line.data(), "%lf %lf %lf %lf %lf %lf %lf", 
                        &waypoint.time_, &waypoint.position_.latitude(), &waypoint.position_.longitude(), &waypoint.position_.altitude(), 
                        &waypoint.speed_, &waypoint.acceleration_, &waypoint.turnRadius_
                    );
                    waypoint.position_.latitude() *= kDegToRad;
                    waypoint.position_.longitude() *= kDegToRad;
                    
                    if(status != 7){
                        aError("invalid waypoint line");
                        // return eErrorInvalidFile;
                    }else{
                        data.waypoints_.push_back(waypoint);
                    }
                }
            }else{
                _aSkipUnknownBlock(parser, item.value());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "GreatArc")){
                break;
            }
        }else if(token == BKVParser::eEOF){
            return eNoError;
        }else{
            return eErrorInvalidFile;
        }
    }

    // 创建 GreatArc 运动模型
    auto motionGreatArc = MotionGreatArc::New();
    
    // 设置开始时间
    motionGreatArc->setStartTime(data.timeOfFirstWaypoint_);
    
    // 设置航点数据
    motionGreatArc->setWayPoints(data.waypoints_);
    
    // 设置运动模型
    motionProfile = motionGreatArc;
    
    return eNoError;
}

errc_t _aLoadExternExternalEphemeris(BKVParser& parser, const VehiclePathData& VehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    BKVItemView item;
    BKVParser::EToken token;
    struct {
        std::string filename_;
    } data;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Format")){
                // @todo 处理格式
            }else if(aEqualsIgnoreCase(item.key(), "Covariance")){
                // @todo 处理ephemeris
            }else if(aEqualsIgnoreCase(item.key(), "Filename"))
            {
                data.filename_ = item.value().toString();
            }
        }else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "StkExternal")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);

    auto motionExtern = MotionExternalEphemeris::New();
    motionExtern->setFilePath(data.filename_);
    motionProfile = motionExtern;
    
    return eNoError;
}

errc_t _aLoadAstrogator(BKVParser& parser, const VehiclePathData& VehiclePathData, ScopedPtr<MotionProfile>& motionProfile)
{
    SharedPtr<Value> value;
    errc_t rc = aLoadValue(parser.getFile(), value);
    if(rc || !value)
    {
        aError("failed to load astrogator");
    }
    else
    {

        const Value& dictSequence = value->operator[]("MainSEQUENCE");
        if(!dictSequence.isNull())
        {
            auto motionMissionCommand = MotionMissionCommand::New();
            motionProfile = motionMissionCommand;
            rc = aLoadSequence(dictSequence, motionMissionCommand->getSequence());
            if(rc)
                aError("failed to load MainSEQUENCE");
        }
        else
        {
            aError("failed to find MainSEQUENCE");
            return eErrorInvalidParam;
        }
    }
    BKVItemView item;
    auto token = parser.getNext(item);
    if(token == BKVParser::eBlockEnd && aEqualsIgnoreCase(item.value(), "Astrogator"))
    {
        return eNoError;
    }
    else
    {
        aError("expected end of Astrogator block");
        return eErrorInvalidParam;
    }
}

errc_t _aLoadPassDefn(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Break")){
                // @todo 处理分割点
            }else if(aEqualsIgnoreCase(item.key(), "CoordSystem")){
                // @todo 处理坐标系
            }else if(aEqualsIgnoreCase(item.key(), "BreakType")){
                // @todo 处理分割类型
            }else if(aEqualsIgnoreCase(item.key(), "BreakAngle")){
                // @todo 处理分割角度
            }else if(aEqualsIgnoreCase(item.key(), "PartialPassMethod")){
                // @todo 这是什么???
            }else if(aEqualsIgnoreCase(item.key(), "DisplayFlag")){
                // @todo 处理显示标志
            }else if(aEqualsIgnoreCase(item.key(), "FirstPass")){
                // @todo 处理第一次通过
            }else if(aEqualsIgnoreCase(item.key(), "FirstPath")){
                // @todo 处理第一个路径
            }else if(aEqualsIgnoreCase(item.key(), "RepeatCycle")){
                // @todo 处理重复周期
            }else if(aEqualsIgnoreCase(item.key(), "MaintainPassNum")){
                // @todo 处理保持通过次数
            }else if(aEqualsIgnoreCase(item.key(), "RangeFirstPass")){
                // @todo 处理第一次通过
            }else if(aEqualsIgnoreCase(item.key(), "RangeLastPass")){
                // @todo 处理最后一次通过
            }else if(aEqualsIgnoreCase(item.key(), "DisplayScheme")){
                // @todo 处理显示方案
            }else if(aEqualsIgnoreCase(item.key(), "ScenarioEpoch")){
                // @todo 处理场景历元
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "PassDefn")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadVehiclePath(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    VehiclePathData data;
    data.centralBody_ = aGetDefaultBody();  // default body
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "CentralBody")){
                data.centralBody_ = aGetBody(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "StoreEphemeris")){
                data.storeEphemeris_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "SmoothInterp")){
                data.smoothInterp_ = item.value().toBool();
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "PassDefn"))
            {
                if(errc_t rc = _aLoadPassDefn(parser, mover)){
                    return rc;
                }
            }
            else
            {
                ScopedPtr<MotionProfile> motionProfile;
                if(aEqualsIgnoreCase(item.value(), "TwoBody")){
                    if(errc_t rc = _aLoadTwoBody(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "J2Perturbation")){
                    if(errc_t rc = _aLoadJ2Perturbation(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "J4Perturbation")){
                    if(errc_t rc = _aLoadJ4Perturbation(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "SGP4")){
                    if(errc_t rc = _aLoadSGP4(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "HPOP"))
                {
                    if(errc_t rc = _aLoadHPOP(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "SPICE")){
                    if(errc_t rc = _aLoadSPICE(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "Ballistic")){
                    if(errc_t rc = _aLoadBallistic(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "SimpleAscent")){
                    if(errc_t rc = _aLoadSimpleAscent(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "GreatArc")){
                    if(errc_t rc = _aLoadGreatArc(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "StkExternal"))
                {
                    if(errc_t rc = _aLoadExternExternalEphemeris(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else if(aEqualsIgnoreCase(item.value(), "Astrogator")){
                    if(errc_t rc = _aLoadAstrogator(parser, data, motionProfile)){
                        return rc;
                    }
                }
                else
                {
                    _aSkipUnknownBlock(parser, item.value());
                }
                mover.setMotionProfile(motionProfile.release());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "VehiclePath")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


errc_t _aLoadEphemeris(BKVParser& parser, Mover& mover)
{
    errc_t rc = aLoadSTKEphemeris(parser, mover.getEphemerisHandle());
    return rc;
}

errc_t _aLoadMassProperties(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Mass")){
                // @todo 处理质量
            }else if(aEqualsIgnoreCase(item.key(), "InertiaXX")){
                // @todo 处理转动惯量XX
            }else if(aEqualsIgnoreCase(item.key(), "InertiaYX")){
                // @todo 处理转动惯量YX
            }else if(aEqualsIgnoreCase(item.key(), "InertiaYY")){
                // @todo 处理转动惯量YY
            }else if(aEqualsIgnoreCase(item.key(), "InertiaZX")){
                // @todo 处理转动惯量ZX
            }else if(aEqualsIgnoreCase(item.key(), "InertiaZY")){
                // @todo 处理转动惯量ZY
            }else if(aEqualsIgnoreCase(item.key(), "InertiaZZ")){
                // @todo 处理转动惯量ZZ
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "MassProperties")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadAttitude(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "ScenarioEpoch")){
                // @todo 处理场景历元
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Profile")){
                // @todo 处理姿态定义
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Attitude")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadSwath(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "SwathType")){
                // @todo 处理扫描带类型
            }else if(aEqualsIgnoreCase(item.key(), "ElevationAngle")){
                // @todo 处理仰角
            }else if(aEqualsIgnoreCase(item.key(), "HalfAngle")){
                // @todo 处理半角
            }else if(aEqualsIgnoreCase(item.key(), "Distance")){
                // @todo 处理距离
            }else if(aEqualsIgnoreCase(item.key(), "RepType")){
                // @todo 处理表示类型
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Swath")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadEclipse(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Sunlight")){
                // @todo 处理阳光
            }else if(aEqualsIgnoreCase(item.key(), "SunlightLineStyle")){
                // @todo 处理阳光线样式
            }else if(aEqualsIgnoreCase(item.key(), "SunlightLineWidth")){
                // @todo 处理阳光线宽度
            }else if(aEqualsIgnoreCase(item.key(), "SunlightMarkerStyle")){
                // @todo 处理阳光标记样式
            }else if(aEqualsIgnoreCase(item.key(), "Penumbra")){
                // @todo 处理半影
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraLineStyle")){
                // @todo 处理半影线样式
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraLineWidth")){
                // @todo 处理半影线宽度
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraMarkerStyle")){
                // @todo 处理半影标记样式
            }else if(aEqualsIgnoreCase(item.key(), "Umbra")){
                // @todo 处理本影
            }else if(aEqualsIgnoreCase(item.key(), "UmbraLineStyle")){
                // @todo 处理本影线样式
            }else if(aEqualsIgnoreCase(item.key(), "UmbraLineWidth")){
                // @todo 处理本影线宽度
            }else if(aEqualsIgnoreCase(item.key(), "UmbraMarkerStyle")){
                // @todo 处理本影标记样式
            }else if(aEqualsIgnoreCase(item.key(), "SunlightPenumbraLine")){
                // @todo 处理阳光半影线
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraUmbraLine")){
                // @todo 处理半影本影线
            }else if(aEqualsIgnoreCase(item.key(), "SunlightColor")){
                // @todo 处理阳光颜色
            }else if(aEqualsIgnoreCase(item.key(), "PenumbraColor")){
                // @todo 处理半影颜色
            }else if(aEqualsIgnoreCase(item.key(), "UmbraColor")){
                // @todo 处理本影颜色
            }else if(aEqualsIgnoreCase(item.key(), "UseCustomEclipseBodies")){
                // @todo 处理是否使用自定义遮挡天体??
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Eclipse")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadRealTimeDef(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "HistoryDuration")){
                // @todo 处理历史持续时间
            }else if(aEqualsIgnoreCase(item.key(), "LookAheadDuration")){
                // @todo 处理前瞻持续时间
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "RealTimeDef")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadExtensions(BKVParser& parser, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            // @todo 处理各种扩展块
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadMover(BKVParser& parser, StringView moverType, Mover& mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Name")){
                mover.setName(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "VehiclePath")){
                if(errc_t rc = _aLoadVehiclePath(parser, mover)){
                    aError("failed to load vehicle path");
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Ephemeris")){
                if(errc_t rc = _aLoadEphemeris(parser, mover)){
                    aError("failed to load ephemeris");
                    return rc;
                }
            }
            else if(aEqualsIgnoreCase(item.value(), "MassProperties")){
                if(errc_t rc = _aLoadMassProperties(parser, mover)){
                    aError("failed to load mass properties");
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Attitude")){
                if(errc_t rc = _aLoadAttitude(parser, mover)){
                    aError("failed to load attitude");
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Swath")){
                if(errc_t rc = _aLoadSwath(parser, mover)){
                    aError("failed to load swath");
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Eclipse")){
                if(errc_t rc = _aLoadEclipse(parser, mover)){
                    aError("failed to load eclipse");
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "RealTimeDef")){
                if(errc_t rc = _aLoadRealTimeDef(parser, mover)){
                    aError("failed to load real time def");
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Extensions")){
                if(errc_t rc = _aLoadExtensions(parser, mover)){
                    aError("failed to load extensions");
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SubObjects")){
                if(errc_t rc = _aLoadSubObjects(parser, &mover)){
                    aError("failed to load sub objects");
                    return rc;
                }
            }else
            {
                _aSkipUnknownBlock(parser, item.value());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), moverType)){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


errc_t _aLoadSatellite(BKVParser& parser, Mover& mover)
{
    return _aLoadMover(parser, "Satellite", mover);
}


errc_t aLoadMover(StringView filepath, Mover &mover)
{
    BKVItemView item;
    BKVParser::EToken token;
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        aError("failed to open file '%.*s'", (int)filepath.size(), filepath.data());
        return eErrorInvalidFile;
    }
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            // 处理文件开头的版本信息和作者信息
            // @todo 处理版本信息
        }
        else if(token == BKVParser::eBlockBegin){
            // if(aEqualsIgnoreCase(item.value(), "Satellite")){
            //     if(errc_t rc = _aLoadSatellite(parser, mover)){
            //         return rc;
            //     }
            // }

            
            // 第一个BEGIN END块的名称就是Mover的类型
            std::string moverType = item.value().toString();
            return _aLoadMover(parser, moverType, mover);
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


AST_NAMESPACE_END