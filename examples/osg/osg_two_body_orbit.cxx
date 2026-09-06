///
/// @file      osg_two_body_orbit.cxx
/// @brief     OSG 可视化二体轨道示例
/// @details   使用 OpenSceneGraph 将二体传播的轨道渲染为 3D 轨迹，叠加一个地球球体。
///            默认弹出交互窗口（鼠标旋转/缩放/平移），同时导出截图 PNG。
/// @author    axel
/// @date      2026-09-06
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
/// 编译: xmake b osg_two_body_orbit
/// 运行: xmake run osg_two_body_orbit   # 弹出交互窗口，并保存 osg_two_body_orbit.png
///

#ifdef AST_WITH_OSG

#ifdef _WIN32
#   define WIN32
#endif
#define _USE_MATH_DEFINES

#include "ast/TwoBody.hpp"
#include "ast/Vector.hpp"
#include "ast/Literals.hpp"
#include "ast/Constants.hpp"

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/Material>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Camera>
#include <osg/GL>
#include <osgDB/WriteFile>
#include <osgGA/TrackballManipulator>

#include <clocale>
#include <cmath>
#include <iostream>
#include <vector>

AST_USING_NAMESPACE
using namespace _AST literals;

// 二体传播一个完整轨道周期，采样为轨迹点列（单位 km）
std::vector<osg::Vec3d> propagateOrbitPoints(int n, double gm, const Vector3d& r0, const Vector3d& v0)
{
    const double rmag = r0.norm();
    const double vmag2 = v0.norm() * v0.norm();
    const double energy = 0.5 * vmag2 - gm / rmag;
    const double a = -gm / (2.0 * energy);
    const double period = 2.0 * kPI * std::sqrt(a * a * a / gm);

    std::cout << "轨道周期: " << period / 60.0 << " min, 半长轴: " << a / 1000.0 << " km\n";

    std::vector<osg::Vec3d> points;
    points.reserve(n + 1);
    for (int i = 0; i <= n; ++i) {
        Vector3d ri = r0;
        Vector3d vi = v0;
        const double t = period * static_cast<double>(i) / static_cast<double>(n);
        if (aTwoBodyProp(t, gm, ri, vi) != eNoError) {
            std::cerr << "传播失败\n";
            return {};
        }
        // 换算为 km 便于可视化
        points.emplace_back(ri[0] / 1000.0, ri[1] / 1000.0, ri[2] / 1000.0);
    }
    return points;
}

// 创建轨道轨迹几何（LINE_STRIP 线带）
osg::ref_ptr<osg::Geode> createOrbitTrajectory(const std::vector<osg::Vec3d>& points, const osg::Vec4& color)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
    verts->reserve(points.size());
    for (const osg::Vec3d& p : points) {
        verts->push_back(osg::Vec3(p.x(), p.y(), p.z()));
    }
    geom->setVertexArray(verts);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, verts->size()));

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(color);
    geom->setColorArray(colors);
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom);

    // 高亮金黄色轨迹，禁用光照使其以纯色显示
    geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.0f));
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    return geode;
}

// 创建地球球体（半径取 km，海洋蓝色、略带透明）
osg::ref_ptr<osg::Geode> createEarthSphere()
{
    const float earthRadius = 6378.0f;  // 地球半径 (km)
    const osg::Vec4 color(0.2f, 0.45f, 0.9f, 0.9f);  // 海洋蓝色

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(0, 0, 0), earthRadius);
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable(sphere);
    shape->setColor(color);
    geode->addDrawable(shape);

    // 材质（漫反射随光照，面片受光）
    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.2f, 0.45f, 0.9f, 0.9f));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.2f, 0.45f, 0.9f, 0.9f));
    material->setAlpha(osg::Material::FRONT_AND_BACK, 0.9f);
    geode->getOrCreateStateSet()->setAttribute(material);

    // 半透明物体：开启混合，并置于透明渲染 bin（在不透明轨迹之后混合）
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    return geode;
}

