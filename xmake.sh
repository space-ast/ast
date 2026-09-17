#!/bin/sh
# xmake.sh 构建配置（脚本式构建，用法类似 autotools）
#
# 用法：
#   ./configure                       # 配置工程，默认 release 模式
#   ./configure --mode=debug          # debug 模式
#   ./configure --mode=coverage       # 覆盖率模式
#   ./configure --generator=ninja     # 生成 build.ninja 而不是 Makefile（需要已安装 ninja）
#   ./configure --fmt=y --eigen=n     # 手动开启/关闭某个可选依赖（默认自动探测）
#   ./configure --builddir=build-xsh  # 指定构建目录，默认 build（与 xmake 的 release 输出目录同名，注意别混用）
#   make -j                           # 编译全部模块
#   make AstCore                      # 只编译指定模块
#   make install                      # 安装到 $(PREFIX)
#
# 说明：
#   1. 目前只接入 src/ 下的库模块（test/examples/projects/thirdparty 对应的目录尚未接入）；
#   2. Qt(moc/uic/rcc)、SWIG、COM IDL、WASM、Fortran 等依赖 xmake 自定义 rule 的
#      目标没法用 xmake.sh 描述，所以 AstGUI/AstChart/AstUi*/AstPy/AstCOM/AstWasm
#      等模块没有对应的 xmake.sh；
#   3. 可选第三方依赖通过 option 自动探测（pkg-config / 头文件 / 链接测试都可以），
#      探测不到的依赖，相关代码会被宏自动屏蔽，不影响其它模块编译。
#
# 参考：https://github.com/xmake-io/xmake.sh
#

set_project "ast"
set_version "0.2.0" "%Y%m%d"

#-----------------------------------------------------------------------------
# 工程选项
#-----------------------------------------------------------------------------

# 是否把警告作为编译错误处理（对应 xmake.lua 里的 check_warnings）
# @note 默认关闭，打开后会添加 -Wall -Wextra -Werror
option "check_warnings" "Treat warnings as errors." false

#-----------------------------------------------------------------------------
# 可选第三方依赖
# @note 不设置默认值，由 configure 自动探测；也可以在命令行用 --<name>=y/n 强制指定
#-----------------------------------------------------------------------------

# fmt：格式化输出（AST_WITH_FMT）
option "fmt"
    add_cxxincludes "fmt/format.h"
    add_links "fmt"
    before_check "option_find_fmt"
    add_defines "AST_WITH_FMT"
option_end

option_find_fmt() {
    local cflags=""
    local ldflags=""
    cflags=`pkg-config --cflags fmt 2>/dev/null`
    ldflags=`pkg-config --libs fmt 2>/dev/null`
    option "fmt"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
        if test_nz "${ldflags}"; then
            add_ldflags "${ldflags}"
        fi
    option_end
}

# eigen：线性代数（AST_WITH_EIGEN），头文件库
option "eigen"
    add_cxxincludes "Eigen/Core"
    before_check "option_find_eigen"
    add_defines "AST_WITH_EIGEN"
option_end

option_find_eigen() {
    local cflags=""
    cflags=`pkg-config --cflags eigen3 2>/dev/null`
    option "eigen"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
    option_end
}

# cspice：NASA SPICE 工具箱（AST_WITH_CSPICE）
option "cspice"
    add_cincludes "SpiceUsr.h"
    add_links "cspice"
    before_check "option_find_cspice"
    add_defines "AST_WITH_CSPICE"
option_end

option_find_cspice() {
    local cflags=""
    local ldflags=""
    cflags=`pkg-config --cflags cspice 2>/dev/null`
    ldflags=`pkg-config --libs cspice 2>/dev/null`
    option "cspice"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
        if test_nz "${ldflags}"; then
            add_ldflags "${ldflags}"
        fi
    option_end
}

# agg：矢量绘图后端（AST_WITH_AGG）
option "agg"
    add_cxxincludes "agg2/agg_basics.h"
    add_links "agg"
    before_check "option_find_agg"
    add_defines "AST_WITH_AGG"
option_end

option_find_agg() {
    local cflags=""
    local ldflags=""
    cflags=`pkg-config --cflags agg 2>/dev/null`
    ldflags=`pkg-config --libs agg 2>/dev/null`
    option "agg"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
        if test_nz "${ldflags}"; then
            add_ldflags "${ldflags}"
        fi
    option_end
}

# matplotplusplus：绘图（AST_WITH_MATPLOT）
option "matplotplusplus"
    add_cxxincludes "matplot/matplot.h"
    add_links "matplot"
    before_check "option_find_matplotplusplus"
    add_defines "AST_WITH_MATPLOT"
option_end

option_find_matplotplusplus() {
    local cflags=""
    local ldflags=""
    cflags=`pkg-config --cflags matplot++ 2>/dev/null`
    ldflags=`pkg-config --libs matplot++ 2>/dev/null`
    option "matplotplusplus"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
        if test_nz "${ldflags}"; then
            add_ldflags "${ldflags}"
        fi
    option_end
}

