///
/// @file      QwtBackend.hpp
/// @brief     matplot++ Qwt backend
/// @details   Qt/Qwt rendering backend for matplot++
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
#include <matplot/backend/backend_interface.h>
#include <memory>
#include <string>

class QwtFigure;

AST_NAMESPACE_BEGIN

class UiFigure;

class AST_CHART_API QwtBackend : public matplot::backend::backend_interface {
public:
    QwtBackend();
    virtual ~QwtBackend() override;

    bool consumes_gnuplot_commands() override;
    bool is_interactive() override;

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

AST_CHART_API void aUseQwtBackend();

AST_NAMESPACE_END
