#!/usr/bin/env bash
#
# find-release-gaps.sh —— 列出 GitHub 最新正式版在 Gitee / GitCode 镜像仓库里缺的附件。
#
#   find-release-gaps.sh [--platform all|gitee|gitcode]
#
# 把待补发项摊平成 JSON 数组打到 stdout: 
#
#   [{"platform":"gitee","tag":"v1.4.0","file":"SpaceAST-v1.4.0-windows-x64.zip"}]
#
# 进度、警告、错误全部走 stderr
#
# 退出码: 0 成功 · 1 查询失败 · 2 用法错误

set -euo pipefail

# 三个平台上的仓库路径(Gitee / GitCode 上都是同一路径的镜像仓库)。
# GITHUB_REPOSITORY 在 Actions 里由平台注入, 本地手工跑时退回默认值
REPO="space-ast/ast"
GITHUB_REPOSITORY="${GITHUB_REPOSITORY:-$REPO}"
GITEE_REPO="$REPO"
GITCODE_REPO="$REPO"

GITEE_API="https://gitee.com/api/v5/repos/${GITEE_REPO}"
GITCODE_API="https://api.gitcode.com/api/v5/repos/${GITCODE_REPO}"
# 裸仓库地址, 用 ls-remote 探 tag 有没有被镜像同步过来。
GITEE_GIT="https://gitee.com/${GITEE_REPO}.git"
GITCODE_GIT="https://gitcode.com/${GITCODE_REPO}.git"

WORK=""
cleanup() { [ -n "$WORK" ] && rm -rf "$WORK"; return 0; }
trap cleanup EXIT

usage() {
    sed -n '3,5p' "$0" | sed 's/^# \{0,1\}//'
}

log() { echo "$*" >&2; }
die() { echo "::error::$*" >&2; exit 1; }

# 将 jq 输出的行尾统一成 LF
# Windows 上装的原生 jq(winget 的 jqlang.jq)走的是 C 运行时的文本模式, 每行输出的是CRLF；
# 而 gh 内置的 jq 是 Go 写的, 只输出 LF。两边清单行尾对不上, comm 就会认为没有一行相同, 把已经躺在平台上的附件全判成待补发。
# 这个脚本要在本机 git bash 里也能照着跑, 所以统一过一遍
jq_lf() {
    jq "$@" | tr -d '\r'
}

# 给 API 地址带上令牌: 有令牌就拼上 ?access_token=, 没有就匿名请求。
#
# 两个平台读公开仓库都不要求令牌, 手工在本地跑时不必先配一堆 token。
# 注意不能把空的 access_token 拼进去: Gitee 会把空值当成非法令牌直接返回 401, 匿名反而读得到。
with_token() {
    local url="$1" token="${2:-}"

    if [ -z "$token" ]; then
        printf '%s' "$url"
        return 0
    fi
    case "$url" in
        *\?*) printf '%s&access_token=%s' "$url" "$token" ;;
        *)    printf '%s?access_token=%s' "$url" "$token" ;;
    esac
}

# 取一个 API: 响应体写进 $2, HTTP 状态码打到 stdout。
#
# 状态码走 stdout 是为了能直接写进 $()；响应体写入文件, 出错时能原样输出到日志。
# curl 不带 -f —— HTTP 错误码是本脚本要判断的正常输入(404 表示 release 还没建), 
# 网络层失败才由调用方处理。
http_get() {
    local url="$1" out="$2"
    curl -sS -o "$out" -w '%{http_code}' "$url"
}

# 按平台名分派到对应的「列出已有附件」实现, 两个平台只在查询方式上不同。
list_assets() {
    case "$1" in
        gitee)   gitee_asset_names   "$2" ;;
        gitcode) gitcode_asset_names "$2" ;;
        *)       die "未知平台 $1" ;;
    esac
}

# 镜像仓库的裸地址。
platform_git_url() {
    case "$1" in
        gitee)   printf '%s' "$GITEE_GIT" ;;
        gitcode) printf '%s' "$GITCODE_GIT" ;;
        *)       die "未知平台 $1" ;;
    esac
}

# 列出 Gitee 上已有的附件名(每行一个, 并按文件名排序)
#
# Gitee 的 release 对象里只有自动生成的源码包, 上传的附件要查 attach_files 接口
# release 还不存在时输出空列表
gitee_asset_names() {
    local tag="$1" code id

    code=$(http_get "$(with_token "${GITEE_API}/releases/tags/${tag}" "${GITEE_TOKEN:-}")" \
        "$WORK/release.json") || die "请求 Gitee release 失败, 请检查网络"
    case "$code" in
        200) id=$(jq_lf -r .id "$WORK/release.json") ;;
        404)
            log "Gitee 上还没有 $tag 的 release"
            return 0
            ;;
        *)
            die "查询 Gitee release 返回 HTTP $code: $(head -c 300 "$WORK/release.json")"
            ;;
    esac

    code=$(http_get \
        "$(with_token "${GITEE_API}/releases/${id}/attach_files?per_page=100" "${GITEE_TOKEN:-}")" \
        "$WORK/attach.json") || die "请求 Gitee 附件列表失败, 请检查网络"
    [ "$code" = 200 ] || die "查询 Gitee 附件列表返回 HTTP $code: $(head -c 300 "$WORK/attach.json")"

    # 一个附件都没有时 Gitee 返回的是 null 而不是 [], 按空列表处理。
    # 直接 .[] 会报 "Cannot iterate over null"
    jq_lf -r '(. // [])[].name' "$WORK/attach.json" | LC_ALL=C sort
}

