#pragma once

#if defined(AST_WITH_MATPLOT) && defined(AST_WITH_AGG)
#include "AstCompiler.h"

A_SUPPRESS_WARNINGS_BEGIN
#include <matplot/util/visitor.h>
A_SUPPRESS_WARNINGS_END

#include "AggRenderer.hpp"

AST_NAMESPACE_BEGIN


/// matplot::visitor 实现 — 将 matplot++ 对象转为 Agg 渲染调用
class AggVisitor : public matplot::visitor {
public:
    AggVisitor(AggRenderer& renderer, double fig_w, double fig_h);

    AggVisitor(const AggVisitor&) = delete;
    AggVisitor& operator=(const AggVisitor&) = delete;

    void setAxes(const matplot::axes_type& axes);

    /// 绘制坐标轴元素 (背景、spines)
    void drawAxes();

    // ---- visit 重载 ----
    void visit(class matplot::line& l) override;

    // 以下暂为空实现
    void visit(class matplot::histogram&) override {}
    void visit(class matplot::function_line&) override {}
    void visit(class matplot::stair&) override {}
    void visit(class matplot::string_function&) override {}
    void visit(class matplot::bars&) override {}
    void visit(class matplot::error_bar&) override {}
    void visit(class matplot::filled_area&) override {}
    void visit(class matplot::surface&) override {}
    void visit(class matplot::matrix&) override {}
    void visit(class matplot::vectors&) override {}
    void visit(class matplot::network&) override {}
    void visit(class matplot::parallel_lines&) override {}
    void visit(class matplot::labels&) override {}
    void visit(class matplot::box_chart&) override {}
    void visit(class matplot::circles&) override {}
    void visit(class matplot::contours&) override {}

private:
    /// 构建 data → pixel (Y-up) 变换矩阵
    agg::trans_affine makeTransform(double xmin, double xmax, double ymin, double ymax) const;
    const agg::trans_affine& getTransform() const{return trans_;}

    AggRenderer& renderer_;
    double fig_w_{}, fig_h_{};

    const matplot::axes_type* axes_{nullptr};
    double xmin_{0.0}, xmax_{1.0}, ymin_{0.0}, ymax_{1.0};
    agg::trans_affine trans_{};
};

AST_NAMESPACE_END

#endif  // AST_WITH_AGG && AST_WITH_MATPLOT
