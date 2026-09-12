///
/// @file      ScenarioLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-07
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

#include "ScenarioLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "BasicComponentLoader.hpp"
#include "AstSim/Scenario.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/EventTimeFallback.hpp"


AST_NAMESPACE_BEGIN

errc_t _aLoadEpoch(BKVParser& parser, Scenario& scenario)
{
    TimePoint epoch{};
    HEventTime eventTime;
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Epoch")){
                epoch = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "SmartEpoch")){
                errc_t rc = _aLoadEventTime(parser, eventTime);
                if(rc){
                    aError(_("加载 SmartEpoch 失败"));
                }
            }
        }else if(token == BKVParser::eBlockBegin){
            
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Epoch")){
                auto fallback = EventTimeFallback::New(eventTime, epoch);
                scenario.setEpoch(fallback);
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadInterval(BKVParser& parser, Scenario& scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    TimePoint startTime{}, stopTime{};
    HEventInterval eventInterval;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Start")){
                startTime = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "Stop")){
                stopTime = TimePoint::Parse(item.value());
            }else if(aEqualsIgnoreCase(item.key(), "SmartInterval")){
                errc_t rc = _aLoadEventInterval(parser, eventInterval);
                if(rc){
                    aError(_("加载 SmartInterval 失败"));
                }
            }else if(aEqualsIgnoreCase(item.key(), "EpochUsesAnalStart")){
                // 处理 EpochUsesAnalStart
            }else if(aEqualsIgnoreCase(item.key(), "AnimStartUsesAnalStart")){
                // 处理 AnimStartUsesAnalStart
            }else if(aEqualsIgnoreCase(item.key(), "AnimStopUsesAnalStop")){
                // 处理 AnimStopUsesAnalStop
            }
        }else if(token == BKVParser::eBlockBegin){
            
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Interval")){
                TimeInterval interval(startTime, stopTime);
                auto fallback = EventIntervalFallback::New(eventInterval, interval);
                scenario.setInterval(fallback);
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadEOPFile(BKVParser& parser, Scenario& scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "InheritEOPSource")){
                scenario.setInheritEOPSource(item.value().toBool());
            }else if(aEqualsIgnoreCase(item.key(), "EOPFilename")){
                scenario.setEOPFileName(item.value());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "EOPFile")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadGlobalPrefs(BKVParser& parser, Scenario& scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            // 处理各种全局偏好设置
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "GlobalPrefs")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadCentralBody(BKVParser& parser, Scenario& scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "PrimaryBody")){
                // 处理 PrimaryBody
                auto body = aGetBody(item.value());
                if(!body){
                    aError(_("未找到主天体 '%.*s'"), item.value().size(), item.value().data());
                }
                scenario.setPrimaryBody(body);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "CentralBody")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadCentralBodyTerrain(BKVParser& parser, Scenario& scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "CentralBody")){
                // 处理 CentralBody 块
                while(1){
                    BKVItemView bodyItem;
                    BKVParser::EToken bodyToken;
                    bodyToken = parser.getNext(bodyItem);
                    if(bodyToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(bodyItem.value(), "CentralBody")){
                            break;
                        }
                    }
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "CentralBodyTerrain")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadStarCollection(BKVParser& parser, Scenario& scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Name")){
                // 处理 StarCollection 名称
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "StarCollection")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadScenarioLicenses(BKVParser& parser, Scenario& scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Module")){
                // 处理模块许可证
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "ScenarioLicenses")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadExtensions(BKVParser& parser, Scenario& scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            // 处理各种扩展块
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t aLoadScenario(StringView filepath, Scenario &scenario)
{
    BKVItemView item;
    BKVParser::EToken token;
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        aError(_("打开文件 '%.*s' 失败"), (int)filepath.size(), filepath.data());
        return eErrorInvalidFile;
    }
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            // 处理文件开头的版本信息和作者信息
        }
        else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Scenario")){
                // 处理 Scenario 块
                BKVItemView scenarioItem;
                BKVParser::EToken scenarioToken;
                do{
                    scenarioToken = parser.getNext(scenarioItem);
                    if(scenarioToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(scenarioItem.key(), "Name")){
                            scenario.setName(scenarioItem.value());
                        }
                    }else if(scenarioToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(scenarioItem.value(), "Epoch")){
                            if(errc_t rc = _aLoadEpoch(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "Interval")){
                            if(errc_t rc = _aLoadInterval(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "EOPFile")){
                            if(errc_t rc = _aLoadEOPFile(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "GlobalPrefs")){
                            if(errc_t rc = _aLoadGlobalPrefs(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "CentralBody")){
                            if(errc_t rc = _aLoadCentralBody(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "CentralBodyTerrain")){
                            if(errc_t rc = _aLoadCentralBodyTerrain(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "StarCollection")){
                            if(errc_t rc = _aLoadStarCollection(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "ScenarioLicenses")){
                            if(errc_t rc = _aLoadScenarioLicenses(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "Extensions")){
                            if(errc_t rc = _aLoadExtensions(parser, scenario)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(scenarioItem.value(), "SubObjects")){
                            if(errc_t rc = _aLoadSubObjects(parser, &scenario)){
                                return rc;
                            }
                        }
                    }else if(scenarioToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(scenarioItem.value(), "Scenario")){
                            return eNoError;
                        }
                    }
                }while(scenarioToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


AST_NAMESPACE_END
