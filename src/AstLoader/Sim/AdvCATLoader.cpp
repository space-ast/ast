///
/// @file      AdvCATLoader.cpp
/// @brief     高级CAT加载器实现
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

#include "AdvCATLoader.hpp"
#include "AstSim/AdvCAT.hpp"
#include "AstLoader/BasicComponentLoader.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/BKVParser.hpp"

AST_NAMESPACE_BEGIN

/// @brief 加载时间间隔设置
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadInterval(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "StartTime")){
                // 处理 StartTime
            }else if(aEqualsIgnoreCase(item.key(), "StopTime")){
                // 处理 StopTime
            }else if(aEqualsIgnoreCase(item.key(), "AnalysisWindowPtr")){
                // 处理 AnalysisWindowPtr
            }else if(aEqualsIgnoreCase(item.key(), "DisplayComputeAck")){
                // 处理 DisplayComputeAck
            }else if(aEqualsIgnoreCase(item.key(), "MsgAlertOnHit")){
                // 处理 MsgAlertOnHit
            }else if(aEqualsIgnoreCase(item.key(), "ForceRePropOnSatFileLoad")){
                // 处理 ForceRePropOnSatFileLoad
            }else if(aEqualsIgnoreCase(item.key(), "AllowPartialEphemeris")){
                // 处理 AllowPartialEphemeris
            }else if(aEqualsIgnoreCase(item.key(), "RemoveSecondaryBySSC")){
                // 处理 RemoveSecondaryBySSC
            }else if(aEqualsIgnoreCase(item.key(), "MinSampleStepSize")){
                // 处理 MinSampleStepSize
            }else if(aEqualsIgnoreCase(item.key(), "MaxSampleStepSize")){
                // 处理 MaxSampleStepSize
            }else if(aEqualsIgnoreCase(item.key(), "ComputeOnLoad")){
                // 处理 ComputeOnLoad
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "EVENTINTERVAL")){
                // 处理 EVENTINTERVAL 块
                BKVItemView eventItem;
                BKVParser::EToken eventToken;
                do{
                    eventToken = parser.getNext(eventItem);
                    if(eventToken == BKVParser::eKeyValue){
                        // 处理 EVENTINTERVAL 内的键值对
                    }else if(eventToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(eventItem.value(), "EVENTINTERVAL")){
                            // 处理嵌套的 EVENTINTERVAL 块
                            BKVItemView nestedEventItem;
                            BKVParser::EToken nestedEventToken;
                            do{
                                nestedEventToken = parser.getNext(nestedEventItem);
                                if(nestedEventToken == BKVParser::eKeyValue){
                                    // 处理嵌套的 EVENTINTERVAL 内的键值对
                                }
                            }while(nestedEventToken != BKVParser::eBlockEnd);
                        }
                    }
                }while(eventToken != BKVParser::eBlockEnd);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Interval")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载过滤设置
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadFilterSettings(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Threshold")){
                // 处理 Threshold
            }else if(aEqualsIgnoreCase(item.key(), "ApogeePerigeePad")){
                // 处理 ApogeePerigeePad
            }else if(aEqualsIgnoreCase(item.key(), "OrbitPathPad")){
                // 处理 OrbitPathPad
            }else if(aEqualsIgnoreCase(item.key(), "TimeDistancePad")){
                // 处理 TimeDistancePad
            }else if(aEqualsIgnoreCase(item.key(), "OutOfDateDuration")){
                // 处理 OutOfDateDuration
            }else if(aEqualsIgnoreCase(item.key(), "UseOutOfDateFilter")){
                // 处理 UseOutOfDateFilter
            }else if(aEqualsIgnoreCase(item.key(), "UseApogeePerigeeFilter")){
                // 处理 UseApogeePerigeeFilter
            }else if(aEqualsIgnoreCase(item.key(), "UsePathFilter")){
                // 处理 UsePathFilter
            }else if(aEqualsIgnoreCase(item.key(), "UseTimeFilter")){
                // 处理 UseTimeFilter
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "FilterSettings")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载概率数据
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadProbabilityData(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Resolution")){
                // 处理 Resolution
            }else if(aEqualsIgnoreCase(item.key(), "AngleLimit")){
                // 处理 AngleLimit
            }else if(aEqualsIgnoreCase(item.key(), "SigmaLimit")){
                // 处理 SigmaLimit
            }else if(aEqualsIgnoreCase(item.key(), "SigmaFinal")){
                // 处理 SigmaFinal
            }else if(aEqualsIgnoreCase(item.key(), "FractionalProbLimit")){
                // 处理 FractionalProbLimit
            }else if(aEqualsIgnoreCase(item.key(), "MaxTimeStep")){
                // 处理 MaxTimeStep
            }else if(aEqualsIgnoreCase(item.key(), "HalfTimeSpan")){
                // 处理 HalfTimeSpan
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "ProbabilityData")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载全局椭球属性
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadGlobalEllipsoidProperties(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "ScaleFactor")){
                // 处理 ScaleFactor
            }else if(aEqualsIgnoreCase(item.key(), "QuadraticDB")){
                // 处理 QuadraticDB
            }else if(aEqualsIgnoreCase(item.key(), "FixedByOrbitClassDB")){
                // 处理 FixedByOrbitClassDB
            }else if(aEqualsIgnoreCase(item.key(), "QuadraticByOrbitClassDB")){
                // 处理 QuadraticByOrbitClassDB
            }else if(aEqualsIgnoreCase(item.key(), "CrossReferenceDB")){
                // 处理 CrossReferenceDB
            }else if(aEqualsIgnoreCase(item.key(), "UseCrossRefDb")){
                // 处理 UseCrossRefDb
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "GlobalEllipsoidProperties")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载日志文件设置
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadLogFileSettings(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "OutputToLogFile")){
                // 处理 OutputToLogFile
            }else if(aEqualsIgnoreCase(item.key(), "BufferOutput")){
                // 处理 BufferOutput
            }else if(aEqualsIgnoreCase(item.key(), "FileMode")){
                // 处理 FileMode
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "LogFileSettings")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载关系文件设置
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadRelationFile(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "RelationFileName")){
                // 处理 RelationFileName
            }else if(aEqualsIgnoreCase(item.key(), "UseRelFile")){
                // 处理 UseRelFile
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "RelationFile")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载SSC硬体半径文件设置
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadSSCHardBodyRadFile(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "UseSSCFile")){
                // 处理 UseSSCFile
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "SSCHardBodyRadFile")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载主要对象设置
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadPrimaries(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "DefaultEllipsoid")){
                // 处理 DefaultEllipsoid 块
                BKVItemView ellipsoidItem;
                BKVParser::EToken ellipsoidToken;
                do{
                    ellipsoidToken = parser.getNext(ellipsoidItem);
                    if(ellipsoidToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(ellipsoidItem.key(), "Tangential")){
                            // 处理 Tangential
                        }else if(aEqualsIgnoreCase(ellipsoidItem.key(), "CrossTrack")){
                            // 处理 CrossTrack
                        }else if(aEqualsIgnoreCase(ellipsoidItem.key(), "Normal")){
                            // 处理 Normal
                        }else if(aEqualsIgnoreCase(ellipsoidItem.key(), "EllipsoidType")){
                            // 处理 EllipsoidType
                        }
                    }
                }while(ellipsoidToken != BKVParser::eBlockEnd);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Primaries")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载次要对象设置
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadSecondaries(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "DefaultEllipsoid")){
                // 处理 DefaultEllipsoid 块
                BKVItemView ellipsoidItem;
                BKVParser::EToken ellipsoidToken;
                do{
                    ellipsoidToken = parser.getNext(ellipsoidItem);
                    if(ellipsoidToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(ellipsoidItem.key(), "Tangential")){
                            // 处理 Tangential
                        }else if(aEqualsIgnoreCase(ellipsoidItem.key(), "CrossTrack")){
                            // 处理 CrossTrack
                        }else if(aEqualsIgnoreCase(ellipsoidItem.key(), "Normal")){
                            // 处理 Normal
                        }else if(aEqualsIgnoreCase(ellipsoidItem.key(), "EllipsoidType")){
                            // 处理 EllipsoidType
                        }
                    }
                }while(ellipsoidToken != BKVParser::eBlockEnd);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Secondaries")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载扩展设置
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t _aLoadExtensions(BKVParser& parser, AdvCAT& advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "ADFFileData")){
                // 处理 ADFFileData 块
                BKVItemView adfItem;
                BKVParser::EToken adfToken;
                do{
                    adfToken = parser.getNext(adfItem);
                }while(adfToken != BKVParser::eBlockEnd);
            }else if(aEqualsIgnoreCase(item.value(), "Desc")){
                // 处理 Desc 块
                BKVItemView descItem;
                BKVParser::EToken descToken;
                do{
                    descToken = parser.getNext(descItem);
                }while(descToken != BKVParser::eBlockEnd);
            }else if(aEqualsIgnoreCase(item.value(), "VO")){
                // 处理 VO 块
                BKVItemView voItem;
                BKVParser::EToken voToken;
                do{
                    voToken = parser.getNext(voItem);
                }while(voToken != BKVParser::eBlockEnd);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载高级CAT对象
/// @param parser BKV解析器
/// @param objectType 对象类型
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t aLoadAdvCAT(BKVParser &parser, StringView objectType, AdvCAT &advCAT)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Name")){
                advCAT.setName(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Interval")){
                if(errc_t rc = _aLoadInterval(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "FilterSettings")){
                if(errc_t rc = _aLoadFilterSettings(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "ProbabilityData")){
                if(errc_t rc = _aLoadProbabilityData(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "GlobalEllipsoidProperties")){
                if(errc_t rc = _aLoadGlobalEllipsoidProperties(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "LogFileSettings")){
                if(errc_t rc = _aLoadLogFileSettings(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "RelationFile")){
                if(errc_t rc = _aLoadRelationFile(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SSCHardBodyRadFile")){
                if(errc_t rc = _aLoadSSCHardBodyRadFile(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Primaries")){
                if(errc_t rc = _aLoadPrimaries(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Secondaries")){
                if(errc_t rc = _aLoadSecondaries(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Extensions")){
                if(errc_t rc = _aLoadExtensions(parser, advCAT)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SubObjects")){
                if(errc_t rc = _aLoadSubObjects(parser, &advCAT)){
                    return rc;
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), objectType)){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载高级CAT对象
/// @param parser BKV解析器
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t aLoadAdvCAT(BKVParser& parser, AdvCAT& advCAT)
{
    return aLoadAdvCAT(parser, "AdvCAT", advCAT);
}

/// @brief 加载高级CAT对象
/// @param filepath 文件路径
/// @param objectType 对象类型
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t aLoadAdvCAT(StringView filepath, StringView objectType, AdvCAT& advCAT)
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
        if(token == BKVParser::eKeyValue){
            // 处理文件开头的版本信息和作者信息
        }
        else if(token == BKVParser::eBlockBegin){
            // 第一个BEGIN END块的名称就是对象的类型
            StringView objectType = item.value();
            return aLoadAdvCAT(parser, objectType, advCAT);
        }
    }while(token != BKVParser::eEOF);
    
    return eNoError;
}

/// @brief 加载高级CAT对象
/// @param filepath 文件路径
/// @param advCAT 高级CAT对象引用
/// @return 错误码
errc_t aLoadAdvCAT(StringView filepath, AdvCAT& advCAT)
{
    return aLoadAdvCAT(filepath, "AdvCAT", advCAT);
}

AST_NAMESPACE_END