// 模拟日光的点光源，照亮地球亮面
osg::ref_ptr<osg::LightSource> createLightSource()
{
    osg::ref_ptr<osg::Light> light = new osg::Light;
    light->setLightNum(0);
    light->setPosition(osg::Vec4(30000.0, 30000.0, 30000.0, 1.0));  // 从斜上方照射
    light->setAmbient(osg::Vec4(0.25f, 0.25f, 0.30f, 1.0f));
    light->setDiffuse(osg::Vec4(0.9f, 0.9f, 0.9f, 1.0f));
    light->setSpecular(osg::Vec4(0.4f, 0.4f, 0.4f, 1.0f));

    osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource;
    lightSource->setLight(light);
    return lightSource;
}

int main()
{
    setlocale(LC_ALL, ".UTF-8");

    // ---- 地球与初始轨道状态 ----
    const double gm = kEarthGrav;          // 地球引力常数 (m³/s²)
    const double r0 = 7000.0_km;           // 近地点高度 (m)

    const Vector3d r{ r0, 0.0, 0.0 };      // 位置 (m)
    const Vector3d v{ 0.0, 7.6_km_s, 1.5_km_s };  // 速度 (m/s)，含 z 分量产生轨道倾角

    // 传播一个完整轨道周期，采样为轨迹点列
    const int n = 200;
    std::vector<osg::Vec3d> points = propagateOrbitPoints(n, gm, r, v);
    if (points.empty()) {
        return 1;
    }

    // ---- 组合场景图 ----
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild(createOrbitTrajectory(points, osg::Vec4(1.0f, 0.8f, 0.2f, 1.0f)));  // 金黄色轨迹
    root->addChild(createEarthSphere());
    root->addChild(createLightSource());

    // ---- 查看器与相机操作器 ----
    osgViewer::Viewer viewer;
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);  // 便于读回像素
    viewer.setSceneData(root);

    osg::ref_ptr<osgGA::TrackballManipulator> manipulator = new osgGA::TrackballManipulator;
    // 与 VTK 示例一致的视角（Azimuth=35°, Elevation=22°），换算为世界坐标 eye
    const double az = osg::DegreesToRadians(35.0);
    const double el = osg::DegreesToRadians(22.0);
    const double distance = 40000.0;  // 视距 (km)
    const osg::Vec3d eye(
        distance * std::cos(el) * std::cos(az),
        distance * std::cos(el) * std::sin(az),
        distance * std::sin(el));
    manipulator->setHomePosition(eye, osg::Vec3d(0.0, 0.0, 0.0), osg::Vec3d(0.0, 0.0, 1.0));
    viewer.setCameraManipulator(manipulator);
    manipulator->home(0.0);  // 让相机初始落在 home 视角

    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.addEventHandler(new osgViewer::HelpHandler);

    viewer.setUpViewInWindow(100, 100, 1024, 768);

    // 深空背景 + 自动近远裁剪，避免大尺度被裁掉
    viewer.getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.12f, 1.0f));
    viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);

    // ---- 先渲染一帧并保存 PNG ----
    viewer.realize();
    viewer.frame();

    osg::ref_ptr<osg::Viewport> vp = viewer.getCamera()->getViewport();
    if (vp.valid()) {
        const int w = static_cast<int>(std::round(vp->width()));
        const int h = static_cast<int>(std::round(vp->height()));

        osg::ref_ptr<osg::GraphicsContext> context = viewer.getCamera()->getGraphicsContext();
        if (context.valid()) {
            context->makeCurrent();
            glReadBuffer(GL_FRONT);  // 双缓冲交换后，画面位于前缓冲
            osg::ref_ptr<osg::Image> image = new osg::Image;
            image->readPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE);
            glReadBuffer(GL_BACK);   // 恢复默认
            context->releaseContext();

            if (osgDB::writeImageFile(*image, "osg_two_body_orbit.png")) {
                std::cout << "已保存截图: osg_two_body_orbit.png\n";
            } else {
                std::cerr << "截图保存失败\n";
            }
        }
    }

    // ---- 交互窗口：支持鼠标旋转/缩放/平移，关闭窗口退出 ----
    std::cout << "按 Esc 或关闭窗口退出。\n";
    std::cout << "  左键拖拽: 旋转视图\n  中键拖拽/滚轮: 平移/缩放\n  右键拖拽: 缩放\n";

    return viewer.run();
}

#else
#include <cstdio>
int main() {
    std::printf("OSG 未安装，示例已跳过。\n");
    return 0;
}
#endif
