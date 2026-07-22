///
/// @file      AccessStepper.hpp
/// @brief     访问评估步进策略抽象基类
/// @details   定义时间步进策略接口，支持固定步长、自适应步长等扩展。
/// @author    axel
/// @date      2026-07-22
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "AstCore/TimePoint.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/// @brief 步进策略抽象基类
/// @details 
/// 子类实现不同的时间采样策略：
/// - FixedStepStepper：固定步长
/// - 未来可扩展自适应步长、基于轨道特征的步长等
class AST_CORE_API AccessStepper : public ObjectNamed
{
public:
    AST_OBJECT(AccessStepper)

    AccessStepper() = default;
    virtual ~AccessStepper() = default;

    /// @brief 初始化时间范围
    /// @param start 开始时刻
    /// @param stop 结束时刻
    virtual void init(const TimePoint& start, const TimePoint& stop) = 0;

    /// @brief 获取下一个采样时刻
    /// @param tp 输出下一个采样时刻
    /// @return false 表示无更多采样点
    virtual bool next(TimePoint& tp) = 0;
};

AST_NAMESPACE_END
