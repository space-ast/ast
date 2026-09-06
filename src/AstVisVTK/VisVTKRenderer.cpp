///
/// @file      VisVTKRenderer.cpp
/// @brief     VTK 渲染后端实现
/// @details   把 AstVisualization 场景图编译为 VTK 场景（轨迹 polyline、天体球体、
///            地面轨迹），支持离屏 PNG 导出与交互窗口。坐标统一采用国际单位（米）。
/// @author    axel
/// @date      2026-09-06
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

#ifdef _WIN32
#   define NOMINMAX
#endif

#include "VisVTKRenderer.hpp"
#include "AstVisualization/VisView.hpp"
#include "AstVisualization/VisGroup.hpp"
#include "AstVisualization/VisTrajectory.hpp"
#include "AstVisualization/VisCelestialBody.hpp"
#include "AstVisualization/VisGroundTrack.hpp"
#include "AstVisualization/VisVisitor.hpp"

#include "AstCore/Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstMath/Vector.hpp"

#include <vector>
#include <cstddef>

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkCamera.h>
#include <vtkTexture.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricEllipsoid.h>
#include <vtkAutoInit.h>

// VTK 9.x 模块化构建：显式初始化 OpenGL2 渲染后端与交互模块，
// 否则可能回退到不完整的渲染路径，导致渲染窗口 / 像素读回异常。
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 把一串点构建为 VTK 折线演员并加入渲染器
void addPolyLine(vtkRenderer* renderer, const std::vector<Vector3d>& points,
                 double scale, const double color[3], double lineWidth)
{
    if (points.empty() || !renderer) return;

    const int n = static_cast<int>(points.size());

    auto vtkPts = vtkSmartPointer<vtkPoints>::New();
    vtkPts->SetDataTypeToDouble();   // 大坐标（米制）下使用双精度，避免 float 精度损失
    vtkPts->SetNumberOfPoints(n);
    for (int i = 0; i < n; ++i) {
        const Vector3d& p = points[static_cast<size_t>(i)];
        vtkPts->SetPoint(i, p[0] * scale, p[1] * scale, p[2] * scale);
    }

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(vtkPts);

    auto polyLine = vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(n);
    for (int i = 0; i < n; ++i) {
        polyLine->GetPointIds()->SetId(i, i);
    }
    auto cells = vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell(polyLine);
    polyData->SetLines(cells);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color[0], color[1], color[2]);
    actor->GetProperty()->SetLineWidth(lineWidth);
    renderer->AddActor(actor);
}

/// @brief 可视化场景构建器（访问者）
/// @details 遍历场景图，把每个 VisObject 转换为对应的 VTK 演员。
class VisVTKSceneBuilder: public VisVisitor
{
public:
    VisVTKSceneBuilder(vtkRenderer* renderer, double scale,
                       Frame* viewFrame, TimePoint epoch)
        : renderer_(renderer)
        , scale_(scale)
        , viewFrame_(viewFrame)
        , epoch_(epoch)
    {}

    void visit(VisGroup& group) override
    {
        for (const auto& child : group.children()) {
            if (child) {
                child->accept(*this);
            }
        }
    }

    void visit(VisTrajectory& trajectory) override
    {
        std::vector<Vector3d> positions;
        trajectory.getPositions(positions);
        const double gold[3] = {1.0, 0.8, 0.2};
        addPolyLine(renderer_, positions, scale_, gold, 2.0);
    }

    void visit(VisCelestialBody& celestialBody) override
    {
        const double radius = celestialBody.radius() * scale_;
        if (radius <= 0.0) {
            return;
        }

        // 参数化椭球（三半径相等即球体），通过 vtkParametricFunctionSource 原生生成纹理坐标
        auto sphere = vtkSmartPointer<vtkParametricEllipsoid>::New();
        sphere->SetXRadius(radius);
        sphere->SetYRadius(radius);
        sphere->SetZRadius(radius);

        auto source = vtkSmartPointer<vtkParametricFunctionSource>::New();
        source->SetParametricFunction(sphere);
        source->SetUResolution(128);
        source->SetVResolution(64);
        source->SetGenerateTextureCoordinates(true);

        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(source->GetOutputPort());

        auto actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0.2, 0.45, 0.9);   // 无纹理时的回退色（海洋蓝）
        actor->GetProperty()->SetOpacity(0.9);

