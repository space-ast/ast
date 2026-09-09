///
/// @file      vtk_two_body_orbit.cpp
/// @brief     VTK 可视化二体轨道示例
/// @details   使用 VTK 将二体传播的轨道渲染为 3D 轨迹，叠加一个地球球体。
///            默认弹出交互窗口（鼠标旋转/缩放/平移），同时导出截图 PNG。
/// @author    axel
/// @date      2026-09-05
/// @copyright 版权所有 (C) 2026-present, ast项目.
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
///
/// 编译: xmake b vtk_two_body_orbit
/// 运行: xmake run vtk_two_body_orbit   # 弹出交互窗口，并保存 vtk_two_body_orbit.png
///

#ifdef AST_WITH_VTK

#ifdef _WIN32
#define NOMINMAX
#endif

#include "ast/TwoBody.hpp"
#include "ast/Vector.hpp"
#include "ast/Literals.hpp"
#include "ast/Constants.hpp"

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
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkCamera.h>
#include <vtkAutoInit.h>

// VTK 9.x 模块化构建：显式初始化 OpenGL2 渲染后端与交互模块，
// 否则可能回退到不完整的渲染路径，导致渲染窗口 / 像素读回异常。
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

#include <clocale>
#include <cmath>
#include <cstdlib>
#include <iostream>

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    setlocale(LC_ALL, ".UTF-8");

    // ---- 地球与初始轨道状态 ----
    const double gm = kEarthGrav;          // 地球引力常数 (m³/s²)
    const double r0 = 7000.0_km;           // 近地点高度 (m)

    const Vector3d r{ r0, 0.0, 0.0 };      // 位置 (m)
    const Vector3d v{ 0.0, 7.6_km_s, 1.5_km_s };  // 速度 (m/s)，含 z 分量产生轨道倾角

    // 由二体能量求解半长轴与轨道周期
    const double rmag = r.norm();
    const double vmag2 = v.norm() * v.norm();
    const double energy = 0.5 * vmag2 - gm / rmag;
    const double a = -gm / (2.0 * energy);
    const double period = 2.0 * kPI * std::sqrt(a * a * a / gm);

    std::cout << "轨道周期: " << period / 60.0 << " min, 半长轴: " << a / 1000.0 << " km\n";

    // ---- 传播一个完整轨道周期，采样为轨迹点列 ----
    const int n = 200;
    auto points = vtkSmartPointer<vtkPoints>::New();
    for (int i = 0; i <= n; ++i) {
        Vector3d ri = r;
        Vector3d vi = v;
        const double t = period * static_cast<double>(i) / static_cast<double>(n);
        if (aTwoBodyProp(t, gm, ri, vi) != eNoError) {
            std::cerr << "传播失败\n";
            return 1;
        }
        // 换算为 km 便于可视化
        points->InsertNextPoint(ri[0] / 1000.0, ri[1] / 1000.0, ri[2] / 1000.0);
    }

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);

    auto polyLine = vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(n + 1);
    for (int i = 0; i <= n; ++i) {
        polyLine->GetPointIds()->SetId(i, i);
    }
    auto cells = vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell(polyLine);
    polyData->SetLines(cells);

    // ---- 轨道演员 ----
    auto orbitMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    orbitMapper->SetInputData(polyData);
    auto orbitActor = vtkSmartPointer<vtkActor>::New();
    orbitActor->SetMapper(orbitMapper);
    orbitActor->GetProperty()->SetColor(1.0, 0.8, 0.2);  // 金黄色轨迹
    orbitActor->GetProperty()->SetLineWidth(2.0);

    // ---- 地球球体 ----
    auto sphere = vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetRadius(6378.0);              // 地球半径 (km)
    sphere->SetThetaResolution(64);
    sphere->SetPhiResolution(64);
    auto earthMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    earthMapper->SetInputConnection(sphere->GetOutputPort());
    auto earthActor = vtkSmartPointer<vtkActor>::New();
    earthActor->SetMapper(earthMapper);
    earthActor->GetProperty()->SetColor(0.2, 0.45, 0.9);  // 海洋蓝色
    earthActor->GetProperty()->SetOpacity(0.9);

    // ---- 渲染器与窗口 ----
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(orbitActor);
    renderer->AddActor(earthActor);
    renderer->SetBackground(0.0, 0.0, 0.12);  // 深空背景
    renderer->GetActiveCamera()->Azimuth(35.0);
    renderer->GetActiveCamera()->Elevation(22.0);
    renderer->ResetCamera();

    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(1024, 768);

    // 默认弹出交互窗口
    const bool interactive = true;

    // ---- 先离屏渲染并保存 PNG（复用已验证的离屏读回路径） ----
    renderWindow->SetOffScreenRendering(1);
    renderWindow->Render();
    auto imageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    imageFilter->SetInput(renderWindow);
    imageFilter->SetReadFrontBuffer(false);   // 离屏渲染内容在背面缓冲，需读背面
    imageFilter->SetInputBufferTypeToRGB();   // 用 RGB，避免背景 alpha=0 被显示成白色
    imageFilter->Update();
    auto pngWriter = vtkSmartPointer<vtkPNGWriter>::New();
    pngWriter->SetFileName("vtk_two_body_orbit.png");
    pngWriter->SetInputConnection(imageFilter->GetOutputPort());
    pngWriter->Write();
    std::cout << "已保存截图: vtk_two_body_orbit.png\n";
    renderWindow->SetOffScreenRendering(0);

    // ---- 交互窗口：支持鼠标旋转/缩放/平移，关闭窗口退出 ----
    if (interactive) {
        auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        interactor->SetRenderWindow(renderWindow);
        renderer->ResetCamera();
        renderWindow->Render();
        std::cout << "按 Q 或关闭窗口退出。\n";
        interactor->Start();
    }

    return 0;
}

#else
#include <cstdio>
int main() {
    std::printf("VTK 未安装，示例已跳过。\n");
    return 0;
}
#endif
