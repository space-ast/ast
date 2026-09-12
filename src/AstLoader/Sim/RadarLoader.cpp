///
/// @file      RadarLoader.cpp
/// @brief     雷达加载器实现
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

#include "RadarLoader.hpp"
#include "AstSim/Radar.hpp"
#include "BasicComponentLoader.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/BKVParser.hpp"

AST_NAMESPACE_BEGIN

/// @brief 加载雷达模式设置
/// @param parser BKV解析器
/// @param radar 雷达引用
/// @return 错误码
errc_t _aLoadRadarPattern(BKVParser& parser, Radar& radar)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Type")){
                // 处理 Type
            }else if(aEqualsIgnoreCase(item.key(), "Frequency")){
                // 处理 Frequency
            }else if(aEqualsIgnoreCase(item.key(), "Bandwidth")){
                // 处理 Bandwidth
            }else if(aEqualsIgnoreCase(item.key(), "Polarization")){
                // 处理 Polarization
            }else if(aEqualsIgnoreCase(item.key(), "TransmitPower")){
                // 处理 TransmitPower
            }else if(aEqualsIgnoreCase(item.key(), "Gain")){
                // 处理 Gain
            }else if(aEqualsIgnoreCase(item.key(), "BeamWidth")){
                // 处理 BeamWidth
            }else if(aEqualsIgnoreCase(item.key(), "SidelobeLevel")){
                // 处理 SidelobeLevel
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Pattern")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载雷达指向设置
/// @param parser BKV解析器
/// @param radar 雷达引用
/// @return 错误码
errc_t _aLoadRadarPointing(BKVParser& parser, Radar& radar)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "PointingMethod")){
                // 处理 PointingMethod
            }else if(aEqualsIgnoreCase(item.key(), "Sequence")){
                // 处理 Sequence
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Fixed")){
                // 处理 Fixed 块
                BKVItemView fixedItem;
                BKVParser::EToken fixedToken;
                do{
                    fixedToken = parser.getNext(fixedItem);
                    if(fixedToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(fixedItem.key(), "Qx")){
                            // 处理 Qx
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qy")){
                            // 处理 Qy
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qz")){
                            // 处理 Qz
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qs")){
                            // 处理 Qs
                        }
                    }
                }while(fixedToken != BKVParser::eBlockEnd);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Pointing")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载雷达处理设置
/// @param parser BKV解析器
/// @param radar 雷达引用
/// @return 错误码
errc_t _aLoadRadarProcessing(BKVParser& parser, Radar& radar)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "PRF")){
                // 处理 PRF
            }else if(aEqualsIgnoreCase(item.key(), "PulseWidth")){
                // 处理 PulseWidth
            }else if(aEqualsIgnoreCase(item.key(), "RangeResolution")){
                // 处理 RangeResolution
            }else if(aEqualsIgnoreCase(item.key(), "AzimuthResolution")){
                // 处理 AzimuthResolution
            }else if(aEqualsIgnoreCase(item.key(), "MaxRange")){
                // 处理 MaxRange
            }else if(aEqualsIgnoreCase(item.key(), "MinRange")){
                // 处理 MinRange
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Processing")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载雷达扩展设置
/// @param parser BKV解析器
/// @param radar 雷达引用
/// @return 错误码
errc_t _aLoadRadarExtensions(BKVParser& parser, Radar& radar)
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

/// @brief 加载雷达对象
/// @param parser BKV解析器
/// @param objectType 对象类型
/// @param radar 雷达引用
/// @return 错误码
errc_t aLoadRadar(BKVParser &parser, StringView objectType, Radar &radar)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Name")){
                radar.setName(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Pattern")){
                if(errc_t rc = _aLoadRadarPattern(parser, radar)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Pointing")){
                if(errc_t rc = _aLoadRadarPointing(parser, radar)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Processing")){
                if(errc_t rc = _aLoadRadarProcessing(parser, radar)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Extensions")){
                if(errc_t rc = _aLoadRadarExtensions(parser, radar)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SubObjects")){
                if(errc_t rc = _aLoadSubObjects(parser, &radar)){
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

/// @brief 加载雷达对象
/// @param parser BKV解析器
/// @param radar 雷达引用
/// @return 错误码
errc_t aLoadRadar(BKVParser& parser, Radar& radar)
{
    return aLoadRadar(parser, "Radar", radar);
}

/// @brief 加载雷达对象
/// @param filepath 文件路径
/// @param objectType 对象类型
/// @param radar 雷达引用
/// @return 错误码
errc_t aLoadRadar(StringView filepath, StringView objectType, Radar& radar)
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
            return aLoadRadar(parser, objectType, radar);
        }
    }while(token != BKVParser::eEOF);
    
    return eNoError;
}

/// @brief 加载雷达对象
/// @param filepath 文件路径
/// @param radar 雷达引用
/// @return 错误码
errc_t aLoadRadar(StringView filepath, Radar& radar)
{
    return aLoadRadar(filepath, "Radar", radar);
}

AST_NAMESPACE_END