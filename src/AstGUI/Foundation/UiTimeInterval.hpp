///
/// @file      UiTimeInterval.hpp
/// @brief     ~
/// @details   ~
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
#include "AstCore/TimeInterval.hpp"
#include "UiTimePoint.hpp"
#include <QWidget>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup GUI
    @{
*/

/// @brief 时间区间编辑控件
/// @details 用于编辑和显示时间区间，由两个时间点编辑控件组成
class AST_GUI_API UiTimeInterval : public QWidget
{
    Q_OBJECT
public:
    explicit UiTimeInterval(QWidget* parent = nullptr);
    ~UiTimeInterval() = default;
    UiTimeInterval(const UiTimeInterval&) = delete;
    UiTimeInterval& operator=(const UiTimeInterval&) = delete;
    
    /// @brief 设置时间区间
    /// @param timeInterval 时间区间
    void setTimeInterval(const TimeInterval& timeInterval);
    
    /// @brief 获取时间区间
    /// @return 时间区间
    const TimeInterval& getTimeInterval() const { return timeInterval_; }
signals:
    void timeIntervalChanged(const TimeInterval& timeInterval);
private:
    void updateTimeInterval();
private:
    UiTimePoint* startTimeEdit_{nullptr};
    UiTimePoint* stopTimeEdit_{nullptr};
    TimeInterval timeInterval_{};
};

/*! @} */

AST_NAMESPACE_END
