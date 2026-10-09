///
/// @file      ObjectComponentLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-09
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

#include "ObjectComponentLoader.hpp"
#include "AstSim/ObjectAccessConstraints.hpp"
#include "AstSim/ObjectComponent.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/ValueView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include "AstLoader/BasicComponentLoader.hpp"

AST_NAMESPACE_BEGIN

/// @brief 加载单条访问约束的参数
/// @details 参数由空格分隔，支持可选的 "Min <值>"、"Max <值>"，以及末尾的 "IncludeIntervals"/"ExcludeIntervals"。
///          例如 "Min  0.0000000000e+00    Max  5.0000000000e+01 IncludeIntervals"
/// @param constraint 访问约束
/// @param value 参数值视图
/// @return 错误码
errc_t _aLoadAccessConstraintParams(ObjectAccessConstraint& constraint, ValueView value)
{
    auto tokens = value.toVector();
    for(size_t i = 0; i < tokens.size(); ++i)
    {
        StringView token = tokens[i].toStringView();
        if(aEqualsIgnoreCase(token, "Min")){
            if(i + 1 < tokens.size()){
                constraint.setMin(tokens[++i].toDouble());
                constraint.setUseMin(true);
            }else{
                aWarning(_("访问约束的 Min 缺少取值"));
            }
        }else if(aEqualsIgnoreCase(token, "Max")){
            if(i + 1 < tokens.size()){
                constraint.setMax(tokens[++i].toDouble());
                constraint.setUseMax(true);
            }else{
                aWarning(_("访问约束的 Max 缺少取值"));
            }
        }else if(aEqualsIgnoreCase(token, "IncludeIntervals")){
            constraint.setExclude(false);
        }else if(aEqualsIgnoreCase(token, "ExcludeIntervals")){
            constraint.setExclude(true);
        }else{
            aWarning(_("不支持的访问约束参数: %.*s"), (int)token.size(), token.data());
        }
    }
    return eNoError;
}

/// @brief 加载访问约束块
/// @details 块内每一行的键为约束类型，值为该约束的参数。
///          例如：
///          @code
///          BEGIN AccessConstraints
///              LineOfSight IncludeIntervals
///              ElevationAngle Min  0.0000000000e+00    Max  5.0000000000e+01 IncludeIntervals
///              Range Min  4.0000000000e+04 ExcludeIntervals
///          END AccessConstraints
///          @endcode
/// @param parser BKV解析器
/// @param constraints 访问约束容器
/// @return 错误码
errc_t _aLoadAccessConstraints(BKVParser& parser, ObjectAccessConstraints& constraints)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            StringView name = item.key();
            ObjectAccessConstraint* constraint = constraints.addConstraint(name);
            if(constraint == nullptr){
                aWarning(_("不支持的访问约束类型: %.*s"), (int)name.size(), name.data());
            }else{
                // 将约束设置为启用状态
                constraint->setEnabled(true);
                _aLoadAccessConstraintParams(*constraint, item.value());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "AccessConstraints")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t aLoadObjectExtensions(BKVParser &parser, Object &object)
{
    BKVItemView item;
    BKVParser::EToken token;

    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "AccessConstraints")){
                auto& constraints = aObject_EnsureAccessConstraints(object);
                if(errc_t rc = _aLoadAccessConstraints(parser, constraints))
                    return rc;
            }else if(aEqualsIgnoreCase(item.value(), "ExternData")){
                // @todo 处理外部数据
            }else if(aEqualsIgnoreCase(item.value(), "ADFFileData")){
                // @todo 处理ADF文件数据
            }else if(aEqualsIgnoreCase(item.value(), "AccessConstraints")){
                // @todo 处理访问约束
            }else if(aEqualsIgnoreCase(item.value(), "ObjectCoverage")){
                // @todo 处理对象覆盖
            }else if(aEqualsIgnoreCase(item.value(), "Desc")){
                // @todo 处理描述
            }else if(aEqualsIgnoreCase(item.value(), "Refraction")){
                // @todo 处理折射
            }else if(aEqualsIgnoreCase(item.value(), "Crdn")){
                // @todo 处理坐标系统
            }else if(aEqualsIgnoreCase(item.value(), "Graphics")){
                // @todo 处理图形
            }else if(aEqualsIgnoreCase(item.value(), "Swath")){
                // @todo 处理扫描带
            }else if(aEqualsIgnoreCase(item.value(), "VO")){
                // @todo 处理VO
            }else if(aEqualsIgnoreCase(item.value(), "DIS")){
                // @todo 处理DIS
            }
            else
            {
                _aSkipUnknownBlock(parser, item.value());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


AST_NAMESPACE_END
