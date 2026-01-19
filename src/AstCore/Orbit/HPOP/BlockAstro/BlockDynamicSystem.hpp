///
/// @file      BlockDynamicSystem.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
#include "BlockAstro.hpp"
#include "AstCore/BlockSystem.hpp"
#include <unordered_map>

AST_NAMESPACE_BEGIN

class AST_CORE_API BlockDynamicSystem : public BlockSystem
{
public:
    BlockDynamicSystem() = default;
    ~BlockDynamicSystem() = default;

    /// @brief 获取状态量大小
    /// @return 状态量大小
    size_t size() const { return state_.size(); }

    /// @brief 获取状态量数据指针
    /// @return 状态量数据指针
    const double* getStateData() const { return state_.data(); }

    /// @brief 获取状态量导数数据指针
    /// @return 状态量导数数据指针
    const double* getDerivativeData() const { return derivative_.data(); }

    /// @brief 根据函数块的依赖关系，对子系统块中的函数块进行排序
    void sortBlocks();

    /// @brief 根据函数块的信号名称，自动连接子系统块中的信号
    void connectSignalsByNames();
protected:
    std::vector<double> state_;         // 状态量
    std::vector<double> derivative_;    // 状态量导数
    std::unordered_map<Identifier*, double*> stateMap_;
};

AST_NAMESPACE_END
