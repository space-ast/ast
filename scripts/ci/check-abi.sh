#!/usr/bin/env bash
#
# 扫描发布出去的产物包，检查 ELF 的 glibc / libstdc++ ABI 版本门槛。
#
#   check-abi.sh <artifact.zip|dir> [--max-glibc=2.28] [--max-glibcxx=3.4.29]
#                 [--glibcxx=warn|fail] [--no-needed] [--quiet]
#
# 退出码：0 通过 · 1 超限 · 2 用法或环境错误
#
# 扫的是**打包后的 zip**（而不是 build 目录），这样打进包里的第三方 .so 也一并覆盖。
# 只解析 `readelf --version-info` 的 Version needs 段：Version definition 段是库自己
# 的版本标签，不是它对别人的要求，混进来会把门槛算高一截。
#
# 为什么要卡这个：产物能跑在哪个发行版上，取决于它引用的最高 GLIBC_/GLIBCXX_ 版本，
# 而不是编译它的发行版。构建环境一换（runner 镜像、容器、工具链），这个数字就会悄悄
# 变化，且不会有任何编译期警告 —— 只有符号表能看出来。

set -euo pipefail

MAX_GLIBC="2.28"
MAX_GLIBCXX=""
GLIBCXX_MODE="warn"
SHOW_NEEDED=1
QUIET=0
TARGET=""

usage() {
    sed -n '3,6p' "$0" | sed 's/^# \{0,1\}//'
}

while [ $# -gt 0 ]; do
    case "$1" in
        --max-glibc=*)   MAX_GLIBC="${1#*=}" ;;
        --max-glibcxx=*) MAX_GLIBCXX="${1#*=}" ;;
        --glibcxx=*)     GLIBCXX_MODE="${1#*=}" ;;
        --no-needed)     SHOW_NEEDED=0 ;;
        --quiet)         QUIET=1 ;;
        -h|--help)       usage; exit 0 ;;
        -*)              echo "unknown option: $1" >&2; exit 2 ;;
        *)
            if [ -n "$TARGET" ]; then echo "只能指定一个目标（已有 $TARGET）" >&2; exit 2; fi
            TARGET="$1" ;;
    esac
    shift
done

if [ -z "$TARGET" ]; then
    usage >&2
    exit 2
fi
case "$GLIBCXX_MODE" in
    warn|fail) ;;
    *) echo "--glibcxx 只能是 warn 或 fail（收到 $GLIBCXX_MODE）" >&2; exit 2 ;;
esac
if ! command -v readelf >/dev/null 2>&1; then
    echo "找不到 readelf，请先安装 binutils" >&2
    exit 2
fi

WORK=""
cleanup() { [ -n "$WORK" ] && rm -rf "$WORK"; return 0; }
trap cleanup EXIT

case "$TARGET" in
    *.zip)
        if ! command -v unzip >/dev/null 2>&1; then
            echo "找不到 unzip，无法解压 $TARGET" >&2
            exit 2
        fi
        WORK="$(mktemp -d)"
        unzip -qq "$TARGET" -d "$WORK"
        ROOT="$WORK"
        ;;
    *)
        if [ ! -d "$TARGET" ]; then
            echo "$TARGET 既不是 .zip 也不是目录" >&2
            exit 2
        fi
        ROOT="$TARGET"
        ;;
esac

# $1 > $2 时返回 0
ver_gt() {
    [ "$1" = "$2" ] && return 1
    [ "$(printf '%s\n%s\n' "$1" "$2" | sort -V | tail -n 1)" = "$1" ]
}

# 两个版本号取大者，空值忽略；都为空则输出空
max_of() {
    printf '%s\n%s\n' "$1" "$2" | sed '/^$/d' | sort -V | tail -n 1
}

is_elf() {
    [ -f "$1" ] && [ "$(head -c 4 "$1" 2>/dev/null | od -An -tx1 | tr -d ' \n')" = "7f454c46" ]
}

# 取 Version needs 段里引用的所有版本名
needed_versions() {
    readelf --version-info "$1" 2>/dev/null | awk '
        /^Version needs section/                { inneed = 1; next }
        /^Version (definition|symbols) section/ { inneed = 0; next }
        inneed
    ' | grep -oE '(GLIBCXX|GLIBC|CXXABI|GCC)_[0-9]+(\.[0-9]+)*' || true
}

# stdin 输入版本名列表，取某个前缀族的最高版本
family_max() {
    awk -F_ -v pfx="$1" '$1 == pfx { print $2 }' | sort -V | tail -n 1 || true
}

# 高于通用档位的 CPU 基线（旧机器上会非法指令）
isa_baseline() {
    local out
    out="$(readelf --notes "$1" 2>/dev/null | grep -oE 'x86-64-v[234]' | head -n 1 || true)"
    if [ -z "$out" ]; then
        out="$(readelf -A "$1" 2>/dev/null | grep -oE 'Tag_CPU_arch: v8\.[2-9]' | head -n 1 || true)"
    fi
    printf '%s' "$out"
}

list_files() {
    ( cd "$1" && find . -type f | sed 's|^\./||' | sort )
}

