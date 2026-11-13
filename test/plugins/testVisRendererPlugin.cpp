///
/// @file      testVisRendererPlugin.cpp
/// @brief     可视化渲染后端插件接口测试
/// @details   验证按插件键创建/选择渲染器的边界行为；VTK 插件缺失时跳过成功路径
/// @author    axel
/// @date      2026-11-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "ast/Test.hpp"
#include "ast/VisBackend.hpp"
#include "ast/VisRenderer.hpp"
#include "ast/PluginRegistry.hpp"
#include <string>
#include <vector>

AST_USING_NAMESPACE

TEST(VisRendererPlugin, EmptyKey)
{
    EXPECT_EQ(aUseVisRenderer(StringView()), eErrorNullInput);
    EXPECT_TRUE(aNewVisRenderer(StringView()) == nullptr);
}

TEST(VisRendererPlugin, MissingBackend)
{
    EXPECT_TRUE(aNewVisRenderer("__no_such_vis_backend__") == nullptr);
    EXPECT_EQ(aUseVisRenderer("__no_such_vis_backend__"), eErrorNotFound);
}

TEST(VisRendererPlugin, NamesCallable)
{
    const std::vector<std::string> names = aVisRendererNames();
    (void)names;
}

TEST(VisRendererPlugin, UseVtkWhenAvailable)
{
    aScanPlugins();
    if (!aPluginRegistry().find<IVisRendererProvider>("vtk"))
        return;   // 未构建 VTK 插件，或运行库缺失导致加载失败

    EXPECT_EQ(aUseVisRenderer("vtk"), eNoError);
    EXPECT_TRUE(VisRenderer::Default() != nullptr);
}

GTEST_MAIN()
