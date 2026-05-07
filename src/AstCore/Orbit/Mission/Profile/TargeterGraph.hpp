///
/// @file      TargeterGraph.hpp
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

class TargeterGraphControlOptions;
class TargeterGraphResultOptions;

/// @brief 目标器图形配置
class AST_CORE_API TargeterGraph : public ObjectNamed
{
public:

    TargeterGraph() = default;
    ~TargeterGraph() override = default;

    bool graphWhileRunning_ = false;
    std::string independentVariableSaveName_;
    bool labelIterations_ = false;
    bool showDesired_ = false;
    bool showTolerance_ = false;

    std::vector<SharedPtr<TargeterGraphControlOptions>> controlOptions_;
    std::vector<SharedPtr<TargeterGraphResultOptions>> resultOptions_;
};

/// @brief 目标器图形控制选项
class AST_CORE_API TargeterGraphControlOptions : public ObjectNamed
{
public:

    TargeterGraphControlOptions() = default;
    ~TargeterGraphControlOptions() override = default;

    int graphLineColor_ = 0;
    std::string graphPointStyle_;
    bool graphValue_ = false;
    std::string graphYAxis_;
};

/// @brief 目标器图形结果选项
class AST_CORE_API TargeterGraphResultOptions : public ObjectNamed
{
public:

    TargeterGraphResultOptions() = default;
    ~TargeterGraphResultOptions() override = default;

    bool graphDesired_ = false;
    int graphLineColor_ = 0;
    std::string graphOption_;
    std::string graphPointStyle_;
    bool graphTolerance_ = false;
    std::string graphYAxis_;
};

AST_NAMESPACE_END