        // 贴图：显式纹理或从天体数据目录自动探测
        const std::string texturePath = celestialBody.effectiveTexture();
        if (!texturePath.empty()) {
            auto imgReader = vtkSmartPointer<vtkImageReader2>::Take(
                vtkImageReader2Factory::CreateImageReader2(texturePath.c_str()));
            if (imgReader && imgReader->CanReadFile(texturePath.c_str())) {
                imgReader->SetFileName(texturePath.c_str());
                imgReader->Update();

                auto texture = vtkSmartPointer<vtkTexture>::New();
                texture->SetInputConnection(imgReader->GetOutputPort());
                texture->InterpolateOn();
                actor->SetTexture(texture);
                // 贴图会与漫反射颜色相乘：置为白色以显示贴图原色，去掉先前蓝色的偏色
                actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
                actor->GetProperty()->SetOpacity(1.0);   // 真实贴图天体更清晰
            }
        }

        // 在视图参考系中按渲染时刻定位天体（Point::getPosIn 会经 frame 图求天体在该系中的位置）
        Vector3d pos{0.0, 0.0, 0.0};
        if (viewFrame_ && epoch_.isValid()) {
            Body* body = celestialBody.body();
            if (body) {
                body->getPosIn(viewFrame_, epoch_, pos);   // 出错时保持原点，可视地兜底
            }
        }
        actor->SetPosition(pos[0] * scale_, pos[1] * scale_, pos[2] * scale_);

        renderer_->AddActor(actor);
    }

    void visit(VisGroundTrack& groundTrack) override
    {
        Body* body = groundTrack.body();
        Point* point = groundTrack.point();
        if (!body || !point) {
            return;
        }

        Frame* frame = body->getFrameFixed();
        if (!frame) {
            frame = body->getFrameICRF();
        }
        const double radius = body->getRadius();
        if (!frame || radius <= 0.0) {
            return;
        }

        // 在固连坐标系中按区间采样，并把每一点投影到天体球面
        const size_t maxPoints = 100000;
        std::vector<Vector3d> surface;
        TimePoint t = groundTrack.interval().start();
        const TimePoint stop = groundTrack.interval().stop();
        const double step = groundTrack.step();

        while (t <= stop && surface.size() < maxPoints) {
            Vector3d pos{};
            if (point->getPosIn(frame, t, pos) == eNoError) {
                const double norm = pos.norm();
                if (norm > 0.0) {
                    surface.push_back(pos * (radius / norm));
                }
            }
            t = t + step;
        }

        const double red[3] = {0.9, 0.2, 0.2};
        addPolyLine(renderer_, surface, scale_, red, 2.0);
    }

private:
    vtkRenderer* renderer_;
    double       scale_;
    Frame*       viewFrame_;
    TimePoint    epoch_;
};

}

errc_t VisVTKRenderer::render(const VisView& view, const TimePoint& epoch)
{
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(bgR_, bgG_, bgB_);

    VisGroup* root = view.objects();
    if (root) {
        VisVTKSceneBuilder builder(renderer, scale_, view.frame(), epoch);
        root->accept(builder);
    }

    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(width_, height_);

    renderer->GetActiveCamera()->Azimuth(azimuth_);
    renderer->GetActiveCamera()->Elevation(elevation_);
    renderer->ResetCamera();

    // 离屏渲染并保存 PNG（复用已验证的离屏读回路径）
    if (savePng_) {
        renderWindow->SetOffScreenRendering(1);
        renderWindow->Render();

        auto imageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
        imageFilter->SetInput(renderWindow);
        imageFilter->SetReadFrontBuffer(false);   // 离屏渲染内容在背面缓冲，需读背面
        imageFilter->SetInputBufferTypeToRGB();   // 用 RGB，避免背景 alpha=0 被显示成白色
        imageFilter->Update();

        auto pngWriter = vtkSmartPointer<vtkPNGWriter>::New();
        pngWriter->SetFileName(pngPath_.c_str());
        pngWriter->SetInputConnection(imageFilter->GetOutputPort());
        pngWriter->Write();

        renderWindow->SetOffScreenRendering(0);
    }

    // 交互窗口：支持鼠标旋转/缩放/平移，关闭窗口退出
    if (interactive_) {
        auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        interactor->SetRenderWindow(renderWindow);

        auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        style->SetDefaultRenderer(renderer);
        style->SetMouseWheelMotionFactor(mouseWheelMotionFactor_);
        interactor->SetInteractorStyle(style);

        renderer->ResetCamera();
        renderWindow->Render();
        interactor->Start();
    }

    return eNoError;
}

AST_NAMESPACE_END


extern "C" AST_VISVTK_API void aVisVtkRegister()
{
    AST_USING_NAMESPACE
    VisRenderer::SetDefault(new VisVTKRenderer());
}
