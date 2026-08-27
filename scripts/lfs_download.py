#!/usr/bin/env python3
"""
从 Git LFS 指针文件下载真实的文件内容。

实现的是 git-lfs batch API 的完整流程：

    指针（oid + size）
        -> 推导 LFS 端点（远端地址 + /info/lfs）
        -> 向 /objects/batch 发起 POST，带上 oid
        -> 服务器返回一个带签署（临时有效）的下载地址 href
        -> GET 该 href 获取字节流
        -> 用 sha256 校验字节流是否与 oid 一致
        -> 写入文件

仅使用标准库。适用于 GitHub LFS 以及任何遵循
https://git-lfs.github.com/spec/v1 规范的服务器。

用法示例：

    # 从远端地址推导端点（把 <owner>/<repo> 换成实际的仓库路径）：
    python lfs_download.py -p ceres.bsp -o ceres.bsp \
        -r https://github.com/<owner>/<repo>.git

    # 显式指定端点：
    python lfs_download.py -p ceres.bsp -o ceres.bsp \
        --lfs-url https://github.com/<owner>/<repo>.git/info/lfs

    # 直接用本地 checkout 的远端推导：
    python lfs_download.py -p ceres.bsp -o ceres.bsp -r "$(git remote get-url github)"
"""

import argparse
import hashlib
import json
import sys
import urllib.error
import urllib.request

GIT_LFS_SPEC = "https://git-lfs.github.com/spec/v1"  # LFS 规范标识，用于校验指针头
MEDIA_TYPE = "application/vnd.git-lfs+json"          # Git LFS 的标准媒体类型


def parse_pointer(path):
    """读取指针文件，返回 (oid, size)。

    指针文件是 130 字节左右的纯文本，形如：
        version https://git-lfs.github.com/spec/v1
        oid sha256:06b7...   <- 内容的 sha256 哈希
        size 61440           <- 内容的字节数
    它并不包含任何下载地址。
    """
    oid = None
    size = None
    with open(path, "r", encoding="utf-8") as handle:
        for line in handle:
            line = line.strip()
            if line.startswith("oid sha256:"):
                oid = line.split(":", 1)[1].strip()
            elif line.startswith("size "):
                size = int(line.split(maxsplit=1)[1])
    if not oid:
        raise ValueError("%s: 不是 Git LFS 指针文件（缺少 'oid sha256:' 行）" % path)
    return oid, size


def endpoint_from_remote(remote_url):
    """从 git 远端地址推导 LFS 端点。

    git-lfs 的推导规则是「远端地址 + /info/lfs」，并且【不会】去掉 .git 后缀。
    例如：https://github.com/<owner>/<repo>.git
        -> https://github.com/<owner>/<repo>.git/info/lfs

    注意：GitHub 的 LFS 端点必须保留 .git，否则 422。这一点和普通直觉相反。
    """
    return remote_url.rstrip("/") + "/info/lfs"


def batch_request(endpoint, oid, size, timeout=30):
    """向 batch API 发起 POST，换取单个 oid 的下载链接。

    POST 内容是 JSON，声明 operation=download、transfers=basic 以及目标 oid。
    服务器返回的响应里可能带一个 actions.download.href，这才是真正的下载地址。
    """
    url = endpoint.rstrip("/") + "/objects/batch"
    body = json.dumps({
        "operation": "download",
        "transfers": ["basic"],
        "objects": [{"oid": oid, "size": size}],
    }).encode("utf-8")
    headers = {
        "Content-Type": MEDIA_TYPE,
        "Accept": MEDIA_TYPE,
    }
    request = urllib.request.Request(url, data=body, headers=headers, method="POST")
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            return json.loads(response.read().decode("utf-8"))
    except urllib.error.HTTPError as exc:
        raise RuntimeError(
            "batch 请求 %s 失败：HTTP %s %s" % (url, exc.code, exc.reason)
        ) from exc


def download(oid, size, endpoint, timeout=120):
    """根据 oid 下载字节流，并校验 sha256 是否匹配。

    返回校验通过的原始字节；一旦哈希不匹配说明内容有误或服务器返回了错误数据。
    """
    payload = batch_request(endpoint, oid, size)
    for obj in payload.get("objects", []):
        if obj.get("oid") != oid:
            continue  # 只处理我们要的那个对象
        download_action = obj.get("actions", {}).get("download")
        if not download_action:
            error = obj.get("error", {})
            raise RuntimeError(
                "oid %s 没有可用的下载地址：%s" % (oid, error or "无")
            )
        href = download_action["href"]
        request = urllib.request.Request(href)
        try:
            with urllib.request.urlopen(request, timeout=timeout) as response:
                data = response.read()
        except urllib.error.HTTPError as exc:
            raise RuntimeError(
                "下载 %s 失败：HTTP %s %s" % (oid, exc.code, exc.reason)
            ) from exc

        # 关键一步：用指针里的 oid 校验下载结果，防止拿到损坏或冒名的数据。
        digest = hashlib.sha256(data).hexdigest()
        if digest != oid:
            raise RuntimeError(
                "sha256 校验失败：%s 期望 %s，实际 %s" % (oid, oid, digest)
            )
        return data
    raise RuntimeError("batch 响应中未找到 oid %s" % oid)


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="从 Git LFS 指针文件下载真实内容。",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__.split("\n\n\n", 1)[-1],
    )
    parser.add_argument("-p", "--pointer", required=True,
                        help="Git LFS 指针文件的路径")
    parser.add_argument("-o", "--output", required=True,
                        help="真实内容要写入的目标路径")
    parser.add_argument("-r", "--remote",
                        help="git 远端地址；端点由它推导为 远端+/info/lfs")
    parser.add_argument("--lfs-url",
                        help="显式指定 LFS 端点（优先于 --remote）")
    parser.add_argument("--timeout", type=int, default=120,
                        help="下载超时（秒），默认 120")
    args = parser.parse_args(argv)

    if not args.lfs_url and not args.remote:
        parser.error("请提供 --remote 或 --lfs-url 之一")

    oid, size = parse_pointer(args.pointer)
    endpoint = args.lfs_url or endpoint_from_remote(args.remote)

    sys.stderr.write("oid     : %s\n" % oid)
    sys.stderr.write("size    : %s\n" % size)
    sys.stderr.write("endpoint: %s\n" % endpoint)

    data = download(oid, size, endpoint, args.timeout)

    with open(args.output, "wb") as handle:
        handle.write(data)
    print("成功：已写入 %d 字节到 %s" % (len(data), args.output))
    return 0


if __name__ == "__main__":
    sys.exit(main())
