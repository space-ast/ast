///
/// @file      TargeterSequence.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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

#pragma once

#include "AstGlobal.h"
#include "Sequence.hpp"
#include "AstCore/TargeterProfile.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 目标序列段，用于建模并求解轨迹规划问题，可以定义多个控制变量、约束条件、目标函数等，调用求解器求解满足约束和最优的轨迹
class AST_CORE_API TargeterSequence : public Sequence
{
public:
    AST_OBJECT(TargeterSequence)

    TargeterSequence() = default;
    ~TargeterSequence() = default;

    /// @brief 设置目标序列段的目标配置
    /// @param profiles 目标配置列表
    void setProfiles(const std::vector<HTargeterProfile>& profiles){profiles_ = profiles;}

    TargeterProfile* getTargeterProfile(StringView name) const;

    
    errc_t execute() override;

private:
    std::vector<HTargeterProfile> profiles_;
};




/*! @} */

AST_NAMESPACE_END
