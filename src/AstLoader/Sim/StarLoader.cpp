///
/// @file      StarLoader.cpp
/// @brief     恒星对象加载器实现
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

#include "StarLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/Star.hpp"

AST_NAMESPACE_BEGIN

errc_t aLoadStar(StringView filepath, Star& star)
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
                star.setName(item.value());
            }
        }
        else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Star")){
                BKVItemView starItem;
                BKVParser::EToken starToken;
                do{
                    starToken = parser.getNext(starItem);
                    if(starToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(starItem.key(), "Name")){
                            star.setName(starItem.value());
                        }
                    }else if(starToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(starItem.value(), "Star")){
                            return eNoError;
                        }
                    }
                }while(starToken != BKVParser::eEOF);
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END