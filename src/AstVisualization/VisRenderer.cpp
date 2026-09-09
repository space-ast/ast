///
/// @file      VisRenderer.cpp
/// @brief     渲染器注册表实现
/// @details   维护全局默认渲染器；当未显式设置时，尝试运行时动态加载 VTK 后端模块。
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

#include "VisRenderer.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/FileSystem.hpp"

#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

static VisRenderer* defaultRenderer = nullptr;

namespace
{

/// @brief 连接路径与库名（按平台使用对应分隔符）
std::string joinLibPath(const std::string& dir, const std::string& name)
{
    if (dir.empty()) {
        return name;
    }
#ifdef _WIN32
    return dir + "\\" + name;
#else
    return dir + "/" + name;
#endif
}

/// @brief 取路径的目录部分（去掉文件名）
std::string dirOf(const std::string& path)
{
    const size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return "";
    }
    return path.substr(0, pos);
}

/// @brief 尝试加载并注册默认渲染后端
/// @details 通过 aLoadLibrary 探测 VTK 后端模块，命中后调用其导出的 aVisVtkRegister()。
///          成功后不卸载模块（保持驻留以承载 VTK 窗口）。
void aTryLoadBackend()
{
    // 依据平台与调试后缀枚举候选库名
    std::vector<std::string> names;
#ifdef _WIN32
    names.push_back("AstVisVTK.dll");
    names.push_back("AstVisVTKD.dll");   // Windows debug 版尾随 D 后缀
#else
    names.push_back("libAstVisVTK.so");
    names.push_back("libAstVisVTK.dylib");
#endif

    // 先在可执行文件目录、本模块目录等位置探测
    std::vector<std::string> dirs;
    dirs.push_back("");
    dirs.push_back(aExeDir());
    dirs.push_back(dirOf(aGetModulePathFromAddress(reinterpret_cast<void*>(&VisRenderer::Default))));
    dirs.push_back(aLibDir());

    for (const std::string& dir : dirs) {
        for (const std::string& name : names) {
            void* lib = aLoadLibrary(joinLibPath(dir, name).c_str());
            if (!lib) {
                continue;
            }
            typedef void (*RegisterFn)();
            RegisterFn fn = reinterpret_cast<RegisterFn>(aGetProcAddress(lib, "aVisVtkRegister"));
            if (fn) {
                fn();
                return;   // 注册成功，停止探测
            }
        }
    }
}

}

VisRenderer* VisRenderer::Default()
{
    static bool triedLoad = false;
    if (!defaultRenderer && !triedLoad) {
        triedLoad = true;
        aTryLoadBackend();
    }
    return defaultRenderer;
}

void VisRenderer::SetDefault(VisRenderer* renderer)
{
    if (renderer) {
        defaultRenderer = renderer;
    }
}

AST_NAMESPACE_END