all_glibc=""
all_glibcxx=""
all_cxxabi=""
over_glibc=0
over_glibcxx=0
shipped_runtime=""
isa_notes=""

printf '%-56s %-9s %-10s %-9s\n' FILE GLIBC GLIBCXX CXXABI
printf '%s\n' '-----------------------------------------------------------------------------------------'

while IFS= read -r rel; do
    [ -n "$rel" ] || continue
    f="$ROOT/$rel"
    is_elf "$f" || continue

    case "$rel" in
        */libstdc++.so*|*/libgcc_s.so*) shipped_runtime="$shipped_runtime $rel" ;;
    esac

    isa="$(isa_baseline "$f")"
    [ -n "$isa" ] && isa_notes="$isa_notes $rel($isa)"

    vers="$(needed_versions "$f")"
    [ -n "$vers" ] || continue

    g="$(printf '%s\n' "$vers" | family_max GLIBC)"
    x="$(printf '%s\n' "$vers" | family_max GLIBCXX)"
    a="$(printf '%s\n' "$vers" | family_max CXXABI)"

    printf '%-56s %-9s %-10s %-9s\n' "$rel" "${g:--}" "${x:--}" "${a:--}"

    if [ -n "$g" ]; then
        all_glibc="$(max_of "$all_glibc" "$g")"
        if ver_gt "$g" "$MAX_GLIBC"; then
            over_glibc=$((over_glibc + 1))
            echo "  ^^ GLIBC $g > $MAX_GLIBC"
        fi
    fi
    if [ -n "$x" ]; then
        all_glibcxx="$(max_of "$all_glibcxx" "$x")"
        if [ -n "$MAX_GLIBCXX" ] && ver_gt "$x" "$MAX_GLIBCXX"; then
            over_glibcxx=$((over_glibcxx + 1))
            echo "  ^^ GLIBCXX $x > $MAX_GLIBCXX"
        fi
    fi
    if [ -n "$a" ]; then
        all_cxxabi="$(max_of "$all_cxxabi" "$a")"
    fi
done < <(list_files "$ROOT")

echo
if [ "$SHOW_NEEDED" = 1 ] && { [ -d "$ROOT/bin" ] || [ -d "$ROOT/lib" ]; }; then
    echo "=== bin/ 与 lib/ 下的运行时依赖与 runpath ==="
    while IFS= read -r rel; do
        [ -n "$rel" ] || continue
        f="$ROOT/$rel"
        is_elf "$f" || continue
        readelf -d "$f" 2>/dev/null \
            | grep -E '\(NEEDED\)|\(RUNPATH\)|\(RPATH\)' \
            | sed -e "s|^|$rel: |" -e 's|  *| |g' || true
    done < <(cd "$ROOT" && for d in bin lib; do [ -d "$d" ] && find "$d" -type f; done | sort)
    echo
fi

{
    echo "## ABI 门槛：$(basename "$TARGET")"
    echo
    echo "| 项目 | 实测 | 门槛 |"
    echo "| --- | --- | --- |"
    echo "| GLIBC | ${all_glibc:-无} | ${MAX_GLIBC} |"
    echo "| GLIBCXX | ${all_glibcxx:-无} | ${MAX_GLIBCXX:-未设（仅报告）} |"
    echo "| CXXABI | ${all_cxxabi:-无} | — |"
    echo
    if [ -n "$shipped_runtime" ]; then
        echo "⚠️ 包内出现编译器运行时（按约定不应打包）：${shipped_runtime}"
        echo
    fi
    if [ -n "$isa_notes" ]; then
        echo "⚠️ 以下文件的 CPU 基线高于通用档位，旧机器上会非法指令：${isa_notes}"
        echo
    fi
} >> "${GITHUB_STEP_SUMMARY:-/dev/null}"

if [ "$QUIET" != 1 ]; then
    echo "GLIBC 上限:   ${all_glibc:-无}  (门槛 ${MAX_GLIBC})"
    echo "GLIBCXX 上限: ${all_glibcxx:-无}  ${MAX_GLIBCXX:+（门槛 ${MAX_GLIBCXX}）}"
    echo "CXXABI 上限:  ${all_cxxabi:-无}"
    [ -n "$shipped_runtime" ] && echo "警告：包内出现编译器运行时${shipped_runtime}"
    [ -n "$isa_notes" ] && echo "警告：CPU 基线偏高${isa_notes}"
fi

fail=0
if [ "$over_glibc" -gt 0 ]; then
    echo "FAIL: $over_glibc 个文件的 GLIBC 需求超过 ${MAX_GLIBC}" >&2
    fail=1
fi
if [ "$over_glibcxx" -gt 0 ]; then
    if [ "$GLIBCXX_MODE" = "fail" ]; then
        echo "FAIL: $over_glibcxx 个文件的 GLIBCXX 需求超过 ${MAX_GLIBCXX}" >&2
        fail=1
    else
        echo "WARN: $over_glibcxx 个文件的 GLIBCXX 需求超过 ${MAX_GLIBCXX}（本轮只报告）" >&2
    fi
fi

if [ "$fail" -eq 0 ]; then
    echo "OK: GLIBC 需求不超过 ${MAX_GLIBC}"
fi
exit "$fail"
