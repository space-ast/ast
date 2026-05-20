///
/// @file      QwtPlotVisitor.hpp
/// @brief     matplot++ visitor that creates Qwt plot items
/// @details   Implements matplot::visitor to convert axes_objects into Qwt plot items
/// @author    axel
/// @date      2026-05-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
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
#include <matplot/util/visitor.h>
#include <array>
#include <QColor>

class QwtPlot;

AST_NAMESPACE_BEGIN

inline QColor toQColor(const std::array<float, 4>& c) {
    return QColor::fromRgbF(
        static_cast<double>(c[1]),
        static_cast<double>(c[2]),
        static_cast<double>(c[3]),
        static_cast<double>(1.0 - c[0]));
}

class QwtPlotVisitor : public matplot::visitor {
public:
    explicit QwtPlotVisitor(QwtPlot* plot);
    ~QwtPlotVisitor() override;

    void visit(matplot::line& l) override;
    void visit(matplot::histogram& h) override;
    void visit(matplot::function_line& fl) override;
    void visit(matplot::stair& s) override;

private:
    QwtPlot* plot_;
};

AST_NAMESPACE_END
