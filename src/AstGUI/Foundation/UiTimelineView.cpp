///
/// @file      UiTimelineView.cpp
/// @brief     任务分析时间轴视图控件实现
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

#include "UiTimelineView.hpp"
#include "AstCore/DateTime.hpp"

#include <QFont>
#include <QPainter>
#include <QPaintEvent>
#include <QRectF>
#include <QString>

AST_NAMESPACE_BEGIN

namespace {
const qreal kLeftWidth = 170.0;   ///< 左侧时刻标签列宽度
const qreal kRulerHeight = 26.0;  ///< 顶端小时刻度轴高度
const qreal kBarHeight = 22.0;    ///< 单条区间条高度
} // namespace

UiTimelineView::UiTimelineView(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(300, 90);
}

UiTimelineView::~UiTimelineView() = default;

void UiTimelineView::setTimeSpan(const TimeInterval& span)
{
    start_ = span.start();
    stop_  = span.stop();
    update();
}

void UiTimelineView::setCurrentTime(const TimePoint& current)
{
    current_ = current;
    update();
}

void UiTimelineView::setBars(const std::vector<Bar>& bars)
{
    bars_ = bars;
    update();
}

qreal UiTimelineView::xFor(const TimePoint& t) const
{
    const double span = stop_.durationFrom(start_);
    if (span <= 0.0)
        return kLeftWidth;
    const double frac = t.durationFrom(start_) / span;
    return kLeftWidth + frac * (width() - kLeftWidth);
}

void UiTimelineView::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal w = width();
    const qreal h = height();
    if (w <= 0 || h <= 0)
        return;

    // --- 背景 ---
    painter.fillRect(rect(), QColor(0xF2, 0xF2, 0xF2));
    painter.fillRect(QRectF(0, 0, kLeftWidth, h), QColor(0xE8, 0xE8, 0xE8));

    // --- 左侧当前时刻标签（格式 "07 Sep 2026 04:00:00.000"）---
    DateTime dttmCur;
    aTimePointToUTC(current_, dttmCur);
    std::string curStr;
    aDateTimeFormat(dttmCur, "%d %b %Y %H:%M:%S.%L", curStr);

    QFont font = painter.font();
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor(0x10, 0x10, 0x10));
    painter.drawText(QRectF(4, 2, kLeftWidth - 8, kRulerHeight - 4),
                     Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(curStr));

    // 分隔线
    painter.setPen(QColor(0xBB, 0xBB, 0xBB));
    painter.drawLine(QPointF(kLeftWidth, 0), QPointF(kLeftWidth, h));
    painter.drawLine(QPointF(0, kRulerHeight), QPointF(w, kRulerHeight));

    const double span = stop_.durationFrom(start_);
    if (span <= 0.0)
        return;

    // --- 小时刻度轴 ---
    DateTime dttm;
    aTimePointToUTC(start_, dttm);
    dttm.setMinute(0);
    dttm.setSecond(0.0);
    TimePoint tickTime = TimePoint::FromUTC(dttm);
    // 对齐到首个 >= start 的整点
    if (tickTime.durationFrom(start_) < 0.0)
    {
        aDateTimeAddHours(dttm, 1);
        tickTime = TimePoint::FromUTC(dttm);
    }

    int tickIndex = 0;
    while (tickTime.durationFrom(stop_) <= 0.0)
    {
        const qreal x = xFor(tickTime);

        // 竖网格线
        painter.setPen(QColor(0xCC, 0xCC, 0xCC));
        painter.drawLine(QPointF(x, kRulerHeight), QPointF(x, h));

        // 小时标签，蓝/红交替
        const QColor tickColor = (tickIndex % 2 == 0)
            ? QColor(0x30, 0x52, 0xD4)
            : QColor(0xD0, 0x30, 0x30);
        const QString label =
            QString("%1:%2")
                .arg(dttm.hour(), 2, 10, QChar('0'))
                .arg(dttm.minute(), 2, 10, QChar('0'));
        painter.setPen(tickColor);
        painter.drawText(QRectF(x - 22, 2, 44, kRulerHeight - 8), Qt::AlignCenter, label);

        aDateTimeAddHours(dttm, 1);
        tickTime = TimePoint::FromUTC(dttm);
        ++tickIndex;
    }

    // --- 彩色区间条 ---
    qreal y = kRulerHeight + 2;
    for (const Bar& bar : bars_)
    {
        const qreal x0 = xFor(bar.start);
        const qreal x1 = xFor(bar.stop);
        const qreal barWidth = (x1 - x0) > 0.0 ? (x1 - x0) : 0.0;
        QRectF rect(x0, y, barWidth, kBarHeight - 2);
        painter.fillRect(rect, bar.color);
        painter.setPen(Qt::black);
        painter.drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, bar.label);
        y += kBarHeight;
    }
}

AST_NAMESPACE_END
