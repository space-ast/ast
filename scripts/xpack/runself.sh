#!/bin/sh
# ast 源码自解压安装包（xmake pack -f runself）的安装脚本。
#
# 由 xmake 生成的 __setup__.sh 在解包目录里调用，用户执行 .run 时传的参数会透传进来：
#   sh ./scripts/xpack/runself.sh "$@"
#
# 编译和安装使用工程自带的 xmake.sh（configure + make），不依赖 xmake。
#
# 用法：
#   ./SpaceAST-v0.2.0.gz.run [选项]
#
# 选项：
#   --prefix=DIR      安装目录，默认 $HOME/.local（等价于设置环境变量 PREFIX）
#   --destdir=DIR     DESTDIR，打包/暂存用，默认空
#   -j N, --jobs=N    并行编译任务数，默认 CPU 核数
#   --mode=MODE       编译模式：release（默认）/ debug / coverage
#   --builddir=DIR    构建目录，默认 build
#   --no-build        只解包，不编译不安装
#   -h, --help        显示帮助
#
# 也可以直接用环境变量：PREFIX=~/.local JOBS=8 MODE=release ./SpaceAST-v0.2.0.gz.run

set -e

prefix="${PREFIX:-$HOME/.local}"
destdir="${DESTDIR:-}"
jobs="${JOBS:-}"
mode="${MODE:-release}"
builddir="${BUILDDIR:-build}"
no_build=false

usage() {
    cat <<'EOF'
用法: ./SpaceAST-<version>.gz.run [选项]

选项:
  --prefix=DIR      安装目录，默认 $HOME/.local
  --destdir=DIR     DESTDIR（打包暂存目录），默认空
  -j N, --jobs=N    并行编译任务数，默认 CPU 核数
  --mode=MODE       编译模式: release（默认）/ debug / coverage
  --builddir=DIR    构建目录，默认 build
  --no-build        只解包，不编译不安装
  -h, --help        显示帮助
EOF
}

while test $# -gt 0; do
    case "$1" in
        --prefix=*)   prefix="${1#--prefix=}" ;;
        --prefix)     shift; prefix="${1}" ;;
        --destdir=*)  destdir="${1#--destdir=}" ;;
        --destdir)    shift; destdir="${1}" ;;
        -j)           shift; jobs="${1}" ;;
        -j*)          jobs="${1#-j}" ;;
        --jobs=*)     jobs="${1#--jobs=}" ;;
        --mode=*)     mode="${1#--mode=}" ;;
        --builddir=*) builddir="${1#--builddir=}" ;;
        --no-build)   no_build=true ;;
        -h|--help)    usage; exit 0 ;;
        *)            echo "error: 未知选项 '$1'" >&2; usage >&2; exit 1 ;;
    esac
    shift
done

srcdir=`pwd -P`
echo "==> 源码目录: ${srcdir}"

if ${no_build}; then
    echo "==> 已跳过编译安装（--no-build），源码保留在当前目录"
    exit 0
fi

# 并行度默认取 CPU 核数
if test -z "${jobs}"; then
    jobs=`nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4`
fi

# 检查编译工具是否齐全，缺了给个明确的提示
for tool in make; do
    if ! command -v ${tool} >/dev/null 2>&1; then
        echo "error: 未找到 ${tool}，请先安装 make 和 C/C++ 编译器" >&2
        exit 1
    fi
done
if test ! -f ./configure -o ! -f ./xmake.sh; then
    echo "error: 当前目录不像 ast 源码目录（缺少 configure/xmake.sh）" >&2
    exit 1
fi

echo "==> 配置工程: mode=${mode} builddir=${builddir}"
sh ./configure --mode="${mode}" --builddir="${builddir}"

echo "==> 编译（-j${jobs}）"
make -j"${jobs}"

echo "==> 安装到: ${destdir}${prefix}"
make install PREFIX="${prefix}" DESTDIR="${destdir}"

cat <<EOF
==> 安装完成

    头文件:   ${destdir}${prefix}/include/Ast*/*.hpp
    库文件:   ${destdir}${prefix}/lib/libAst*.so

编译请配置：

    头文件路径:   ${destdir}${prefix}/include
    库文件路径:   ${destdir}${prefix}/lib

使用示例: (以使用 AstCore 和 AstUtil 模块为例)
    g++ main.cpp -I${destdir}${prefix}/include -L${destdir}${prefix}/lib -lAstCore -lAstUtil -Wl,-rpath,${destdir}${prefix}/lib

EOF
