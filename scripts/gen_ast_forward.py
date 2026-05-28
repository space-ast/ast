"""遍历 include/ 下的模块头文件，在 include/ast/ 中生成转发头文件。

例如 AstCore/Frame.hpp 生成 ast/Frame.hpp，内容为:
    #include "AstCore/Frame.hpp"

当多个模块存在同名头文件时（如 AstCore/Vector.hpp 和 AstMath/Vector.hpp），
转发头文件会列出所有来源:
    #include "AstCore/Vector.hpp"
    #include "AstMath/Vector.hpp"
"""

import os
import traceback


def is_header(filename):
    """判断是否为头文件"""
    return filename.endswith((".hpp", ".h", ".inl"))


def gen_ast_forward_headers(incroot):
    """在 include/ast/ 中生成转发头文件"""
    outdir = os.path.join(incroot, "ast")

    # 按文件名收集头文件: 文件名 -> 相对于 incroot 的路径列表
    headers_by_name = {}

    for root, dirs, filenames in os.walk(incroot):
        rel_root = os.path.relpath(root, incroot)
        rel_root = rel_root.replace("\\", "/")

        # 跳过输出目录自身，避免递归
        if rel_root == "ast":
            dirs[:] = []
            continue

        # 跳过 include 根目录下的全局头文件（如 AstGlobal.h、AstCompiler.h），
        # 它们不是模块头文件，无需生成转发头
        if rel_root == ".":
            continue

        # 跳过 AstCOM 目录，COM 接口头文件无需转发
        if rel_root.startswith("AstCOM"):
            continue

        for filename in filenames:
            if not is_header(filename):
                continue

            # 构建使用方将引用的包含路径，如 "AstCore/Frame.hpp"
            include_path = filename if rel_root == "." else f"{rel_root}/{filename}"
            include_path = include_path.replace("\\", "/")

            headers_by_name.setdefault(filename, []).append(include_path)

    # 创建输出目录
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    # 写入转发头文件
    written = set()
    duplicates = []
    for filename, sources in sorted(headers_by_name.items()):
        dst = os.path.join(outdir, filename)
        sources.sort()
        with open(dst, "w", newline="\n") as f:
            for src in sources:
                f.write(f'#include "{src}"\n')
        written.add(filename)
        if len(sources) > 1:
            duplicates.append((filename, sources))

    # 打印同名头文件
    if duplicates:
        print(f"\n存在 {len(duplicates)} 个同名头文件:")
        for filename, sources in duplicates:
            print(f"  {filename}:")
            for src in sources:
                print(f"    -> {src}")
        print()

    # 清理失效的转发头文件（源文件已删除的）
    for filename in os.listdir(outdir):
        if is_header(filename) and filename not in headers_by_name:
            stale = os.path.join(outdir, filename)
            print(f"删除失效的转发头文件: {stale}")
            os.remove(stale)

    print(f"已生成 {len(written)} 个转发头文件，目标目录: {outdir}")


if __name__ == "__main__":
    try:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        rootdir = os.path.dirname(script_dir)
        incroot = os.path.join(rootdir, "include")
        print(f"rootdir: {rootdir}")
        os.chdir(rootdir)
        gen_ast_forward_headers(incroot)
    except Exception:
        traceback.print_exc()
        raise SystemExit(1)
