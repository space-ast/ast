#!/bin/sh
# src/ 下的模块库（每个模块一个共享库，等价于 xmake.lua 里的一个 target）
#
# 模块按依赖顺序排列：AstUtil -> AstMath -> AstScript/AstWeather -> AstCore -> AstSim -> 其它
#

add_defines "AST_BUILD_LIB"

# @note xmake.sh 暂不支持 set_suffixname（windows debug 库加 D 后缀），这里略过

# for gcc: 将一些警告转换为错误，以发现潜在的问题
if is_plat "linux" or is_plat "mingw"; then
    add_cxflags "-Werror=uninitialized"
    add_cxflags "-Werror=init-self"
    add_cxflags "-Werror=maybe-uninitialized"
    add_cxflags "-Werror=missing-field-initializers"
    # C++ 专用警告
    add_cxxflags "-Werror=effc++"
    add_cxxflags "-Werror=reorder"
fi

if is_plat "windows"; then
    add_cxflags "/utf-8"
fi

# 基础模块
includes "AstUtil"
includes "AstMath"
includes "AstScript"
includes "AstWeather"
includes "AstCore"
includes "AstSim"

# 上层模块
includes "AstAI"
includes "AstFault"
includes "AstCalc"
includes "AstPlot"
includes "AstAnalyzer"
includes "AstOpt"
includes "AstCmd"
includes "AstReport"
includes "AstEngine"
includes "AstMock"
includes "AstVisualization"
includes "AstDataUpdate"
includes "AstSPICE"
includes "AstLoader"
includes "AstTest"

# 依赖 vtk 的模块，探测到 vtk 才编译
if has_config "vtk"; then
    includes "AstVisVTK"
fi

# @note 以下模块依赖 xmake 的自定义 rule，无法用 xmake.sh 描述，暂未接入：
#   AstChart/AstUiCore/AstUiAI/AstUiDataUpdate/AstUiPilot/AstUiPilotBoot/AstUiUtil  -- Qt (moc/uic/rcc)
#   AstPy                                                                      -- SWIG
#   AstCOM                                                                     -- Windows IDL/ATL
#   AstWasm                                                                    -- wasm 平台
#   AstWeather/Atmosphere/{MSIS86,ussa1976}                                     -- Fortran (ifort)
