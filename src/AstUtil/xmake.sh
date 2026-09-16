#!/bin/sh
# AstUtil：工具库（容器、字符串、解析、IO、单位、RTTI、序列化、网络等）

target "AstUtil"
    set_kind "shared"
    add_files "**.cpp"
    add_files "**.cxx"
    add_includedirs "../../include/AstUtil" "{public}"
    add_defines "AST_BUILD_LIB_UTIL"
    add_headerfiles "(**.hpp)" "AstUtil"
    add_headerfiles "(**.h)" "AstUtil"
    # @note 这里必须用 "目录/(*.ext)" 这种带括号的写法：xmake.sh 内部对 headerfiles 模式
    # 做了不带引号的 shell 展开，像 "../../include/ast/*" 会先被 glob 展开成一堆文件名，
    # 只剩下第一个文件被安装；加上括号后该模式匹配不到真实文件，才能原样传给内部逻辑
    add_headerfiles "../../include/ast/(*.hpp)" "ast"
    add_headerfiles "../../include/ast/(*.h)" "ast"
    add_headerfiles "../../include/ast/(*.inl)" "ast"
    add_headerfiles "../../include/AstGlobal.h"
    add_headerfiles "../../include/AstCompiler.h"
    if is_plat "linux"; then
        add_syslinks "dl"
    fi
    if is_plat "windows" "mingw"; then
        add_syslinks "ole32" "oleaut32" "uuid" "shell32" "user32" "shlwapi"
    fi
target_end
