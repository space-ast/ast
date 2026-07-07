///
/// @file      AggBackend.hpp
/// @brief     matplot++ Agg 后端 — 基于 Anti-Grain Geometry 的离屏渲染
/// @details   实现 matplot::backend::backend_interface, 将 figure_type 图元树
///            渲染为像素缓冲区并输出为图像文件
/// @author    axel
/// @date      2026-06-07
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

#if defined(AST_WITH_AGG) && defined(AST_WITH_MATPLOT)
#include <matplot/backend/backend_interface.h>

#include <memory>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// matplot++ Agg 后端
///
/// 非交互式后端，将 matplot figure 渲染到内存像素缓冲区 (RGBA32)，
/// 然后输出为 BMP 文件。
class AST_PLOT_API AggBackend : public matplot::backend::backend_interface {
public:
    AggBackend();
    virtual ~AggBackend() override;

    // ---- backend_interface 实现 ----

    bool consumes_gnuplot_commands() override;   // false
    bool is_interactive() override;              // false

    const std::string& output() override;
    const std::string& output_format() override;
    bool output(const std::string& filename) override;
    bool output(const std::string& filename,
                const std::string& file_format) override;

    unsigned int width() override;
    unsigned int height() override;
    void width(unsigned int new_width) override;
    void height(unsigned int new_height) override;

    unsigned int position_x() override;
    unsigned int position_y() override;
    void position_x(unsigned int new_position_x) override;
    void position_y(unsigned int new_position_y) override;

    void window_title(const std::string& title) override;
    std::string window_title() override;

    bool new_frame() override;
    bool render_data() override;
    void show(matplot::figure_type* f) override;
    bool should_close() override;
    bool supports_fonts() override;

    void draw(matplot::figure_type* f) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/*! @} */

AST_NAMESPACE_END

#endif  // AST_WITH_AGG && AST_WITH_MATPLOT
