///
/// @file      ChainLoader.cpp
/// @brief     链加载器实现
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

#include "ChainLoader.hpp"
#include "AstSim/Chain.hpp"
#include "AstLoader/BasicComponentLoader.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/BKVParser.hpp"

AST_NAMESPACE_BEGIN

/// @brief 加载定义设置
/// @param parser BKV解析器
/// @param chain 链对象引用
/// @return 错误码
errc_t _aLoadDefinition(BKVParser& parser, Chain& chain)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Type")){
                // 处理 Type
            }else if(aEqualsIgnoreCase(item.key(), "FromOperator")){
                // 处理 FromOperator
            }else if(aEqualsIgnoreCase(item.key(), "FromOrder")){
                // 处理 FromOrder
            }else if(aEqualsIgnoreCase(item.key(), "ToOperator")){
                // 处理 ToOperator
            }else if(aEqualsIgnoreCase(item.key(), "ToOrder")){
                // 处理 ToOrder
            }else if(aEqualsIgnoreCase(item.key(), "Recompute")){
                // 处理 Recompute
            }else if(aEqualsIgnoreCase(item.key(), "IntervalType")){
                // 处理 IntervalType
            }else if(aEqualsIgnoreCase(item.key(), "ComputeIntervalStart")){
                // 处理 ComputeIntervalStart
            }else if(aEqualsIgnoreCase(item.key(), "ComputeIntervalStop")){
                // 处理 ComputeIntervalStop
            }else if(aEqualsIgnoreCase(item.key(), "ComputeIntervalPtr")){
                // 处理 ComputeIntervalPtr
            }else if(aEqualsIgnoreCase(item.key(), "ConstraintsUseStrandTimes")){
                // 处理 ConstraintsUseStrandTimes
            }else if(aEqualsIgnoreCase(item.key(), "UseSaveIntervalFile")){
                // 处理 UseSaveIntervalFile
            }else if(aEqualsIgnoreCase(item.key(), "SaveIntervalFile")){
                // 处理 SaveIntervalFile
            }else if(aEqualsIgnoreCase(item.key(), "UseMinAngle")){
                // 处理 UseMinAngle
            }else if(aEqualsIgnoreCase(item.key(), "UseMaxAngle")){
                // 处理 UseMaxAngle
            }else if(aEqualsIgnoreCase(item.key(), "UseMinLinkTime")){
                // 处理 UseMinLinkTime
            }else if(aEqualsIgnoreCase(item.key(), "LTDelayCriterion")){
                // 处理 LTDelayCriterion
            }else if(aEqualsIgnoreCase(item.key(), "TimeConvergence")){
                // 处理 TimeConvergence
            }else if(aEqualsIgnoreCase(item.key(), "AbsValueConvergence")){
                // 处理 AbsValueConvergence
            }else if(aEqualsIgnoreCase(item.key(), "RelValueConvergence")){
                // 处理 RelValueConvergence
            }else if(aEqualsIgnoreCase(item.key(), "MaxTimeStep")){
                // 处理 MaxTimeStep
            }else if(aEqualsIgnoreCase(item.key(), "MinTimeStep")){
                // 处理 MinTimeStep
            }else if(aEqualsIgnoreCase(item.key(), "UseLightTimeDelay")){
                // 处理 UseLightTimeDelay
            }else if(aEqualsIgnoreCase(item.key(), "DetectEventsUsingSamplesOnly")){
                // 处理 DetectEventsUsingSamplesOnly
            }else if(aEqualsIgnoreCase(item.key(), "UseLoadIntervalFile")){
                // 处理 UseLoadIntervalFile
            }else if(aEqualsIgnoreCase(item.key(), "SaveMode")){
                // 处理 SaveMode
            }else if(aEqualsIgnoreCase(item.key(), "IntervalState")){
                // 处理 IntervalState
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "EVENTINTERVAL")){
                // 处理 EVENTINTERVAL 块
                BKVItemView eventItem;
                BKVParser::EToken eventToken;
                do{
                    eventToken = parser.getNext(eventItem);
                    if(eventToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(eventItem.value(), "Interval")){
                            // 处理 Interval 块
                            BKVItemView intervalItem;
                            BKVParser::EToken intervalToken;
                            do{
                                intervalToken = parser.getNext(intervalItem);
                                if(intervalToken == BKVParser::eKeyValue){
                                    if(aEqualsIgnoreCase(intervalItem.key(), "Start")){
                                        // 处理 Start
                                    }else if(aEqualsIgnoreCase(intervalItem.key(), "Stop")){
                                        // 处理 Stop
                                    }
                                }
                            }while(intervalToken != BKVParser::eBlockEnd);
                        }
                    }
                }while(eventToken != BKVParser::eBlockEnd);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Definition")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载扩展设置
