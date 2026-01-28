///
/// @file      ODEEventsHandler.hpp
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
#include "AstMath/ODEEventObserver.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

class ODEEventObserver;

class ODEEventDetectorList: public ODEStateObserver
{
public:
    ~ODEEventDetectorList() override;
    ODEEventDetectorList() = default;
    
    EODEAction onStateUpdate(double* y, double& x, ODEIntegrator* integrator) final;

    void addEventDetector(ODEEventDetector* detector);

    void removeEventDetector(ODEEventDetector* detector);

    bool empty() const { return eventObservers_.empty(); }

    size_t size() const { return eventObservers_.size(); }

    ODEEventObserver& operator[](size_t index) { return eventObservers_[index]; }
    const ODEEventObserver& operator[](size_t index) const { return eventObservers_[index]; }
protected:
    std::vector<ODEEventObserver> eventObservers_;
};


AST_NAMESPACE_END
