#!/bin/sh
# AstMock：模拟/测试工具

target "AstMock"
    set_kind "shared"
    add_files "**.cpp"
    add_files "**.c"
    add_includedirs "../../include/AstMock" "{public}"
    add_defines "AST_BUILD_LIB_MOCK"
    add_deps "AstUtil" "AstMath" "AstCore"
    add_headerfiles "(**.hpp)" "AstMock"
    add_headerfiles "(**.h)" "AstMock"
    # @note windows 下 xmake 用 utils.symbols.export_list 导出 snopt 符号，这里不支持
target_end
