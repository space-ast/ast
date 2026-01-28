///
/// @file      ODEEventWatcher.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-27
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
#include "AstMath/ODEStateObserver.hpp"
#include "AstMath/ODEEventDetector.hpp"
#include <limits>

AST_NAMESPACE_BEGIN


class ODEEventObserver: public ODEStateObserver
{
public:
    ODEEventObserver() = default;
    explicit ODEEventObserver(ODEEventDetector* detector);
    ~ODEEventObserver() override;
    EODEAction onStateUpdate(double* y, double& x, ODEIntegrator* integrator) final;
    ODEEventDetector* getEventDetector() const { return detector_; }
protected:
    ODEEventDetector* detector_ {nullptr};
    double lastDifference_ {std::numeric_limits<double>::quiet_NaN()};
    double lastTime_{std::numeric_limits<double>::quiet_NaN()};
    int    repeatCount_{0};
};

AST_NAMESPACE_END
