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
#include "AstCore/SimTime.hpp"
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
/// 
/// 动力学系统是 HPOP 的核心执行引擎，管理一组函数块并自动将其组装为常微分方程组。
///
/// 初始化过程（initialize）：
///   1. sortBlocks() — 按依赖关系对函数块进行拓扑排序，确保被依赖的块先执行
///   2. createStateMap() — 扫描所有块的输出/导数端口，构建状态量映射表，分配连续内存
///   3. connectSignalsByNames() — 按端口名称自动连接信号，建立块间的数据流
///
/// 执行过程（run → evaluate）：
///   1. 将外部状态向量 y 写入各块的状态信号指针
///   2. 清零导数缓冲区
///   3. 按序执行所有函数块的 run()
///   4. 从导数信号指针收集各块的导数贡献，输出 dy/dt
///
/// 状态量与导数量的对应关系由端口名称决定：
///   - 输出端口名为 "Pos" 的块提供位置状态量
///   - 导数端口名为 "Pos" 的块贡献位置导数
///   - 导数端口名为 "Vel" 的块贡献速度导数（即加速度）
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

    /// @brief 获取状态量索引
    /// @param id 状态量标识符指针
    /// @return 状态量索引，若未找到则返回 -1
    /// @note 状态量索引从 0 开始
    size_t getStateIndex(Identifier* id) const;
    size_t getStateIndex(StringView name) const;

    /// @brief 填充状态量数据
    /// @param y 状态量数据指针
    void setStateData(const double* y) { std::copy(y, y + this->size(), state_.begin()); }

    /// @brief 获取状态量导数数据指针
    /// @param dydt 状态量导数数据指针
    void getDerivativeData(double* dydt) const { std::copy(derivative_.begin(), derivative_.end(), dydt); }

    /// @brief 填充状态量导数数据
    /// @param value 状态量导数值
    void fillDerivativeData(double value) { std::fill(derivative_.begin(), derivative_.end(), value); }

    /// @brief 填充累加状态量数据
    /// @param value 累加状态量值
    void fillAccumulateData(double value) { std::fill(accumulate_.begin(), accumulate_.end(), value); }

    /// @brief 初始化
    errc_t initialize();

    /// @brief 根据函数块的依赖关系，对子系统块中的函数块进行排序
    errc_t sortBlocks();

    /// @brief 创建状态量映射表
    errc_t createStateMap();

    /// @brief 根据函数块的信号名称，自动连接子系统块中的信号
    errc_t connectSignalsByNames();

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

    /// @brief 执行动力学系统
    errc_t run(const SimTime& simTime) final;

protected:
    A_DISABLE_COPY(BlockDynamicSystem);
    using StateMap = std::unordered_map<Identifier*, double*>;

    std::vector<BlockDerivative*>   derivativeBlocks_;  // 状态量导数函数块
    std::vector<double>             state_;             // 状态量
    std::vector<double>             accumulate_;        // 累加状态量
    std::vector<double>             derivative_;        // 状态量导数
    StateMap                        stateMap_;          // 状态量映射表
    StateMap                        derivativeMap_;     // 状态量导数映射表
};


AST_NAMESPACE_END
