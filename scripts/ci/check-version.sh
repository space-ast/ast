#!/usr/bin/env bash
#
# 检查 xmake.lua 与 xmake.sh 里声明的工程版本号是否一致。
#
#   check-version.sh [--staged]
#
#   --staged  改读暂存区（git show :<file>）而不是工作区，给 pre-commit 钩子用，获取将会被提交的内容
#
# 退出码：0 一致 · 1 不一致或解析失败 · 2 用法错误

set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd -P)"

staged=false
case "${1:-}" in
    --staged) staged=true ;;
    "")       ;;
    *)        echo "unknown option: $1" >&2; exit 2 ;;
esac

if "${staged}"; then
    # 文件不在暂存区时（比如还没 add）按空内容处理，下面自然报「解析不到」
    file_content() { git -C "${root}" show ":$1" 2>/dev/null || true; }
    source_desc="暂存区"
else
    file_content() { cat "${root}/$1"; }
    source_desc="工作区"
fi

# xmake.lua 里是 `set_version("0.3.1", {...})`
version_in_lua() {
    file_content xmake.lua \
        | sed -n 's/^[[:space:]]*set_version[[:space:]]*("\([0-9][0-9.]*\)".*/\1/p' \
        | head -n 1
}

# xmake.sh 里是 `set_version "0.3.1" "%Y%m%d"`
version_in_sh() {
    file_content xmake.sh \
        | sed -n 's/^[[:space:]]*set_version[[:space:]]*"\([0-9][0-9.]*\)".*/\1/p' \
        | head -n 1
}

v_lua="$(version_in_lua)"
v_sh="$(version_in_sh)"

if [ -z "${v_lua}" ]; then
    echo "FAIL: ${source_desc}里的 xmake.lua 找不到 set_version(\"<x.y.z>\")" >&2
    exit 1
fi
if [ -z "${v_sh}" ]; then
    echo "FAIL: ${source_desc}里的 xmake.sh 找不到 set_version \"<x.y.z>\"" >&2
    exit 1
fi

if [ "${v_lua}" != "${v_sh}" ]; then
    echo "FAIL: 版本号不一致 —— xmake.lua=${v_lua}，xmake.sh=${v_sh}" >&2
    echo "      改版本号时请两处一起改（tag-release.yml 取的是 xmake.lua）" >&2
    exit 1
fi

echo "OK: 版本号一致（${v_lua}，取自${source_desc}）"
