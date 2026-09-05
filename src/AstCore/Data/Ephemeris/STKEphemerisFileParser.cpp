///
/// @file      STKEphemerisFileParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-23
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

#include "STKEphemerisFileParser.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/EphemerisLagrangeVar.hpp"


AST_NAMESPACE_BEGIN

errc_t aLoadSTKEphemeris(StringView filepath, HEphemeris &ephemeris)
{
    BKVParser parser(filepath);
    return aLoadSTKEphemeris(parser, ephemeris);
}

enum EInterpolationMethod
{
    eUnknown=-1,    ///< 未知插值方法
    eLagrange,      ///< Lagrange插值
    eOnePt,         /// 这是什么??? 1点插值?
};


static errc_t parsePosVel(
    BKVParser &parser,
    int numpoints, 
    std::vector<double>& times, 
    std::vector<Vector3d>& positions, 
    std::vector<Vector3d>& velocities
)
{
    // 开始解析星历数据
    for(int i = 0; i < numpoints; i++)
    {
        StringView line = parser.getLineSkipComment();
        if(line.empty()){
            aError("parse error: ephemerisTimePosVel line %d is empty", i);
            return eErrorParse;
        }
        // 解析一行数据: 时间偏移 位置x 位置y 位置z 速度x 速度y 速度z
        double timeOffset;
        Vector3d pos, vel;
        int status = sscanf(line.data(), 
            "%lf %lf %lf %lf %lf %lf %lf", 
            &timeOffset, 
            &pos.x(), &pos.y(), &pos.z(), 
            &vel.x(), &vel.y(), &vel.z()
        );
        if(status != 7){
            aError("parse error: '%.*s'", (int)line.size(), line.data());
            return eErrorParse;
        }
        times.push_back(timeOffset);
        positions.push_back(pos);
        velocities.push_back(vel);
    }
    return eNoError;
}


