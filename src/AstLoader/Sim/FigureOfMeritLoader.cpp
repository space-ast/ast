///
/// @file      FigureOfMeritLoader.cpp
/// @brief     性能指标加载器实现
/// @author    axel
/// @date      2026-04-10
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

#include "FigureOfMeritLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/FigureOfMerit.hpp"

AST_NAMESPACE_BEGIN

errc_t _aLoadFomDefinition(BKVParser& parser, FigureOfMerit& figureOfMerit)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "SimpleCoverage")){
                // 处理SimpleCoverage
                while(1){
                    BKVItemView simpleItem;
                    BKVParser::EToken simpleToken;
                    simpleToken = parser.getNext(simpleItem);
                    if(simpleToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(simpleItem.value(), "SimpleCoverage")){
                            break;
                        }
                    }
                }
            }
        }else if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "ApplySatisfaction")){
                // 处理ApplySatisfaction
            }else if(aEqualsIgnoreCase(item.key(), "SatisfactionType")){
                // 处理SatisfactionType
            }else if(aEqualsIgnoreCase(item.key(), "SatisfactionThreshold")){
                // 处理SatisfactionThreshold
            }else if(aEqualsIgnoreCase(item.key(), "NotExecutedValue")){
                // 处理NotExecutedValue
            }else if(aEqualsIgnoreCase(item.key(), "UseFOMValueRangeCheck")){
                // 处理UseFOMValueRangeCheck
            }else if(aEqualsIgnoreCase(item.key(), "ExcludeValuesInRange")){
                // 处理ExcludeValuesInRange
            }else if(aEqualsIgnoreCase(item.key(), "FOMValueRangeMin")){
                // 处理FOMValueRangeMin
            }else if(aEqualsIgnoreCase(item.key(), "FOMValueRangeMax")){
                // 处理FOMValueRangeMax
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "FomDefinition")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadExtensions(BKVParser& parser, FigureOfMerit& figureOfMerit)
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

errc_t aLoadFigureOfMerit(StringView filepath, FigureOfMerit& figureOfMerit)
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
                figureOfMerit.setName(item.value());
            }
        }
        else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "FigureOfMerit")){
                BKVItemView fomItem;
                BKVParser::EToken fomToken;
                do{
                    fomToken = parser.getNext(fomItem);
                    if(fomToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(fomItem.key(), "Name")){
                            figureOfMerit.setName(fomItem.value());
                        }
                    }else if(fomToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(fomItem.value(), "FomDefinition")){
                            if(errc_t rc = _aLoadFomDefinition(parser, figureOfMerit)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(fomItem.value(), "Extensions")){
                            if(errc_t rc = _aLoadExtensions(parser, figureOfMerit)){
                                return rc;
                            }
                        }else if(aEqualsIgnoreCase(fomItem.value(), "SubObjects")){
                            if(errc_t rc = _aLoadSubObjects(parser, &figureOfMerit)){
                                return rc;
                            }
                        }
                    }else if(fomToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(fomItem.value(), "FigureOfMerit")){
                            return eNoError;
                        }
                    }
                }while(fomToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END