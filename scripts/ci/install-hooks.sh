#!/usr/bin/env bash
#
# 把仓库里的 .githooks 设为本仓库的 git 钩子目录。
#
#   install-hooks.sh
#
# 退出码：0 成功 · 2 不在 git 仓库里
#
# 为什么钩子放在 .githooks 而不是 .git/hooks：后者不受版本管理，改了没法跟着仓库走。
# 代价是 core.hooksPath 是本机配置，每个 clone 都得自己执行一次


set -euo pipefail

here="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
root="$(cd -- "${here}/../.." && pwd -P)"

if ! git -C "${root}" rev-parse --git-dir >/dev/null 2>&1; then
    echo "不在 git 仓库里：${root}" >&2
    exit 2
fi

git -C "${root}" config core.hooksPath .githooks

# Windows 上 core.filemode=false，提交进去的钩子可能是 100644，这里补一次可执行位
for hook in "${root}"/.githooks/*; do
    [ -f "${hook}" ] && chmod +x "${hook}"
done

echo "已启用 git 钩子：core.hooksPath=$(git -C "${root}" config --get core.hooksPath)"
echo "跳过某一次提交的检查：git commit --no-verify"
