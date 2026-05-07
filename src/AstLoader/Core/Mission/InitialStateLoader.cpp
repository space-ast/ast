///
/// @file      InitialStateLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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

#include "InitialStateLoader.hpp"
#include "SegmentLoader.hpp"
#include "SpacecraftStateLoader.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#
#include "AstUtil/XMLDocument.hpp"
#include "AstUtil/Archive.hpp"
#include "AstScript/Value.hpp"
#include "ValXMLLoader.hpp"

AST_NAMESPACE_BEGIN

/*
如何同时支持多种XML存储格式?
方案1：
通过实现Archive接口对对象属性进行抽象统一，
这样只要实现两套将XML节点转换为ar对象的调用逻辑即可，与特定对象相关的代码只有一份
但是不太好处理嵌套对象的情况
方案2：
将不同的XML数据转换为统一的抽象树状结构数据，
然后根据树状结构数据加载到对象中，这样与特定对象相关的代码也只有一份
目前工程可用的抽象树状结构数据有ValMap体系，好处是支持Quantity类型
或者针对json格式的数据，写一个json树状结构数据，好处是较为轻量
或者参考boost的property_tree，写一个通用的树状结构数据
XMLNode似乎不太合适用于这个场景
*/

#if 0
namespace{
// 方案1废弃
class InitialStateArchive: public Archive
{
public:
    InitialStateArchive(InitialState& initialState) : initialState_(initialState) {}
    ~InitialStateArchive() = default;
    Archive& operator()(bool& val, const char* name=nullptr)
    {
        return *this;
    }
    Archive& operator()(int& val, const char* name=nullptr)
    {
        return *this;
    }
    Archive& operator()(double& val, const char* name=nullptr)
    {
        return *this;
    }
    Archive& operator()(std::string& val, const char* name=nullptr)
    {
        return *this;
    }
private:
    InitialState& initialState_;
};
}
#endif

errc_t aLoadInitialState(const Value& dictRoot, InitialState& initialState)
{
    if(dictRoot["Type"].toString() != "InitialState")
    {
        aError("invalid type, expect 'InitialState'");
        return eErrorInvalidParam;
    }

    
    // 似乎不用判断"CoordinateType"字段，因为默认存的都是Cartesian坐标
    // auto& dictCoordinateType = dictRoot["CoordinateType"];
    // if(!dictCoordinateType.isNull())
    auto spacecraftStatePtr = initialState.getInitialState();
    if(spacecraftStatePtr)
    {
        auto& spacecraftState = *spacecraftStatePtr;
        aLoadSpacecraftState(dictRoot["InitialState"], spacecraftState);
    }

    // 加载公共属性
    aLoadSegment(dictRoot, initialState);
    return eNoError;
}


errc_t aLoadInitialState(StringView filePath, InitialState& initialState)
{
    errc_t rc;
    SharedPtr<Value> value;
    rc = aLoadValue(filePath, value);
    if(rc)  return rc;
    if(!value) return eErrorNullPtr;
    return aLoadInitialState(*value, initialState);
}

AST_NAMESPACE_END

