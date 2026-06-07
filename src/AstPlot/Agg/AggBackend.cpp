///
/// @file      AggBackend.cpp
/// @brief     AggBackend 实现
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

#include "AggBackend.hpp"

#if defined(AST_WITH_AGG) && defined(AST_WITH_MATPLOT)

#include <cstdio>
#include <matplot/core/figure_type.h>
#include <matplot/core/axes_type.h>

#include "AggRenderer.hpp"
#include "AggVisitor.hpp"

AST_NAMESPACE_BEGIN

struct AggBackend::Impl {
    unsigned int width_ = 640;
    unsigned int height_ = 480;
    unsigned int pos_x_ = 100;
    unsigned int pos_y_ = 100;

    std::string window_title_ = "Figure";

    std::unique_ptr<AggRenderer> renderer_;
    matplot::figure_type* figure_ = nullptr;
};

AggBackend::AggBackend()
    : impl_(std::make_unique<Impl>()) {}

AggBackend::~AggBackend() = default;

// ---- 后端类型 ----

bool AggBackend::consumes_gnuplot_commands() { return false; }
bool AggBackend::is_interactive()           { return false; }

// ---- 输出控制 ----
static std::string empty_filename = "";

const std::string& AggBackend::output()        { return empty_filename; }
const std::string& AggBackend::output_format()  { return empty_filename; }

bool AggBackend::output(const std::string& filename) {
    // 从扩展名推断格式
    auto dot = filename.rfind('.');
    std::string format;
    if (dot != std::string::npos)
        format = filename.substr(dot + 1);
    return output(filename, format);
}

bool AggBackend::output(const std::string& filename, const std::string& format) 
{
    // 输出到文件
    if (!filename.empty()) {
        if (format == "bmp") {
            impl_->renderer_->save_bmp(filename.c_str());
            return true;
        }
        // TODO: 支持其他格式 (PNG 等)
    }
    return false;
}

// ---- 尺寸 ----

unsigned int AggBackend::width()         { return impl_->width_; }
unsigned int AggBackend::height()        { return impl_->height_; }
void AggBackend::width(unsigned int w)   { impl_->width_ = w; }
void AggBackend::height(unsigned int h)  { impl_->height_ = h; }

unsigned int AggBackend::position_x()              { return impl_->pos_x_; }
unsigned int AggBackend::position_y()              { return impl_->pos_y_; }
void AggBackend::position_x(unsigned int x)        { impl_->pos_x_ = x; }
void AggBackend::position_y(unsigned int y)        { impl_->pos_y_ = y; }

void AggBackend::window_title(const std::string& t) { impl_->window_title_ = t; }
std::string AggBackend::window_title()              { return impl_->window_title_; }

// ---- 生命周期 ----

bool AggBackend::new_frame()  { return true; }
bool AggBackend::render_data() { return true; }
bool AggBackend::should_close() { return true; }
bool AggBackend::supports_fonts() { return false; }

void AggBackend::show(matplot::figure_type* f) {
    draw(f);
}

// ========================================================================
// draw — 核心渲染入口
// ========================================================================
void AggBackend::draw(matplot::figure_type* f) {
    impl_->figure_ = f;

    unsigned int w = f->width()  > 0 ? f->width()  : impl_->width_;
    unsigned int h = f->height() > 0 ? f->height() : impl_->height_;

    // 创建渲染器
    impl_->renderer_ = std::make_unique<AggRenderer>(w, h);

    // 背景色
    auto bg = f->color();
    impl_->renderer_->clear(agg::rgba(bg[1], bg[2], bg[3], 1.0 - bg[0]));

    // 遍历所有 axes
    AggVisitor visitor(*impl_->renderer_, (double)w, (double)h);

    for (auto& ax : f->children()) {
        visitor.setAxes(*ax);

        // 先画坐标轴 (背景 + spines)
        visitor.drawAxes();

        // 再画数据
        for (auto& obj : ax->children()) {
            obj->accept(visitor);
        }
    }
}

AST_NAMESPACE_END

#endif  // AST_WITH_AGG && AST_WITH_MATPLOT
