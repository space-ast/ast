///
/// @file      ReceiverLoader.cpp
/// @brief     接收器加载器实现
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

#include "ReceiverLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/Receiver.hpp"

AST_NAMESPACE_BEGIN

errc_t _aLoadExtensions(BKVParser& parser, Receiver& receiver)
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
            }else if(aEqualsIgnoreCase(item.value(), "AccessConstraints")){
                // 处理AccessConstraints
                while(1){
                    BKVItemView constraintItem;
                    BKVParser::EToken constraintToken;
                    constraintToken = parser.getNext(constraintItem);
                    if(constraintToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(constraintItem.value(), "AccessConstraints")){
                            break;
                        }
                    }
                }
            }else if(aEqualsIgnoreCase(item.value(), "ObjectCoverage")){
                // 处理ObjectCoverage
                while(1){
                    BKVItemView coverageItem;
                    BKVParser::EToken coverageToken;
                    coverageToken = parser.getNext(coverageItem);
                    if(coverageToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(coverageItem.value(), "ObjectCoverage")){
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
            }else if(aEqualsIgnoreCase(item.value(), "Refraction")){
                // 处理Refraction
                while(1){
                    BKVItemView refractionItem;
                    BKVParser::EToken refractionToken;
                    refractionToken = parser.getNext(refractionItem);
                    if(refractionToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(refractionItem.value(), "Refraction")){
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
            }else if(aEqualsIgnoreCase(item.value(), "ContourGfx")){
                // 处理ContourGfx
                while(1){
                    BKVItemView contourItem;
                    BKVParser::EToken contourToken;
                    contourToken = parser.getNext(contourItem);
                    if(contourToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(contourItem.value(), "ContourGfx")){
                            break;
                        }
                    }
                }
            }else if(aEqualsIgnoreCase(item.value(), "Contours")){
                // 处理Contours
                while(1){
                    BKVItemView contourItem;
                    BKVParser::EToken contourToken;
                    contourToken = parser.getNext(contourItem);
                    if(contourToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(contourItem.value(), "Contours")){
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
            }else if(aEqualsIgnoreCase(item.value(), "3dVolume")){
                // 处理3dVolume
                while(1){
                    BKVItemView volumeItem;
                    BKVParser::EToken volumeToken;
                    volumeToken = parser.getNext(volumeItem);
                    if(volumeToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(volumeItem.value(), "3dVolume")){
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

errc_t aLoadReceiver(StringView filepath, Receiver& receiver)
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
                receiver.setName(item.value());
            }
        }
        else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Receiver")){
                BKVItemView receiverItem;
                BKVParser::EToken receiverToken;
                do{
                    receiverToken = parser.getNext(receiverItem);
                    if(receiverToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(receiverItem.key(), "Name")){
                            receiver.setName(receiverItem.value());
                        }
                    }else if(receiverToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(receiverItem.value(), "Extensions")){
                            if(errc_t rc = _aLoadExtensions(parser, receiver)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(receiverItem.value(), "SubObjects")){
                            if(errc_t rc = _aLoadSubObjects(parser, &receiver)){
                                return rc;
                            }
                        }
                    }else if(receiverToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(receiverItem.value(), "Receiver")){
                            return eNoError;
                        }
                    }
                }while(receiverToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END