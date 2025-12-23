///
/// @file      OpUnaryRegister.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-22
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstScript/ScriptAPI.hpp"
#include <tuple>
#include <unordered_map>

AST_NAMESPACE_BEGIN

class OpUnaryRegister
{
public:
//  @brief 运算函数注册表键类型
    typedef std::tuple<int, Class*> OpUnaryKey;

    //  @brief 运算函数注册表哈希函数
    struct OpUnaryKeyHash {
        std::size_t operator()(const OpUnaryKey& key) const {
            // 组合哈希值
            std::size_t h1 = std::hash<int>()(static_cast<int>(std::get<0>(key)));
            std::size_t h2 = std::hash<const void*>()(std::get<1>(key));
            // 使用位运算组合哈希值
            return h1 ^ (h2 << 1);
        }
    };
    typedef std::unordered_map<OpUnaryKey, void*, OpUnaryKeyHash> OpUnaryMap;

public:
    OpUnaryRegister() = default;
    ~OpUnaryRegister() = default;
    void* getFunc(EOpUnaryType op, Class* type)
    {
        auto key = OpUnaryKey{static_cast<int>(op), type};
        auto it = map_.find(key);
        if(it == map_.end()){
            return nullptr;
        }
        return it->second;
    }
    void regFunc(EOpUnaryType op, Class* type, void* func)
    {
        map_[{static_cast<int>(op), type}] = func;
    }
protected:
    OpUnaryMap map_;
};

AST_NAMESPACE_END
