///
/// @file      xmake.lua
/// @brief     AstPy模块构建配置
/// @details   使用xmake构建pybind11 Python绑定
/// @author    axel
/// @date      2026-04-09
/// @copyright 版权所有 (C) 2026-present, space-axel/astpy
///
///
/// astpy项目（https://gitee.com/space-axel/astpy）
/// 本项目基于 Apache 2.0 开源许可证分发。
///

-- AstPy Python绑定模块
target("AstPy")
    set_kind("shared")
    add_files("*.cpp")
    add_headerfiles("*.h")
    add_defines("AST_BUILD_LIB_PY")
    add_deps("AstCore", "AstMath", "AstUtil")
    
    -- 添加AstCore头文件路径
    add_includedirs("../AstCore", {public = true})
    add_includedirs("../AstMath", {public = true})
    add_includedirs("../AstUtil", {public = true})
    add_includedirs("../../include", {public = true})
    
    -- pybind11依赖 (header-only)
    add_requires("pybind11", {_configs = {headeronly = true}})
    if has_package("pybind11") then
        add_packages("pybind11")
    end
    
    -- Python开发库
    add_requires("python")
    if has_package("python") then
        add_packages("python")
    end
