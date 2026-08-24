///
/// @file      BasicComponentLoader.cpp
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

#include "BasicComponentLoader.hpp"
#include "AstLoader/ObjectLoader.hpp"
#include "AstCore/EventTime.hpp"
#include "AstCore/EventTimeExplicit.hpp"
#include "AstCore/EventIntervalExplicit.hpp"
#include "AstCore/EventIntervalLinkTo.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN



StringView aClassSTKExtension(StringView objectType)
{
    if(aEqualsIgnoreCase(objectType, "Scenario"))
    {
        return "sc";
    }
    else if(aEqualsIgnoreCase(objectType, "Aircraft"))
    {
        return "ac";
    }
    else if(aEqualsIgnoreCase(objectType, "Chain"))
    {
        return "c";
    }
    else if(aEqualsIgnoreCase(objectType, "Facility"))
    {
        return "f";
    }
    else if(aEqualsIgnoreCase(objectType, "GroundVehicle"))
    {
        return "gv";
    }
    else if(aEqualsIgnoreCase(objectType, "LaunchVehicle"))
    {
        return "lv";
    }
    else if(aEqualsIgnoreCase(objectType, "Missile"))
    {
        return "mi";
    }
    else if(aEqualsIgnoreCase(objectType, "Planet"))
    {
        return "pl";
    }
    else if(aEqualsIgnoreCase(objectType, "Receiver"))
    {
        return "r";
    }
    else if(aEqualsIgnoreCase(objectType, "Satellite"))
    {
        return "sa";
    }
    else if(aEqualsIgnoreCase(objectType, "Sensor"))
    {
        return "sn";
    }
    else if(aEqualsIgnoreCase(objectType, "Ship"))
    {
        return "sh";
    }
    else if(aEqualsIgnoreCase(objectType, "Submarine"))
    {
        return "su";
    }
    else if(aEqualsIgnoreCase(objectType, "Transmitter"))
    {
        return "x";
    }
    else if(aEqualsIgnoreCase(objectType, "Place"))
    {
        return "plc";
    }
    else if(aEqualsIgnoreCase(objectType, "Target"))
    {
        return "t";
    }
    else if(aEqualsIgnoreCase(objectType, "AdvCAT"))
    {
        return "ca";
    }
    else if(aEqualsIgnoreCase(objectType, "Antenna"))
    {
        return "antenna";
    }
    else if(aEqualsIgnoreCase(objectType, "AreaTarget"))
    {
        return "at";
    }
    else if(aEqualsIgnoreCase(objectType, "AttitudeCoverage"))
    {
        return "acv";
    }
    else if(aEqualsIgnoreCase(objectType, "AttitudeFigureOfMerit"))
    {
        return "afm";
    }
    else if(aEqualsIgnoreCase(objectType, "CommSystem"))
    {
        return "cs";
    }
    else if(aEqualsIgnoreCase(objectType, "Constellation"))
    {
        return "cn";
    }
    else if(aEqualsIgnoreCase(objectType, "CoverageDefinition"))
    {
        return "cv";
    }
    else if(aEqualsIgnoreCase(objectType, "FigureOfMerit"))
    {
        return "fm";
    }
    else if(aEqualsIgnoreCase(objectType, "LineTarget"))
    {
        return "lt";
    }
    else if(aEqualsIgnoreCase(objectType, "MTO"))
    {
        return "mt";
    }
    else if(aEqualsIgnoreCase(objectType, "Radar"))
    {
        return "rd";
    }
    else if(aEqualsIgnoreCase(objectType, "Star"))
    {
        return "st";
    }
    else if(aEqualsIgnoreCase(objectType, "Volumetric"))
    {
        return "vo";
    }
    else
    {
        aError("unknown object class: '%.*s'", objectType.size(), objectType.data());    
        return "";
    }
}


errc_t _aLoadEventTimeImplicit(BKVParser& parser, SharedPtr<EventTime>& eventTime)
{
    _aSkipUnknownBlock(parser, "EVENT");
    return 0;
}

errc_t _aLoadEventTime(BKVParser& parser, SharedPtr<EventTime>& eventTime)
{
    BKVItemView item;
    BKVParser::EToken token;
    TimePoint epoch{};
    bool isImplicit = false;
    SharedPtr<EventTime> internal_time;
    token = parser.getNext(item);
    if(token != BKVParser::eBlockBegin || !aEqualsIgnoreCase(item.value(), "EVENT")){
        aError("expect EVENT block");
        return -1;
    }
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Epoch")){
                epoch = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "EpochState")){
                isImplicit = aEqualsIgnoreCase(item.value(), "Implicit");  // "Implicit" 或 "Explicit"
            }
        }
        else if(token == BKVParser::eBlockBegin)
        {
            if(aEqualsIgnoreCase(item.value(), "EVENT")){
                // 一定是 Implicit 类型吗?
                _aLoadEventTimeImplicit(parser, internal_time);
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "EVENT")){
                if(isImplicit && internal_time){
                    eventTime = internal_time;
                }else{
                    eventTime = EventTimeExplicit::New(epoch);
                }
            }else{
                // @todo 处理异常情况
            }
            break;
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadEventIntervalImplicit(BKVParser& parser, SharedPtr<EventInterval>& eventInterval)
{
    BKVItemView item;
    BKVParser::EToken token;
    std::string type;  // 这个变量目前没有使用
    std::string name;
    std::string absolutePath;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Type")){
                type = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "Name")){
                name = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "AbsolutePath")){
                absolutePath = item.value().toString();
            }
        }
        else if(token == BKVParser::eBlockBegin)
        {

        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "EVENTINTERVAL")){
                eventInterval = EventIntervalLinkTo::New(name, absolutePath);
            }else{
                // @todo 处理异常情况
            }
            break;
        }
    }while(token != BKVParser::eBlockEnd);
    return eNoError;
}

