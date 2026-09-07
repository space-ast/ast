///
/// @file      UiTimelineView.hpp
/// @brief     任务分析时间轴视图控件
/// @details   自绘小时刻度轴 + 彩色区间条，数据来自
///            TimePoint/TimeInterval，用 QPainter 按
///            (t - start)/(stop - start) 归一化映射到像素。
/// @author    axel
/// @date      2026-09-07
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
#include "AstCore/TimePoint.hpp"
#include "AstCore/TimeInterval.hpp"
#include <QWidget>
#include <QColor>
#include <QString>
#include <vector>

AST_NAMESPACE_BEGIN

/// @brief 时间轴视图控件
///
/// 布局：
/// - 底部时间轴视口：顶端为小时刻度轴，刻度线正下方为彩色区间条，
///   左侧固定一列显示当前时刻（格式 "07 Sep 2026 04:00:00.000"）。
/// - 通过 setTimeSpan / setCurrentTime / setBars 填充数据。
class AST_GUI_API UiTimelineView : public QWidget
{
    Q_OBJECT

public:
    /// @brief 时间轴区间条
    struct Bar
    {
        TimePoint start;    ///< 区间开始时间
        TimePoint stop;     ///< 区间结束时间
        QString   label;    ///< 区间名称
        QColor    color;    ///< 区间填充颜色
    };

public:
    explicit UiTimelineView(QWidget* parent = nullptr);
    ~UiTimelineView() override;

    UiTimelineView(const UiTimelineView&) = delete;
    UiTimelineView& operator=(const UiTimelineView&) = delete;

    /// @brief 设置时间轴整体显示区间
    void setTimeSpan(const TimeInterval& span);

    /// @brief 设置当前时刻（用于左侧时间标签）
    void setCurrentTime(const TimePoint& current);

    /// @brief 设置彩色区间条
    void setBars(const std::vector<Bar>& bars);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    /// @brief 将时间点映射到视口水平像素坐标
    qreal xFor(const TimePoint& t) const;

    TimePoint   start_{TimePoint::Epoch()};      ///< 时间轴起点
    TimePoint   stop_{TimePoint::Epoch()};       ///< 时间轴终点
    TimePoint   current_{TimePoint::Epoch()};    ///< 当前时刻
    std::vector<Bar> bars_{};                    ///< 区间条
};

AST_NAMESPACE_END
