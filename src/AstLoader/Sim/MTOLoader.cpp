///
/// @file      MTOLoader.cpp
/// @brief     多目标加载器实现
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

#include "MTOLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/MTO.hpp"

AST_NAMESPACE_BEGIN

errc_t _aLoadGeneralTrackData(BKVParser& parser, MTO& mto)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "CentralBody")){
                // 处理中心天体
            }else if(aEqualsIgnoreCase(item.key(), "SaveTracks")){
                // 处理是否保存轨迹
            }else if(aEqualsIgnoreCase(item.key(), "IsStatic")){
                // 处理是否静态
            }else if(aEqualsIgnoreCase(item.key(), "StaticGeometry")){
                // 处理是否静态几何
            }else if(aEqualsIgnoreCase(item.key(), "BlockSize")){
                // 处理块大小
            }else if(aEqualsIgnoreCase(item.key(), "StoredDataType")){
                // 处理存储数据类型
            }else if(aEqualsIgnoreCase(item.key(), "Epoch")){
                // 处理历元
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "GeneralTrackData")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadDefaultTrackBin(BKVParser& parser, MTO& mto)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "DefaultTrackBin")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadExtensions(BKVParser& parser, MTO& mto)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "ADFFileData")){
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

errc_t aLoadMTO(StringView filepath, MTO& mto)
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
            if(aEqualsIgnoreCase(item.key(), "Name")){
                mto.setName(item.value());
            }
        }
        else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "MTO")){
                BKVItemView mtoItem;
                BKVParser::EToken mtoToken;
                do{
                    mtoToken = parser.getNext(mtoItem);
                    if(mtoToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(mtoItem.key(), "Name")){
                            mto.setName(mtoItem.value());
                        }
                    }else if(mtoToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(mtoItem.value(), "GeneralTrackData")){
                            if(errc_t rc = _aLoadGeneralTrackData(parser, mto)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(mtoItem.value(), "DefaultTrackBin")){
                            if(errc_t rc = _aLoadDefaultTrackBin(parser, mto)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(mtoItem.value(), "Extensions")){
                            if(errc_t rc = _aLoadExtensions(parser, mto)){
                                return rc;
                            }
                        }
                    }else if(mtoToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(mtoItem.value(), "MTO")){
                            return eNoError;
                        }
                    }
                }while(mtoToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END