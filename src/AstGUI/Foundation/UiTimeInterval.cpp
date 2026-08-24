///
/// @file      UiTimeInterval.cpp
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

#include "UiTimeInterval.hpp"
#include "AstCore/TimePoint.hpp"
#include <QVBoxLayout>
#include <QDebug>

AST_NAMESPACE_BEGIN

UiTimeInterval::UiTimeInterval(QWidget* parent) : QWidget(parent)
{
    // 创建开始时间编辑控件
    startTimeEdit_ = new UiTimePoint(this);
    
    // 创建结束时间编辑控件
    stopTimeEdit_ = new UiTimePoint(this);
    
    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(startTimeEdit_);
    layout->addWidget(stopTimeEdit_);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // 设置尺寸策略
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 设置默认时间区间
    TimePoint start{}; // = TimePoint::TodayUTC();
    TimePoint end{};  // = TimePoint::TomorrowUTC();
    TimeInterval interval(start, end);
    setTimeInterval(interval);

    // 连接信号和槽
    connect(startTimeEdit_, &UiTimePoint::timePointChanged, this, &UiTimeInterval::updateTimeInterval);
    connect(stopTimeEdit_,  &UiTimePoint::timePointChanged, this, &UiTimeInterval::updateTimeInterval);
}

void UiTimeInterval::setTimeInterval(const TimeInterval& timeInterval)
{
    timeInterval_ = timeInterval;
    
    // 设置开始时间和结束时间
    startTimeEdit_->setTimePoint(timeInterval.getStart());
    stopTimeEdit_->setTimePoint(timeInterval.getStop());
}

void UiTimeInterval::updateTimeInterval()
{
    // 获取开始时间和结束时间
    TimePoint start = startTimeEdit_->getTimePoint();
    TimePoint stop  =  stopTimeEdit_->getTimePoint();
    
    // 更新时间区间
    timeInterval_.setBounds(start, stop);
    emit timeIntervalChanged(timeInterval_);
}



AST_NAMESPACE_END