errc_t _aLoadInterval(BKVParser& parser, TimeInterval& interval)
{
    TimePoint start{};
    TimePoint stop{};
    BKVItemView item;
    while(1){
        BKVParser::EToken token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Start")){
                start = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "Stop")){
                stop = TimePoint::Parse(item.value());
            }else{
                aError("unsupported key %.*s", item.key().size(), item.key().data());
                return -1;
            }
        }
        else if(token == BKVParser::eBlockEnd){
            interval.setBounds(start, stop);
            break;
        }else{
            return 0;
        }
    }
    return 0;
}

errc_t _aLoadEventInterval(BKVParser& parser, SharedPtr<EventInterval>& eventInterval)
{
    BKVItemView item;
    BKVParser::EToken token;
    bool isImplicit = false;
    TimeInterval interval{};
    SharedPtr<EventInterval> internal_interval;
    token = parser.getNext(item);
    if(token != BKVParser::eBlockBegin || !aEqualsIgnoreCase(item.value(), "EVENTINTERVAL")){
        aError("expect EVENTINTERVAL block");
        return -1;
    }
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "EventInterval")){
                // @todo
            }else if(aEqualsIgnoreCase(item.key(), "IntervalState")){
                isImplicit = aEqualsIgnoreCase(item.value(), "Implicit");  // "Implicit" 或 "Explicit"
            }
        }
        else if(token == BKVParser::eBlockBegin)
        {
            if(aEqualsIgnoreCase(item.value(), "EVENTINTERVAL")){
                // 一定是 Implicit 类型吗?
                _aLoadEventIntervalImplicit(parser, internal_interval);
            }
            else if(aEqualsIgnoreCase(item.value(), "Interval"))
            {
                errc_t rc = _aLoadInterval(parser, interval);
                if(rc != 0){
                    aError("Interval is invalid");
                    return rc;
                }
            }
            else{
                aError("unsupported block type %.*s", item.value().size(), item.value().data());
                return -1;
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "EVENTINTERVAL")){
                if(isImplicit && internal_interval){
                    eventInterval = internal_interval;
                }else{
                    eventInterval = EventIntervalExplicit::New(interval);
                }
            }else{
                // @todo 处理异常情况
            }
            break;
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadSubObjects(BKVParser &parser, Object *parentObject)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin)
        {
            if(aEqualsIgnoreCase(item.value(), "SubObject")){
                
            }
        }
        else if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Class")){
                std::string objectType = item.value().toString();
                while(1)
                {
                    StringView objectName = parser.getLineSkipHashComment();
                    objectName = aStripAsciiWhitespace(objectName);
                    if(objectName.empty())
                    {
                        continue;
                    }
                    else if(objectName.starts_with("END "))
                    {
                        break;
                    }
                    else
                    {
                        std::string fileNameWithExt = std::string(objectName) + "." + std::string(aClassSTKExtension(objectType));
                        std::string filePath = fs::path(parser.getFilePath()).parent_path() / fileNameWithExt;
                        SharedPtr<Object> object;
                        errc_t rc = aLoadObject(filePath, objectType, object);
                        if(rc)
                        {
                            aError("failed to load sub object '%.*s'", filePath.size(), filePath.data());
                        }
                        if(object){
                            rc = aSetParentScope(object, parentObject);
                            if(rc)
                            {
                                aError("failed to set parent scope for sub object '%.*s'", filePath.size(), filePath.data());
                            }
                        }else{
                            aError("object is null");
                        }
                    }
                }
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "SubObject")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

void _aSkipUnknownBlock(BKVParser &parser, StringView blockName)
{
    int blockDepth = 0;
    std::string blockNameStr = std::string(blockName);
    while(1)
    {
        BKVItemView item;
        BKVParser::EToken token;
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin)
        {
            blockDepth++;
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(blockDepth == 0 && aEqualsIgnoreCase(item.value(), blockNameStr)){
                break;
            }
            blockDepth--;
        }
        else if(token == BKVParser::eEOF)
        {
            break;
        }else if(token == BKVParser::eError)
        {
            break;
        }
    }
}

AST_NAMESPACE_END
