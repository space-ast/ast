///
/// @file      ConstellationLoader.cpp
/// @brief     星座加载器实现
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

#include "ConstellationLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/Constellation.hpp"

AST_NAMESPACE_BEGIN

errc_t _aLoadDefinition(BKVParser& parser, Constellation& constellation)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Type")){
                // 处理类型
            }else if(aEqualsIgnoreCase(item.key(), "FromOperator")){
                // 处理FromOperator
            }else if(aEqualsIgnoreCase(item.key(), "FromOrder")){
                // 处理FromOrder
            }else if(aEqualsIgnoreCase(item.key(), "ToOperator")){
                // 处理ToOperator
            }else if(aEqualsIgnoreCase(item.key(), "ToOrder")){
                // 处理ToOrder
            }else if(aEqualsIgnoreCase(item.key(), "ToParentConstraint")){
                // 处理ToParentConstraint
            }else if(aEqualsIgnoreCase(item.key(), "FromParentConstraint")){
                // 处理FromParentConstraint
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Definition")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadExtensions(BKVParser& parser, Constellation& constellation)
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
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t aLoadConstellation(StringView filepath, Constellation& constellation)
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
            if(aEqualsIgnoreCase(item.value(), "Constellation")){
                BKVItemView constellationItem;
                BKVParser::EToken constellationToken;
                do{
                    constellationToken = parser.getNext(constellationItem);
                    if(constellationToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(constellationItem.key(), "Name")){
                            constellation.setName(constellationItem.value());
                        }
                    }else if(constellationToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(constellationItem.value(), "Definition")){
                            if(errc_t rc = _aLoadDefinition(parser, constellation)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(constellationItem.value(), "Extensions")){
                            if(errc_t rc = _aLoadExtensions(parser, constellation)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(constellationItem.value(), "SubObjects")){
                            if(errc_t rc = _aLoadSubObjects(parser, &constellation)){
                                return rc;
                            }
                        }
                    }else if(constellationToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(constellationItem.value(), "Constellation")){
                            return eNoError;
                        }
                    }
                }while(constellationToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END