# 列出 GitCode 上已有的附件名(每行一个, 并按文件名排序)
#
# GitCode 的 release 对象里 assets 混了两类东西: 
# 自动生成的源码包(type: source, 没有数字 id)和上传的附件(type: attach, 带 id)。
# 先按 id 筛, 再用 URL 排掉源码包
gitcode_asset_names() {
    local tag="$1" code

    code=$(http_get "$(with_token "${GITCODE_API}/releases/tags/${tag}" "${GITCODE_TOKEN:-}")" \
        "$WORK/release.json") || die "请求 GitCode release 失败, 请检查网络"
    case "$code" in
        200) ;;
        404)
            log "GitCode 上还没有 $tag 的 release"
            return 0
            ;;
        *)
            die "查询 GitCode release 返回 HTTP $code: $(head -c 300 "$WORK/release.json")"
            ;;
    esac

    jq_lf -r '.assets[]?
              | select(.id != null)
              | select((.browser_download_url // "") | contains("/-/archive/") | not)
              | .name' "$WORK/release.json" | LC_ALL=C sort
}

# 判断镜像仓库上有没有指定的 tag
remote_has_tag() {
    local platform="$1" tag="$2" refs

    if ! refs=$(git ls-remote --tags "$(platform_git_url "$platform")" "refs/tags/$tag"); then
        die "git ls-remote 查询 ${platform} 上的 tag 失败, 请检查网络或仓库地址"
    fi
    if [ -z "$refs" ]; then
        log "${platform} 上还没有 tag $tag(镜像尚未同步), 这一轮跳过"
        return 1
    fi
    return 0
}

main() {
    local platform="all" tag p f total=0

    while [ $# -gt 0 ]; do
        case "$1" in
            # 缺值时 shift 2 会失败, 把剩下的参数吃光, 好让下面统一报「平台名不对」
            --platform) platform="${2:-}"; shift 2 || shift $# ;;
            -h|--help)  usage; exit 0 ;;
            *)          usage >&2; exit 2 ;;
        esac
    done
    case "$platform" in
        all|gitee|gitcode) ;;
        *) die "--platform 只能是 all / gitee / gitcode(收到 $platform)" ;;
    esac

    # 不需要显式输入凭证: 
    # gh 自己会先读 GH_TOKEN / GITHUB_TOKEN, 读不到再退回本机 `gh auth login` 的登录态；
    # 确实没有凭据时 gh 会自己报错, 脚本按非零退出。
    #
    # releases/latest 取的是最新一个非草稿、非预发布的 release, 所以 nightly(prerelease)
    # 不会命中, 拿到的一定是最新正式版。
    tag=$(gh api "repos/${GITHUB_REPOSITORY}/releases/latest" --jq .tag_name)
    case "$tag" in
        ""|null) die "取不到最新正式版的 tag" ;;
    esac
    # GitHub 这边的附件清单就是「应该有的东西」, 名字以它为准。
    gh api "repos/${GITHUB_REPOSITORY}/releases/tags/${tag}" --jq '.assets[].name' \
        | LC_ALL=C sort > "$WORK/github-names.txt"
    log "GitHub 上最新正式版: $tag, 共 $(wc -l < "$WORK/github-names.txt") 个附件"

    # 平台清单和结论都往这里写一份, 最后原样贴到 run 的摘要页上
    : > "$WORK/matrix.jsonl"
    : > "$WORK/summary.md"

    for p in gitee gitcode; do
        [ "$platform" = "all" ] || [ "$platform" = "$p" ] || continue

        log "── $p ──"

        # 判断镜像是否已经完成同步, 否则建 release 会造出分叉 tag 或者被镜像规则拒绝
        if ! remote_has_tag "$p" "$tag"; then
            printf -- '- **%s**: 镜像还没同步 tag %s, 这一轮跳过\n' "$p" "$tag" >> "$WORK/summary.md"
            continue
        fi

        list_assets "$p" "$tag" > "$WORK/${p}-names.txt"
        local have miss
        have=$(wc -l < "$WORK/${p}-names.txt")
        log "  平台上已有 $have 个附件: "
        sed 's/^/    /' "$WORK/${p}-names.txt" >&2

        # GitHub 有、平台没有的, 就是这一轮要补的。
        comm -23 "$WORK/github-names.txt" "$WORK/${p}-names.txt" > "$WORK/${p}-missing.txt"
        if [ ! -s "$WORK/${p}-missing.txt" ]; then
            log "  附件完整, 不需要补发"
            printf -- '- **%s**: %s 个附件都有, 不需要补发\n' "$p" "$have" >> "$WORK/summary.md"
            continue
        fi

        miss=$(wc -l < "$WORK/${p}-missing.txt")
        total=$((total + miss))
        log "  待补发 $miss 个: "
        sed 's/^/    /' "$WORK/${p}-missing.txt" >&2
        {
            printf -- '- **%s**: 已有 %s 个, 待补发 %s 个\n' "$p" "$have" "$miss"
            sed 's/^/  - /' "$WORK/${p}-missing.txt"
        } >> "$WORK/summary.md"

        while IFS= read -r f; do
            [ -n "$f" ] || continue
            jq_lf -cn --arg p "$p" --arg t "$tag" --arg f "$f" \
                '{platform: $p, tag: $t, file: $f}' >> "$WORK/matrix.jsonl"
        done < "$WORK/${p}-missing.txt"
    done

    if [ -n "${GITHUB_STEP_SUMMARY:-}" ]; then
        {
            echo "## 补发计划($tag)"
            echo
            echo "GitHub 上一共 $(wc -l < "$WORK/github-names.txt") 个附件, 下面是各平台的盘点: "
            echo
            cat "$WORK/summary.md"
        } >> "$GITHUB_STEP_SUMMARY"
    fi

    log "待补发合计 $total 项"

    # 空列表序列化为 []
    jq_lf -s -c . "$WORK/matrix.jsonl"
}

WORK="$(mktemp -d)"
main "$@"
