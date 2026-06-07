#pragma once

#if defined(AST_WITH_MATPLOT)

#include <matplot/util/visitor.h>

#include "AggRenderer.hpp"

/// matplot::visitor 实现 — 将 matplot++ 对象转为 Agg 渲染调用
class AggVisitor : public matplot::visitor {
public:
    AggVisitor(AggRenderer& renderer, double fig_w, double fig_h);

    /// 设置当前 axes (切换子图时调用)
    void set_axes(const matplot::axes_type& ax);

    // ---- visit 重载 ----
    void visit(matplot::line& l) override;

    // 以下暂为空实现
    void visit(matplot::histogram&) override {}
    void visit(matplot::function_line&) override {}
    void visit(matplot::stair&) override {}
    void visit(matplot::string_function&) override {}
    void visit(matplot::bars&) override {}
    void visit(matplot::error_bar&) override {}
    void visit(matplot::filled_area&) override {}
    void visit(matplot::surface&) override {}
    void visit(matplot::matrix&) override {}
    void visit(matplot::vectors&) override {}
    void visit(matplot::network&) override {}
    void visit(matplot::parallel_lines&) override {}
    void visit(matplot::labels&) override {}
    void visit(matplot::box_chart&) override {}
    void visit(matplot::circles&) override {}
    void visit(matplot::contours&) override {}

private:
    /// 构建 data → pixel (Y-up) 变换矩阵
    agg::trans_affine make_transform() const;

    AggRenderer& renderer_;
    const matplot::axes_type* axes_ = nullptr;
    double fig_w_, fig_h_;
};

#endif  // AST_WITH_AGG && AST_WITH_MATPLOT
