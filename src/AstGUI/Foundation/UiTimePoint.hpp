///
/// @file      UiTimePoint.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-28
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
#include "UiValueEdit.hpp"
#include <QLineEdit>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_GUI_API UiTimePoint : public UiValueEdit
{
    Q_OBJECT
public:
    explicit UiTimePoint(QWidget* parent = nullptr);
    ~UiTimePoint() = default;
    const TimePoint& getTimePoint() const { return timePoint_; }
    void setTimePoint(const TimePoint& timePoint);
signals:
    void timePointChanged(const TimePoint& timePoint);
private:
    void updateTimePoint();
private:
    TimePoint timePoint_{};
};

/*! @} */

AST_NAMESPACE_END