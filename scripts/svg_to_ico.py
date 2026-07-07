#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""将 SVG 文件转换为包含多尺寸的 Windows ICO 文件。"""

import argparse
import io
import sys
from pathlib import Path

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")

try:
    import cairosvg
except ImportError:
    cairosvg = None

try:
    from PIL import Image
except ImportError:
    Image = None


def parse_sizes(value):
    sizes = []
    for item in value.split(","):
        item = item.strip()
        if not item:
            continue
        size = int(item)
        if size <= 0:
            raise argparse.ArgumentTypeError("尺寸必须是正整数")
        sizes.append(size)
    if not sizes:
        raise argparse.ArgumentTypeError("至少需要指定一个尺寸")
    return sizes


def render_svg(svg_path, size):
    png_data = cairosvg.svg2png(
        url=str(svg_path),
        output_width=size,
        output_height=size,
    )
    image = Image.open(io.BytesIO(png_data)).convert("RGBA")
    return image


def convert_svg_to_ico(svg_path, ico_path, sizes):
    images = [render_svg(svg_path, size) for size in sizes]
    ico_path.parent.mkdir(parents=True, exist_ok=True)
    images[-1].save(
        ico_path,
        format="ICO",
        sizes=[(size, size) for size in sizes],
        append_images=images[:-1],
    )


def main():
    parser = argparse.ArgumentParser(
        description="将 SVG 转换为包含多尺寸的 ICO 文件。",
        add_help=False,
    )
    parser.add_argument("-h", "--help", action="help", help="显示帮助信息并退出")
    parser.add_argument("svg", type=Path, help="输入 SVG 文件")
    parser.add_argument("-o", "--output", type=Path, help="输出 ICO 文件")
    parser.add_argument(
        "--sizes",
        type=parse_sizes,
        default=parse_sizes("16,24,32,48,64,128,256"),
        help="逗号分隔的图标尺寸，默认：16,24,32,48,64,128,256",
    )
    args = parser.parse_args()

    if cairosvg is None:
        print("错误：缺少 cairosvg。请运行：pip install cairosvg pillow", file=sys.stderr)
        return 1
    if Image is None:
        print("错误：缺少 pillow。请运行：pip install cairosvg pillow", file=sys.stderr)
        return 1

    svg_path = args.svg
    if not svg_path.exists():
        print(f"错误：输入文件不存在：{svg_path}", file=sys.stderr)
        return 1
    if svg_path.suffix.lower() != ".svg":
        print(f"错误：输入文件必须是 SVG：{svg_path}", file=sys.stderr)
        return 1

    ico_path = args.output or svg_path.with_suffix(".ico")
    convert_svg_to_ico(svg_path, ico_path, args.sizes)
    print(f"已写入：{ico_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