errc_t aLoadSTKEphemeris(BKVParser &parser, ScopedPtr<Ephemeris> &ephemeris)
{
    if(!parser.isOpen())
        return eErrorInvalidParam;
    BKVParser::EToken token;
    BKVItemView item;
    struct {
        std::string binaryFileName_{};                           ///< 二进制文件名称
        int numberOfEphemerisPoints_{0};                         ///< 星历点数
        TimePoint scenarioEpoch_{};                              ///< 场景时间点
        EInterpolationMethod interpolationMethod_{eUnknown};     ///< 插值方法
        int interpolationSamplesM1_{5};                          ///< 插值样本数减1(Minus 1)，相当于插值阶数
        double distanceUnitFactor_{1.0};                         ///< 距离单位的因子
        HBody body_{};                                           ///< 天体
        HFrame frame_{};                                         ///< 坐标系
        std::vector<Vector3d> positions_{};                      ///< 位置序列
        std::vector<Vector3d> velocities_{};                     ///< 速度序列
        std::vector<double>   times_{};                          ///< 时间序列
    } data;
    
    // 解析文件头部
    while((token = parser.getNext(item)) != BKVParser::eEOF)
    {
        if(token == BKVParser::eBlockBegin)
        {
            
        }
        else if(token == BKVParser::eKeyValue)
        {
            StringView key = item.key();
            ValueView value = item.value();
            
            if(aEqualsIgnoreCase(key, "BinaryFileName"))
            {
                data.binaryFileName_ = value.toString();
            }
            else if(aEqualsIgnoreCase(key, "NumberOfEphemerisPoints"))
            {
                data.numberOfEphemerisPoints_ = value.toInt();
                if(data.numberOfEphemerisPoints_ <= 0){
                    aError("numberOfEphemerisPoints must be greater than 0");
                    return eErrorParse;
                }
                data.times_.reserve(data.numberOfEphemerisPoints_);
                data.positions_.reserve(data.numberOfEphemerisPoints_);
                data.velocities_.reserve(data.numberOfEphemerisPoints_);
            }
            else if(aEqualsIgnoreCase(key, "ScenarioEpoch"))
            {
                data.scenarioEpoch_ = TimePoint::Parse(value.data());
            }
            else if(aEqualsIgnoreCase(key, "InterpolationMethod"))
            {
                if(aEqualsIgnoreCase(value, "Lagrange"))
                    data.interpolationMethod_ = eLagrange;
                else if(aEqualsIgnoreCase(value, "OnePt"))
                {
                    // 这个的含义还不清楚，1点插值?
                    data.interpolationMethod_ = eOnePt;
                }
                else if(aEqualsIgnoreCase(value, "GreatArc"))
                {
                    // GreatArc 插值方法，暂时使用 Lagrange 插值
                    data.interpolationMethod_ = eLagrange;
                }
                else
                {
                    aError("unsupported interpolation method: '%.*s'", (int)value.size(), value.data());
                    return eErrorParse;
                }
            }
            else if(aEqualsIgnoreCase(key, "InterpolationSamplesM1"))
            {
                data.interpolationSamplesM1_ = value.toInt();
            }
            else if(aEqualsIgnoreCase(key, "InterpolationOrder"))
            {
                data.interpolationSamplesM1_ = value.toInt();
            }
            else if(aEqualsIgnoreCase(key, "CentralBody"))
            {
                // 获取天体对象
                data.body_ = aGetBody(value.data());
                if(data.body_ == nullptr){
                    aError(
                        "body '%.*s' not found, please check the body name and whether you have call `aInitialize` first", 
                        (int)value.size(), value.data()
                    );
                    return eErrorParse;
                }
            }
            else if(aEqualsIgnoreCase(key, "CoordinateSystem"))
            {
                if(data.body_ == nullptr){
                    // 默认使用地球
                    aWarning("body is not specified, use Earth as default");
                    data.body_ = aGetEarth();
                }
                
                // 获取坐标系
                if(aEqualsIgnoreCase(value, "J2000"))
                {
                    data.frame_ = data.body_->getFrameJ2000();
                }
                else if(aEqualsIgnoreCase(value, "ICRF"))
                {
                    data.frame_ = data.body_->getFrameICRF();
                }
                else if(aEqualsIgnoreCase(value, "Fixed"))
                {
                    data.frame_ = data.body_->getFrameFixed();
                }else if(aEqualsIgnoreCase(value, "Inertial"))
                {
                    data.frame_ = data.body_->getFrameInertial();
                }
                else{
                    aError("unsupported coordinate system: '%.*s'", (int)value.size(), value.data());
                    return eErrorParse;
                }
            }
            else if(aEqualsIgnoreCase(key, "DistanceUnit"))
            {
                // 解析距离单位因子
                if(aEqualsIgnoreCase(value, "Meters"))
                    data.distanceUnitFactor_ = 1.0;
                else if(aEqualsIgnoreCase(value, "Kilometers"))
                    data.distanceUnitFactor_ = 1e3;
            }
            else{
                if(!data.binaryFileName_.empty())
                {
                    // 如果是通过二进制文件保存的，则EphemerisTimePosVel等字段下面没有数据，不需要读取
                    // @todo 解析二进制文件，他的格式是？
                }
                else
                {
                    // 如果不是通过二进制文件保存的，则这些字段下面有数据，需要读取
                    if(aEqualsIgnoreCase(key, "EphemerisTimePosVel"))
                    {
                        errc_t rc = parsePosVel(
                            parser, 
                            data.numberOfEphemerisPoints_,
                            data.times_,
                            data.positions_,
                            data.velocities_
                        );
                        if(rc != eNoError){
                            aError("failed to parse ephemerisTimePosVel");
                            return rc;
                        }
                    }
                    else if(aEqualsIgnoreCase(key, "EphemerisEciTimePosVel"))
                    {
                        data.body_ = aGetEarth();
                        data.frame_ = data.body_->makeFrameInertial();
                        errc_t rc = parsePosVel(
                            parser, 
                            data.numberOfEphemerisPoints_,
                            data.times_,
                            data.positions_,
                            data.velocities_
                        );
                        if(rc != eNoError){
                            aError("failed to parse ephemerisEciTimePosVel");
                            return rc;
                        }
                    }
                }
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "Ephemeris"))
            {
                break;
            }
        }
        else if(token == BKVParser::eEOF)
        {
            break;
        }
        else if(token == BKVParser::eError)
        {
            aError("parse error");
            return eErrorParse;
        }
    }
    // 转换距离单位
    if(data.distanceUnitFactor_ != 1.0)
    {
        for(auto& pos: data.positions_)
            pos *= data.distanceUnitFactor_;
        for(auto& vel: data.velocities_)
            vel *= data.distanceUnitFactor_;
    }
    EphemerisLagrangeVar* ephemerisLagrangeVar = new EphemerisLagrangeVar();
    ephemerisLagrangeVar->setEpoch(data.scenarioEpoch_);
    ephemerisLagrangeVar->setTimes(std::move(data.times_));
    ephemerisLagrangeVar->setPositions(std::move(data.positions_));
    ephemerisLagrangeVar->setVelocities(std::move(data.velocities_));
    ephemerisLagrangeVar->setFrame(data.frame_);
    ephemerisLagrangeVar->setInterpolateOrder(data.interpolationSamplesM1_);
    
    ephemeris = ephemerisLagrangeVar;
    
    return eNoError;
}

errc_t aLoadSTKEphemeris(BKVParser &parser, HEphemeris &ephemeris)
{
    ScopedPtr<Ephemeris> ephem;
    errc_t rc = aLoadSTKEphemeris(parser, ephem);
    if(rc)
        return rc;
    ephemeris = ephem.release();
    return eNoError;
}

errc_t STKEphemerisFileParser::parse(StringView filepath, HEphemeris& ephemeris) const
{
    return aLoadSTKEphemeris(filepath, ephemeris);
}


AST_NAMESPACE_END