# libf2c：f2c 转换后的 Fortran 运行库（AST_WITH_LIBF2C）
option "libf2c"
    add_cxxincludes "f2c.h"
    add_links "f2c"
    before_check "option_find_libf2c"
    add_defines "AST_WITH_LIBF2C"
option_end

option_find_libf2c() {
    local cflags=""
    local ldflags=""
    cflags=`pkg-config --cflags libf2c 2>/dev/null`
    ldflags=`pkg-config --libs libf2c 2>/dev/null`
    option "libf2c"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
        if test_nz "${ldflags}"; then
            add_ldflags "${ldflags}"
        fi
    option_end
}

# ipopt：非线性优化求解器（AST_WITH_IPOPT）
option "ipopt"
    add_cxxincludes "IpIpoptApplication.hpp"
    add_links "ipopt"
    before_check "option_find_ipopt"
    add_defines "AST_WITH_IPOPT"
option_end

option_find_ipopt() {
    local cflags=""
    local ldflags=""
    cflags=`pkg-config --cflags ipopt 2>/dev/null`
    ldflags=`pkg-config --libs ipopt 2>/dev/null`
    option "ipopt"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
        if test_nz "${ldflags}"; then
            add_ldflags "${ldflags}"
        fi
    option_end
}

# vtk：三维可视化（AST_WITH_VTK），同时决定 AstVisVTK 模块是否参与编译
option "vtk"
    add_cxxincludes "vtkVersion.h"
    add_links "vtkCommonCore"
    before_check "option_find_vtk"
    add_defines "AST_WITH_VTK"
option_end

option_find_vtk() {
    local cflags=""
    local ldflags=""
    cflags=`pkg-config --cflags vtk 2>/dev/null`
    ldflags=`pkg-config --libs vtk 2>/dev/null`
    option "vtk"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
        if test_nz "${ldflags}"; then
            add_ldflags "${ldflags}"
        fi
    option_end
}

# gtest：使用外部 GoogleTest，而不是内置的 AstTest/TestFramework.hpp（AST_WITH_GTEST）
option "gtest"
    add_cxxincludes "gtest/gtest.h"
    add_links "gtest"
    before_check "option_find_gtest"
    add_defines "AST_WITH_GTEST"
option_end

option_find_gtest() {
    local cflags=""
    local ldflags=""
    cflags=`pkg-config --cflags gtest 2>/dev/null`
    ldflags=`pkg-config --libs gtest 2>/dev/null`
    option "gtest"
        if test_nz "${cflags}"; then
            add_cxflags "${cflags}"
        fi
        if test_nz "${ldflags}"; then
            add_ldflags "${ldflags}"
        fi
    option_end
}

#-----------------------------------------------------------------------------
# 语言/警告/优化
#-----------------------------------------------------------------------------

# C++11（等价 xmake.lua 的 set_languages("c++11")）
set_languages "c++11"

if is_mode "debug"; then
    set_symbols "debug"
    set_optimizes "none"
elif is_mode "coverage"; then
    set_symbols "debug"
    set_optimizes "none"
    add_cxflags "--coverage"
    add_shflags "--coverage"
    add_ldflags "--coverage"
else
    set_optimizes "faster"                  # -O2
fi

if has_config "check_warnings"; then
    set_warnings "more" "error"
else
    set_warnings "all"
fi

#-----------------------------------------------------------------------------
# 平台相关的公共配置
#-----------------------------------------------------------------------------

# 公共包含目录，例如 `#include "AstCore/OrbitElement.hpp"`
add_includedirs "include"

if is_plat "linux"; then
    add_rpathdirs "@loader_path"             # 运行时依赖库查找目录，使用 @loader_path 而不是 $ORIGIN，避免意外的转义问题
    add_rpathdirs "@loader_path/../lib"      # 运行时依赖库查找目录，指向上一级目录的lib子目录
    add_syslinks "dl" "pthread"
elif is_plat "windows"; then
    add_defines "_CRT_SECURE_NO_WARNINGS" "_SCL_SECURE_NO_WARNINGS"
    add_cxflags "/wd4819"                    # 忽略警告：代码页
    add_cxflags "/wd4251"                    # 忽略警告：需要有 dll 接口
    add_cxflags "/wd4996"                    # 忽略警告：已否决的函数或参数
    add_cxflags "/utf-8"
    add_cxxflags "/we4703"                   # 视为错误：未初始化的本地变量
elif is_plat "mingw"; then
    add_defines "_CRT_SECURE_NO_WARNINGS" "_SCL_SECURE_NO_WARNINGS"
    add_cxflags "-Wno-missing-braces"
elif is_plat "wasm"; then
    add_cxflags "-Wno-missing-braces"
fi

#-----------------------------------------------------------------------------
# 依赖探测结果，等价于 xmake.lua 里的 add_packages(...) + add_defines(...)
#-----------------------------------------------------------------------------

for dep in fmt eigen cspice agg matplotplusplus libf2c ipopt vtk gtest; do
    if has_config "${dep}"; then
        add_options "${dep}"
    fi
done

#-----------------------------------------------------------------------------
# 导入模块
#-----------------------------------------------------------------------------

includes "src"