/// @param parser BKV解析器
/// @param chain 链对象引用
/// @return 错误码
errc_t _aLoadExtensions(BKVParser& parser, Chain& chain)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "ExternData")){
                // 处理 ExternData 块
                BKVItemView externItem;
                BKVParser::EToken externToken;
                do{
                    externToken = parser.getNext(externItem);
                }while(externToken != BKVParser::eBlockEnd);
            }else if(aEqualsIgnoreCase(item.value(), "ADFFileData")){
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
            }else if(aEqualsIgnoreCase(item.value(), "Crdn")){
                // 处理 Crdn 块
                BKVItemView crdnItem;
                BKVParser::EToken crdnToken;
                do{
                    crdnToken = parser.getNext(crdnItem);
                }while(crdnToken != BKVParser::eBlockEnd);
            }else if(aEqualsIgnoreCase(item.value(), "Graphics")){
                // 处理 Graphics 块
                BKVItemView graphicsItem;
                BKVParser::EToken graphicsToken;
                do{
                    graphicsToken = parser.getNext(graphicsItem);
                    if(graphicsToken == BKVParser::eBlockBegin){
                        if(aEqualsIgnoreCase(graphicsItem.value(), "Attributes")){
                            // 处理 Attributes 块
                            BKVItemView attrItem;
                            BKVParser::EToken attrToken;
                            do{
                                attrToken = parser.getNext(attrItem);
                                if(attrToken == BKVParser::eKeyValue){
                                    if(aEqualsIgnoreCase(attrItem.key(), "StaticColor")){
                                        // 处理 StaticColor
                                    }else if(aEqualsIgnoreCase(attrItem.key(), "AnimationColor")){
                                        // 处理 AnimationColor
                                    }else if(aEqualsIgnoreCase(attrItem.key(), "AnimationLineWidth")){
                                        // 处理 AnimationLineWidth
                                    }else if(aEqualsIgnoreCase(attrItem.key(), "StaticLineWidth")){
                                        // 处理 StaticLineWidth
                                    }
                                }
                            }while(attrToken != BKVParser::eBlockEnd);
                        }else if(aEqualsIgnoreCase(graphicsItem.value(), "Graphics")){
                            // 处理 Graphics 块
                            BKVItemView subGraphicsItem;
                            BKVParser::EToken subGraphicsToken;
                            do{
                                subGraphicsToken = parser.getNext(subGraphicsItem);
                                if(subGraphicsToken == BKVParser::eKeyValue){
                                    if(aEqualsIgnoreCase(subGraphicsItem.key(), "ShowGfx")){
                                        // 处理 ShowGfx
                                    }else if(aEqualsIgnoreCase(subGraphicsItem.key(), "ShowStatic")){
                                        // 处理 ShowStatic
                                    }else if(aEqualsIgnoreCase(subGraphicsItem.key(), "ShowAnimationHighlight")){
                                        // 处理 ShowAnimationHighlight
                                    }else if(aEqualsIgnoreCase(subGraphicsItem.key(), "ShowAnimationLine")){
                                        // 处理 ShowAnimationLine
                                    }else if(aEqualsIgnoreCase(subGraphicsItem.key(), "ShowLinkDirection")){
                                        // 处理 ShowLinkDirection
                                    }
                                }
                            }while(subGraphicsToken != BKVParser::eBlockEnd);
                        }
                    }
                }while(graphicsToken != BKVParser::eBlockEnd);
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

/// @brief 加载链对象
/// @param parser BKV解析器
/// @param objectType 对象类型
/// @param chain 链对象引用
/// @return 错误码
errc_t aLoadChain(BKVParser &parser, StringView objectType, Chain &chain)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Name")){
                chain.setName(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Definition")){
                if(errc_t rc = _aLoadDefinition(parser, chain)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Extensions")){
                if(errc_t rc = _aLoadExtensions(parser, chain)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SubObjects")){
                if(errc_t rc = _aLoadSubObjects(parser, &chain)){
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

/// @brief 加载链对象
/// @param parser BKV解析器
/// @param chain 链对象引用
/// @return 错误码
errc_t aLoadChain(BKVParser& parser, Chain& chain)
{
    return aLoadChain(parser, "Chain", chain);
}

/// @brief 加载链对象
/// @param filepath 文件路径
/// @param objectType 对象类型
/// @param chain 链对象引用
/// @return 错误码
errc_t aLoadChain(StringView filepath, StringView objectType, Chain& chain)
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
            return aLoadChain(parser, objectType, chain);
        }
    }while(token != BKVParser::eEOF);
    
    return eNoError;
}

/// @brief 加载链对象
/// @param filepath 文件路径
/// @param chain 链对象引用
/// @return 错误码
errc_t aLoadChain(StringView filepath, Chain& chain)
{
    return aLoadChain(filepath, "Chain", chain);
}

AST_NAMESPACE_END