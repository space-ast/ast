///
/// @file      CommSystemLoader.cpp
/// @brief     通信系统加载器实现
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

#include "CommSystemLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/CommSystem.hpp"

AST_NAMESPACE_BEGIN

errc_t _aLoadExtensions(BKVParser& parser, CommSystem& commSystem)
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
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t aLoadCommSystem(StringView filepath, CommSystem& commSystem)
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
                commSystem.setName(item.value());
            }
        }
        else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "CommSystem")){
                BKVItemView commSystemItem;
                BKVParser::EToken commSystemToken;
                do{
                    commSystemToken = parser.getNext(commSystemItem);
                    if(commSystemToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(commSystemItem.key(), "Name")){
                            commSystem.setName(commSystemItem.value());
                        }
                    }else if(commSystemToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(commSystemItem.value(), "Extensions")){
                            if(errc_t rc = _aLoadExtensions(parser, commSystem)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(commSystemItem.value(), "SubObjects")){
                            if(errc_t rc = _aLoadSubObjects(parser, &commSystem)){
                                return rc;
                            }
                        }
                    }else if(commSystemToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(commSystemItem.value(), "CommSystem")){
                            return eNoError;
                        }
                    }
                }while(commSystemToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END