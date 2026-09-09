///
/// @file      CoverageDefinitionLoader.cpp
/// @brief     覆盖定义加载器实现
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

#include "CoverageDefinitionLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/CoverageDefinition.hpp"

AST_NAMESPACE_BEGIN

errc_t _aLoadGridDefinition(BKVParser& parser, CoverageDefinition& coverageDefinition)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "VersionIndicator")){
                // 处理版本指示器
            }else if(aEqualsIgnoreCase(item.key(), "LatResolution")){
                // 处理纬度分辨率
            }else if(aEqualsIgnoreCase(item.key(), "LonResolution")){
                // 处理经度分辨率
            }else if(aEqualsIgnoreCase(item.key(), "UsePolarRegions")){
                // 处理是否使用极地区域
            }else if(aEqualsIgnoreCase(item.key(), "GridClass")){
                // 处理网格类别
            }else if(aEqualsIgnoreCase(item.key(), "PointLocationMethod")){
                // 处理点位置方法
            }else if(aEqualsIgnoreCase(item.key(), "CustomPtAltMethod")){
                // 处理自定义点高度方法
            }else if(aEqualsIgnoreCase(item.key(), "CentralBody")){
                // 处理中心天体
            }else if(aEqualsIgnoreCase(item.key(), "GroundAltMethod")){
                // 处理地面高度方法
            }else if(aEqualsIgnoreCase(item.key(), "GroundAlt")){
                // 处理地面高度
            }else if(aEqualsIgnoreCase(item.key(), "AltitudeMethod")){
                // 处理高度方法
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "LatitudeBounds")){
                // 处理纬度边界
                while(1){
                    BKVItemView latItem;
                    BKVParser::EToken latToken;
                    latToken = parser.getNext(latItem);
                    if(latToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(latItem.key(), "MinLatitude")){
                            // 处理最小纬度
                        }else if(aEqualsIgnoreCase(latItem.key(), "MaxLatitude")){
                            // 处理最大纬度
                        }
                    }else if(latToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(latItem.value(), "LatitudeBounds")){
                            break;
                        }
                    }
                }
            }else if(aEqualsIgnoreCase(item.value(), "AltitudeList")){
                // 处理高度列表
                while(1){
                    BKVItemView altItem;
                    BKVParser::EToken altToken;
                    altToken = parser.getNext(altItem);
                    if(altToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(altItem.value(), "AltitudeList")){
                            break;
                        }
                    }
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "GridDefinition")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadAccessDefinition(BKVParser& parser, CoverageDefinition& coverageDefinition)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "DataRetention")){
                // 处理数据保留
            }else if(aEqualsIgnoreCase(item.key(), "Convergence")){
                // 处理收敛
            }else if(aEqualsIgnoreCase(item.key(), "AbsValueConvergence")){
                // 处理绝对收敛值
            }else if(aEqualsIgnoreCase(item.key(), "RelValueConvergence")){
                // 处理相对收敛值
            }else if(aEqualsIgnoreCase(item.key(), "MaxTimeStep")){
                // 处理最大时间步长
            }else if(aEqualsIgnoreCase(item.key(), "MinTimeStep")){
                // 处理最小时间步长
            }else if(aEqualsIgnoreCase(item.key(), "UseLightTimeDelay")){
                // 处理是否使用光时间延迟
            }else if(aEqualsIgnoreCase(item.key(), "TimeSense")){
                // 处理时间感知
            }else if(aEqualsIgnoreCase(item.key(), "DetectEventsUsingSamplesOnly")){
                // 处理是否仅使用样本检测事件
            }else if(aEqualsIgnoreCase(item.key(), "AutoRecompute")){
                // 处理是否自动重新计算
            }else if(aEqualsIgnoreCase(item.key(), "UseAllAssetAccess")){
                // 处理是否使用所有资产访问
            }else if(aEqualsIgnoreCase(item.key(), "AssetSatisType")){
                // 处理资产满足类型
            }else if(aEqualsIgnoreCase(item.key(), "AssetSatisThreshold")){
                // 处理资产满足阈值
            }else if(aEqualsIgnoreCase(item.key(), "AccessSaveMode")){
                // 处理访问保存模式
            }else if(aEqualsIgnoreCase(item.key(), "RegionAccessAccel")){
                // 处理区域访问加速
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "AccessDefinition")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadAssetList(BKVParser& parser, CoverageDefinition& coverageDefinition)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "AssetList")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadInterval(BKVParser& parser, CoverageDefinition& coverageDefinition)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "AccStart")){
                // 处理访问开始时间
            }else if(aEqualsIgnoreCase(item.key(), "AccStop")){
                // 处理访问结束时间
            }else if(aEqualsIgnoreCase(item.key(), "UseScenarioInterval")){
                // 处理是否使用场景间隔
            }else if(aEqualsIgnoreCase(item.key(), "AccIntervalPtr")){
                // 处理访问间隔指针
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
            if(aEqualsIgnoreCase(item.value(), "Interval")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadExtensions(BKVParser& parser, CoverageDefinition& coverageDefinition)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "ExternData")){
                // 处理ExternData
                while(1){
                    BKVItemView externItem;
                    BKVParser::EToken externToken;
                    externToken = parser.getNext(externItem);
                    if(externToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(externItem.value(), "ExternData")){
                            break;
                        }
                    }
                }
            }else if(aEqualsIgnoreCase(item.value(), "ADFFileData")){
                // 处理ADFFileData
                while(1){
                    BKVItemView adfItem;
                    BKVParser::EToken adfToken;
                    adfToken = parser.getNext(adfItem);
                    if(adfToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(adfItem.value(), "ADFFileData")){
                            break;
                        }
                    }
                }
            }else if(aEqualsIgnoreCase(item.value(), "Desc")){
                // 处理Desc
                while(1){
                    BKVItemView descItem;
                    BKVParser::EToken descToken;
                    descToken = parser.getNext(descItem);
                    if(descToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(descItem.value(), "Desc")){
                            break;
                        }
                    }
                }
            }else if(aEqualsIgnoreCase(item.value(), "Crdn")){
                // 处理Crdn
                while(1){
                    BKVItemView crdnItem;
                    BKVParser::EToken crdnToken;
                    crdnToken = parser.getNext(crdnItem);
                    if(crdnToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(crdnItem.value(), "Crdn")){
                            break;
                        }
                    }
                }
            }else if(aEqualsIgnoreCase(item.value(), "Graphics")){
                // 处理Graphics
                while(1){
                    BKVItemView graphicsItem;
                    BKVParser::EToken graphicsToken;
                    graphicsToken = parser.getNext(graphicsItem);
                    if(graphicsToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(graphicsItem.value(), "Graphics")){
                            break;
                        }
                    }
                }
            }else if(aEqualsIgnoreCase(item.value(), "VO")){
                // 处理VO
                while(1){
                    BKVItemView voItem;
                    BKVParser::EToken voToken;
                    voToken = parser.getNext(voItem);
                    if(voToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(voItem.value(), "VO")){
                            break;
                        }
                    }
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t aLoadCoverageDefinition(StringView filepath, CoverageDefinition& coverageDefinition)
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
            if(aEqualsIgnoreCase(item.value(), "CoverageDefinition")){
                BKVItemView cdItem;
                BKVParser::EToken cdToken;
                do{
                    cdToken = parser.getNext(cdItem);
                    if(cdToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(cdItem.key(), "Name")){
                            coverageDefinition.setName(cdItem.value());
                        }
                    }else if(cdToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(cdItem.value(), "GridDefinition")){
                            if(errc_t rc = _aLoadGridDefinition(parser, coverageDefinition)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(cdItem.value(), "AccessDefinition")){
                            if(errc_t rc = _aLoadAccessDefinition(parser, coverageDefinition)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(cdItem.value(), "AssetList")){
                            if(errc_t rc = _aLoadAssetList(parser, coverageDefinition)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(cdItem.value(), "Interval")){
                            if(errc_t rc = _aLoadInterval(parser, coverageDefinition)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(cdItem.value(), "Extensions")){
                            if(errc_t rc = _aLoadExtensions(parser, coverageDefinition)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(cdItem.value(), "SubObjects")){
                            if(errc_t rc = _aLoadSubObjects(parser, &coverageDefinition)){
                                return rc;
                            }
                        }
                    }else if(cdToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(cdItem.value(), "CoverageDefinition")){
                            return eNoError;
                        }
                    }
                }while(cdToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END