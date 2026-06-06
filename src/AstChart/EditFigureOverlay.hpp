///
/// @file      EditFigureOverlay.hpp
/// @brief     QwtFigureWidgetOverlay 子类，编辑模式下 Inner 区域点击穿透给 picker
/// @author    axel
/// @date      2026-06-06
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
#include <qwt_figure_widget_overlay.h>

AST_NAMESPACE_BEGIN

/// @brief 编辑模式专用 QwtFigureWidgetOverlay 子类
/// @details 重写 mousePressEvent：子图内部（Inner）区域点击时 ignore() 穿透给 PlotElementPicker，
///          边框/边角仍由父类处理缩放和选中切换。
class AST_CHART_API EditFigureOverlay : public QwtFigureWidgetOverlay {
public:
    using QwtFigureWidgetOverlay::QwtFigureWidgetOverlay;

protected:
    void mousePressEvent(QMouseEvent* me) override;
    void mouseMoveEvent(QMouseEvent* me) override;
    void drawControlLine(QPainter* painter, const QRect& actualRect, const QRectF& normRect) const override;
};

AST_NAMESPACE_END
