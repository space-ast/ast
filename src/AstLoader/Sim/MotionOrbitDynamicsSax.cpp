///
/// @file      MotionSax.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-19
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

#include "MotionOrbitDynamicsSax.hpp"
#include "CommonlyUsedHeaders.hpp"


AST_NAMESPACE_BEGIN

errc_t MotionOrbitDynamicsSax::begin(StringView name)
{
    if(aEqualsIgnoreCase(name, "DisplayFlags"))
    {
        // @todo 处理显示标志
        BKVItemView item;
        BKVParser::EToken token;
        do{
            token = parser_.getNext(item);
        }while(token != BKVParser::eBlockEnd && token != BKVParser::eEOF);
    }
    else
    {
        _aSkipUnknownBlock(parser_, name);
    }
    return eNoError;
}

errc_t MotionOrbitDynamicsSax::end(StringView name)
{
    return endParse;
}

errc_t MotionOrbitDynamicsSax::keyValue(StringView key, const ValueView &value)
{
    if(aEqualsIgnoreCase(key, "EphemSmartEpoch")){
        if(errc_t rc = _aLoadEventTime(parser_, ephemSmartEpoch_))
        {
            aError("EphemSmartEpoch is invalid");
            return rc;
        }
    }else if(aEqualsIgnoreCase(key, "EphemSmartInterval")){
        if(errc_t rc = _aLoadEventInterval(parser_, ephemSmartInterval_))
        {
            aError("EphemSmartInterval is invalid");
            return rc;
        }
    }else if(aEqualsIgnoreCase(key, "EphemEpoch")){
        // @note 这不是与EphemSmartEpoch重复了吗？为什么有两个地方都有值，应该以哪个为准?
        ephemEpoch_ = TimePoint::Parse(value);
    }else if(aEqualsIgnoreCase(key, "StartTime")){
        // @note 这里也和EphemSmartInterval重复了，但是这里才有实际数据
        startTime_ = TimePoint::Parse(value);
    }else if(aEqualsIgnoreCase(key, "StopTime")){
        // @note 这里也和EphemSmartInterval重复了，但是这里才有实际数据
        stopTime_ = TimePoint::Parse(value);
    }else if(aEqualsIgnoreCase(key, "UseScenTime")){
        useScenTime_ = value.toBool();
    }
    return eNoError;
}

HEventTime MotionOrbitDynamicsSax::makeStateEpoch() const
{
    auto explicitEpoch = EventTimeExplicit::New(ephemEpoch_);
    return EventTimeFallback::New(ephemSmartEpoch_, explicitEpoch);
}

HEventInterval MotionOrbitDynamicsSax::makeInterval() const
{
    auto explicitInterval = EventIntervalExplicit::New(startTime_, stopTime_);
    return EventIntervalFallback::New(ephemSmartInterval_, explicitInterval);
}

Body *MotionOrbitDynamicsSax::getBody() const
{
    if(!vehiclePathData_.centralBody_)
    {
        auto body = aGetDefaultBody();
        aWarning("vehicle's centralBody is not set, use default body '%s' instead", body->getName().c_str());
        return body;
    }
    return vehiclePathData_.centralBody_;
}

AST_NAMESPACE_END

