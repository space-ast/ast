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
#include "AstCore/BlockDerivative.hpp"
#include <unordered_map>

AST_NAMESPACE_BEGIN

class BlockDerivative;

/// @brief 动力学系统
/// @details 
/// 动力学系统是一个包含普通函数块和导数函数块的子系统
/// 每个导数块负责计算动力学系统的一个或多个状态变量的导数。
/// 每个函数块通过输入输出端口进行信号连接，组合形成复杂的动力学系统。
/// 
/// 动力学系统将根据端口名称自动建立起函数块间的信号连接关系
/// 并构建状态量和导数量到函数块信号的映射关系
/// 最终得到一个常微分方程组，用于数值积分求解
class AST_CORE_API BlockDynamicSystem : public BlockSystem
{
public:
    BlockDynamicSystem() = default;
    ~BlockDynamicSystem() = default;

    /// @brief 获取状态量大小
    /// @return 状态量大小
    size_t size() const { return state_.size(); }

    /// @brief 获取状态量维度
    /// @return 状态量维度
    int getDimension() const { return static_cast<int>(this->size()); }

    /// @brief 获取状态量数据指针
    /// @return 状态量数据指针
    const double* getStateData() const { return state_.data(); }

    /// @brief 获取状态量导数数据指针
    /// @return 状态量导数数据指针
    const double* getDerivativeData() const { return derivative_.data(); }

    /// @brief 填充状态量数据
    /// @param y 状态量数据指针
    void setStateData(const double* y) { std::copy(y, y + this->size(), state_.begin()); }

    /// @brief 获取状态量导数数据指针
    /// @param dydt 状态量导数数据指针
    void getDerivativeData(double* dydt) const { std::copy(derivative_.begin(), derivative_.end(), dydt); }

    /// @brief 填充状态量导数数据
    /// @param value 状态量导数值
    void fillDerivativeData(double value) { std::fill(derivative_.begin(), derivative_.end(), value); }

    /// @brief 初始化
    err_t initialize();

    /// @brief 根据函数块的依赖关系，对子系统块中的函数块进行排序
    err_t sortBlocks();

    /// @brief 创建状态量映射表
    err_t createStateMap();

    /// @brief 根据函数块的信号名称，自动连接子系统块中的信号
    err_t connectSignalsByNames();

    /// @brief 添加状态量函数块
    /// @param block 状态量函数块指针
    void addBlock(BlockDerivative* block);

    /// @brief 添加状态量导数函数块
    /// @param block 状态量导数函数块指针
    void addDerivativeBlock(BlockDerivative* block);

    /// @brief 添加函数块
    /// @param block 函数块指针
    using BlockSystem::addBlock;

    /// @brief 清除所有函数块
    void clearBlocks();

    /// @brief 重置动力学系统
    void reset();

protected:
    using StateMap = std::unordered_map<Identifier*, double*>;

    std::vector<BlockDerivative*>   derivativeBlocks_;  // 状态量导数函数块
    std::vector<double>             state_;             // 状态量
    std::vector<double>             derivative_;        // 状态量导数
    StateMap                        stateMap_;          // 状态量映射表
    StateMap                        derivativeMap_;     // 状态量导数映射表
};


AST_NAMESPACE_END
