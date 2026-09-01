///
/// @file      FacilityLoader.cpp
/// @brief     设施加载器
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

#include "FacilityLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/Facility.hpp"
#include "AstSim/CentroidPosition.hpp"
#include "AstCore/RunTimeSolarSystem.hpp"

AST_NAMESPACE_BEGIN

/// @brief 加载设施位置信息
/// @param parser BKV解析器
/// @param facility 设施引用
/// @return 错误码
errc_t _aLoadFacilityCentroidPosition(BKVParser& parser, Facility& facility)
{
    BKVItemView item;
    BKVParser::EToken token;
    Body* body = nullptr;
    GeodeticPoint position{};
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "CentralBody")){
                StringView bodyname = item.value();
                body = aGetBody(bodyname);
                if(!body){
                    aError("failed to get body '%.*s'", bodyname.size(), bodyname.data());
                }
            }else if(aEqualsIgnoreCase(item.key(), "EcfLatitude")){
                position.setLatitude(item.value().toAngleRad());
            }else if(aEqualsIgnoreCase(item.key(), "EcfLongitude")){
                position.setLongitude(item.value().toAngleRad());
            }else if(aEqualsIgnoreCase(item.key(), "EcfAltitude")){
                position.setAltitude(item.value().toDouble());
            }else if(aEqualsIgnoreCase(item.key(), "HeightAboveGround")){
                // @todo 设置地面高度
                // aWarning("unsupported feature: HeightAboveGround");
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "CentroidPosition")){
                facility.setPosition(position);
                facility.setBody(body);
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载设施图形属性
/// @param parser BKV解析器
/// @param facility 设施引用
/// @return 错误码
errc_t _aLoadFacilityGfx(BKVParser& parser, Facility& facility)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "ShowLabel")){
                // @todo 设置显示标签
            }else if(aEqualsIgnoreCase(item.key(), "LabelFont")){
                // @todo 设置标签字体
            }else if(aEqualsIgnoreCase(item.key(), "LabelSize")){
                // @todo 设置标签大小
            }else if(aEqualsIgnoreCase(item.key(), "LabelColor")){
                // @todo 设置标签颜色
            }else if(aEqualsIgnoreCase(item.key(), "MarkerStyle")){
                // @todo 设置标记样式
            }else if(aEqualsIgnoreCase(item.key(), "MarkerSize")){
                // @todo 设置标记大小
            }else if(aEqualsIgnoreCase(item.key(), "MarkerColor")){
                // @todo 设置标记颜色
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Gfx")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载设施约束条件
/// @param parser BKV解析器
/// @param facility 设施引用
/// @return 错误码
errc_t _aLoadFacilityConstraints(BKVParser& parser, Facility& facility)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "MinElevation")){
                // @todo 设置最小仰角
            }else if(aEqualsIgnoreCase(item.key(), "MaxRange")){
                // @todo 设置最大距离
            }else if(aEqualsIgnoreCase(item.key(), "MinRange")){
                // @todo 设置最小距离
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Constraints")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载设施扩展属性
/// @param parser BKV解析器
/// @param facility 设施引用
/// @return 错误码
errc_t _aLoadFacilityExtensions(BKVParser& parser, Facility& facility)
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

errc_t aLoadFacility(BKVParser &parser, StringView objectType, Facility &facility)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Name")){
                facility.setName(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "CentroidPosition")){
                if(errc_t rc = _aLoadFacilityCentroidPosition(parser, facility)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Gfx")){
                if(errc_t rc = _aLoadFacilityGfx(parser, facility)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Constraints")){
                if(errc_t rc = _aLoadFacilityConstraints(parser, facility)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Extensions")){
                if(errc_t rc = _aLoadFacilityExtensions(parser, facility)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SubObjects")){
                if(errc_t rc = _aLoadSubObjects(parser, &facility)){
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

/// @brief 加载设施对象
/// @param parser BKV解析器
/// @param facility 设施引用
/// @return 错误码
errc_t aLoadFacility(BKVParser& parser, Facility& facility)
{
    return aLoadFacility(parser, "Facility", facility);
}

errc_t aLoadFacility(StringView filepath, StringView objectType, Facility& facility)
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
            // 第一个BEGIN END块的名称就是对象的类型
            StringView objectType = item.value();
            return aLoadFacility(parser, objectType, facility);
        }
    }while(token != BKVParser::eEOF);
    
    return eNoError;
}


errc_t aLoadFacility(StringView filepath, Facility& facility)
{
    return aLoadFacility(filepath, "Facility", facility);
}

AST_NAMESPACE_END