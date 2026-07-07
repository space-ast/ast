///
/// @file      EditFigureOverlay.cpp
/// @brief     EditFigureOverlay 实现
/// @author    axel
/// @date      2026-06-06
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#include "EditFigureOverlay.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

AST_NAMESPACE_BEGIN



namespace{
constexpr QwtFigureWidgetOverlay::ControlType Move = QwtFigureWidgetOverlay::Inner;             ///< 移动控制，和原来的Inner行为一致
constexpr QwtFigureWidgetOverlay::ControlType None = QwtFigureWidgetOverlay::ControlType(-1);   ///< 无控制类型，用于表示点击区域不在任何控制点上
}

/// @brief 中点感知的位置检测：四角保持原逻辑，边缘只在中点附近命中
QwtFigureWidgetOverlay::ControlType getMidpointControlType(const QPoint& pos, const QRect& region, int err = 4)
{
    if (!region.adjusted(-err, -err, err, err).contains(pos)) {
        return (QwtFigureWidgetOverlay::OutSide);
    }

    int cx = region.center().x();
    int cy = region.center().y();
    int midErr = err * 2;  // 边中点命中容差（比角点大，方便点击）

    // 四角（保持原逻辑）
    if (pos.x() < (region.left() + err)) {
        if (pos.y() < region.top() + err) {
            return (QwtFigureWidgetOverlay::ControlPointTopLeft);
        } else if (pos.y() > region.bottom() - err) {
            return (QwtFigureWidgetOverlay::ControlPointBottomLeft);
        }
    } else if (pos.x() > (region.right() - err)) {
        if (pos.y() < region.top() + err) {
            return (QwtFigureWidgetOverlay::ControlPointTopRight);
        } else if (pos.y() > region.bottom() - err) {
            return (QwtFigureWidgetOverlay::ControlPointBottomRight);
        }
    }

    // 四条边的中点 → 边缘控制（可拖拽缩放）
    if (std::abs(pos.x() - cx) < midErr) {
        if (pos.y() < (region.top() + err)) {
            return QwtFigureWidgetOverlay::ControlLineTop;
        }
        if (pos.y() > (region.bottom() - err)) {
            return QwtFigureWidgetOverlay::ControlLineBottom;
        }
    }
    if (std::abs(pos.y() - cy) < midErr) {
        if (pos.x() < (region.left() + err)) {
            return QwtFigureWidgetOverlay::ControlLineLeft;
        }
        if (pos.x() > (region.right() - err)) {
            return QwtFigureWidgetOverlay::ControlLineRight;
        }
    }

    // 四条边的其他位置（非中点非角落）→ Inner
    bool nearEdge = (pos.x() < (region.left() + err))
                 || (pos.x() > (region.right() - err))
                 || (pos.y() < (region.top() + err))
                 || (pos.y() > (region.bottom() - err));
    if (nearEdge) {
        return Move;
    }

    // 矩形中间 → OutSide
    return None;
}

void EditFigureOverlay::mousePressEvent(QMouseEvent* me)
{
    if (me->button() != Qt::LeftButton) {
        QwtWidgetOverlay::mousePressEvent(me);
        return;
    }

    // 缩放功能未启用 → 父类默认行为
    if (!testBuiltInFunctions(FunResizePlot)) {
        QwtFigureWidgetOverlay::mousePressEvent(me);
        return;
    }
    
    // 检查控制类型
    auto* activeW = currentActiveWidget();
    if (activeW) {
        const QPoint pos = me->pos();
        auto ct = getMidpointControlType(pos, activeW->frameGeometry());
        if (ct == None) {
            me->ignore();  // 让事件继续传递
            return;
        }
        else if (ct != OutSide) 
        {
            startResize(ct, pos);
            me->accept();
            return;
        }
    }

    // 父类处理缩放和选中切换
    QwtFigureWidgetOverlay::mousePressEvent(me);
}

void EditFigureOverlay::mouseMoveEvent(QMouseEvent* me) 
{
    if (!testBuiltInFunctions(FunResizePlot)) {
        // 没有resize plot 功能，退出
        return QwtFigureWidgetOverlay::mouseMoveEvent(me);
    }
    QWidget* activeW = currentActiveWidget();
    if (!activeW) {
        // 没有激活窗口，更新光标并传递事件
        unsetCursor();
        QwtWidgetOverlay::mouseMoveEvent(me);
        return;
    }
    if(!isResizing())
    {
        // 更改鼠标样式
        auto ct = getMidpointControlType(me->pos(), activeW->frameGeometry());
        if (ct == OutSide || ct == None) {
            unsetCursor();
            me->ignore();  // 让事件继续传递
        } else {
            Qt::CursorShape cur = controlTypeToCursor(ct);
            setCursor(cur);
            me->accept();
        }
        return;
    }
    else
    {
        return QwtFigureWidgetOverlay::mouseMoveEvent(me);
    }
}


void EditFigureOverlay::drawControlLine(QPainter* painter, const QRect& actualRect, const QRectF& normRect) const
{
    // 先调用父类绘制：边框 + 虚线 + 百分比文本 + 四角控制点
    QwtFigureWidgetOverlay::drawControlLine(painter, actualRect, normRect);

    // 在四条边中点位置额外绘制控制点
    QSize cpSize = controlPointSize();
    QPoint offset(cpSize.width() / 2, cpSize.height() / 2);
    QRect cpRect(QPoint(0, 0), cpSize);

    painter->setPen(Qt::NoPen);
    painter->setBrush(controlPointBrush());

    QRect edgeRect = actualRect.adjusted(1, 1, -1, -1);
    int cx = edgeRect.center().x();
    int cy = edgeRect.center().y();

    // 上边中点
    cpRect.moveTo(QPoint(cx, edgeRect.top()) - offset);
    painter->drawRect(cpRect);
    // 下边中点
    cpRect.moveTo(QPoint(cx, edgeRect.bottom()) - offset);
    painter->drawRect(cpRect);
    // 左边中点
    cpRect.moveTo(QPoint(edgeRect.left(), cy) - offset);
    painter->drawRect(cpRect);
    // 右边中点
    cpRect.moveTo(QPoint(edgeRect.right(), cy) - offset);
    painter->drawRect(cpRect);
}

AST_NAMESPACE_END
