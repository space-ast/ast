///
/// @file      LineTargetLoader.cpp
/// @brief     线目标加载器实现
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

#include "LineTargetLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/LineTarget.hpp"

AST_NAMESPACE_BEGIN

errc_t _aLoadIndexPtData(BKVParser& parser, LineTarget& lineTarget)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "CentralBody")){
                // 处理中心天体
            }else if(aEqualsIgnoreCase(item.key(), "AnchorPtIndex")){
                // 处理锚点索引
            }else if(aEqualsIgnoreCase(item.key(), "BoundaryType")){
                // 处理边界类型
            }else if(aEqualsIgnoreCase(item.key(), "Granularity")){
                // 处理粒度
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "IndexPtData")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadExtensions(BKVParser& parser, LineTarget& lineTarget)
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

errc_t aLoadLineTarget(StringView filepath, LineTarget& lineTarget)
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
            if(aEqualsIgnoreCase(item.value(), "LineTarget")){
                BKVItemView ltItem;
                BKVParser::EToken ltToken;
                do{
                    ltToken = parser.getNext(ltItem);
                    if(ltToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(ltItem.key(), "Name")){
                            lineTarget.setName(ltItem.value());
                        }
                    }else if(ltToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(ltItem.value(), "IndexPtData")){
                            if(errc_t rc = _aLoadIndexPtData(parser, lineTarget)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(ltItem.value(), "Extensions")){
                            if(errc_t rc = _aLoadExtensions(parser, lineTarget)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(ltItem.value(), "SubObjects")){
                            if(errc_t rc = _aLoadSubObjects(parser, &lineTarget)){
                                return rc;
                            }
                        }
                    }else if(ltToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(ltItem.value(), "LineTarget")){
                            return eNoError;
                        }
                    }
                }while(ltToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END