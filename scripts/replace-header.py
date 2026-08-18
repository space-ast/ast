#!/usr/bin/env python3
# -*- coding: utf-8 -*-
r"""
替换给定目录下所有源文件的版权头。

用法:
  # 将指定目录下所有源文件的版权头替换为 new-header.txt 中的内容
  python replace-header.py E:\0-codes\atk-nt\src\AstWeather --header new-header.txt

  # 使用内联文本作为新的版权头
  python replace-header.py /path/to/dir --inline-header "/// @copyright (C) 2026, MyProject."

  # 预览模式（不实际修改文件）
  python replace-header.py /path/to/dir --header new-header.txt --dry-run

  # 仅处理 .h 和 .cpp 文件
  python replace-header.py /path/to/dir --header new-header.txt --ext .h,.cpp

  # 保留原头部中的元信息行（@file / @brief / @details / @author / @date）
  python replace-header.py /path/to/dir --header new-header.txt --keep-meta

  # 仅在版权头存在时才替换
  python replace-header.py /path/to/dir --header new-header.txt --match-copyright "copyright"
"""

import os
import re
import sys
import stat
import argparse

# ============================================================
# 默认配置
# ============================================================

# 需要处理的默认文件扩展名
DEFAULT_EXTENSIONS = ['.cpp', '.h', '.hpp', '.c', '.cxx', '.inl']

# 默认排除的目录名
DEFAULT_EXCLUDE_DIRS = [
    '.git', '.svn', '.claude', '.github', '.vs', '.vscode',
    '.xmake', '__pycache__', 'node_modules',
    'build', 'artifacts', 'thirdparty', 'data', 'doc', 'docs',
]

# ============================================================
# 正则模式
# ============================================================

# 匹配文件开头所有连续的 /// 注释行（整个头部注释块）
VERSION_HEADER_PATTERN = re.compile(r'\A((?:\s*///[^\n]*\n?)+)')

# 元信息行正则 — 匹配 @file / @brief / @details / @author / @date 行
META_TAG_PATTERN = re.compile(r'@(?:file|brief|details|author|date)\b')

# 作者声明正则（用于替换 @author 行中的作者名）
AUTHOR_PATTERN = re.compile(r'^(\s*///\s*@author\s+).*$', re.MULTILINE)

# 匹配多种注释风格的文件头
# 1. /// 风格（本项目默认）
# 2. // 风格
# 3. /* */ 风格
COMMENT_HEADER_PATTERNS = {
    '///': re.compile(r'\A((?:\s*///[^\n]*\n?)+)'),
    '//': re.compile(r'\A((?:\s*//[^\n]*\n?)+)'),
    '/**': re.compile(r'\A(\s*/\*[*!].*?\*/\s*\n?)', re.DOTALL),
}


def detect_comment_style(content):
    """检测文件使用的注释风格，返回 ('///', '//', 或 '/**') 或 None"""
    if content.startswith('///'):
        return '///'
    if content.startswith('//'):
        return '//'
    if content.startswith('/*'):
        return '/**'
    return None


def ensure_writable(file_path):
    """确保文件可写（Windows 下清除只读属性）"""
    if not os.access(file_path, os.W_OK):
        os.chmod(file_path, stat.S_IWRITE | stat.S_IREAD)


def extract_meta_lines(header_block):
    """从头部注释块中提取元信息行（@file / @brief / @details / @author / @date）"""
    kept_lines = []
    for line in header_block.splitlines(keepends=True):
        if META_TAG_PATTERN.search(line):
            kept_lines.append(line)
    return kept_lines


def load_header_from_file(header_file_path):
    """从文件加载新的版权头文本"""
    with open(header_file_path, 'r', encoding='utf-8') as f:
        header = f.read()
    # 确保头部以换行结尾
    if header and not header.endswith('\n'):
        header += '\n'
    return header


def ensure_comment_prefix(text, comment_style):
    """确保文本的每一行都有正确的注释前缀。

    如果文本行已以注释风格开头，则保留不变；
    否则自动为每行添加注释前缀。
    """
    prefix = comment_style + ' '
    lines = text.split('\n')

    # 检测第一行是否已经有注释前缀
    if lines:
        stripped = lines[0].lstrip()
        # 检查几种常见前缀
        has_prefix = (stripped.startswith('///') or
                      stripped.startswith('//') or
                      stripped.startswith('/*') or
                      stripped.startswith('*'))
        if has_prefix:
            return text

    # 自动添加前缀
    result = []
    for line in lines:
        if line.strip():
            result.append(prefix + line)
        else:
            result.append(comment_style)
    return '\n'.join(result)


def build_new_header(header_text, meta_lines, comment_style, new_author=None):
    """根据新头部文本、保留的元信息和注释风格构建最终头部。

    header_text: 新的版权头正文（可以有或没有注释前缀）
    meta_lines: 从旧头部提取的元信息行（@file/@brief/@details/@author/@date），已有前缀
    comment_style: 注释风格 ('///', '//', 或 '/**')
    new_author: 如果指定，替换元信息中 @author 行的作者名
    """
    # 确保新头部文本有正确的注释前缀
    header_text = ensure_comment_prefix(header_text, comment_style)

    if not header_text.endswith('\n'):
        header_text += '\n'

    if meta_lines:
        # 如果需要替换作者
        if new_author is not None:
            meta_lines = [_replace_author_in_line(line, comment_style, new_author)
                          for line in meta_lines]

        # 元信息行放在前面，版权正文放在后面
        meta_text = ''.join(meta_lines)
        if not meta_text.endswith('\n'):
            meta_text += '\n'
        return meta_text + header_text
    else:
        return header_text


def _replace_author_in_line(line, comment_style, new_author):
    """替换单行中的 @author 值"""
    prefix = comment_style + r'\s*@author\s+'
    pattern = re.compile(r'^(\s*' + re.escape(comment_style) + r'\s*@author\s+).*$')
    return pattern.sub(r'\1' + new_author, line)


def replace_header_in_file(file_path, new_header, dry_run=False, new_author=None):
    """替换单个文件的版权头"""
    try:
        with open(file_path, 'r', encoding='utf-8-sig') as f:
            content = f.read()
    except (UnicodeDecodeError, IOError) as e:
        print(f"  [跳过] 无法读取文件: {e}")
        return False

    if not content.strip():
        print(f"  [跳过] 空文件")
        return False

    match = VERSION_HEADER_PATTERN.match(content)
    if not match:
        print(f"  [跳过] 未找到 /// 版权头")
        return False

    header_block = match.group(0)
    rest = content[match.end():]

    # 提取要保留的元信息行
    meta_lines = extract_meta_lines(header_block)

    # 检查新头部是否与旧头部的非元信息部分相同
    # 先将 new_header 归一化（确保有正确的前缀），再与旧内容比较
    normalized_new = ensure_comment_prefix(new_header, '///')
    old_non_meta = [line for line in header_block.splitlines(keepends=True)
                    if not META_TAG_PATTERN.search(line)]
    old_body = ''.join(old_non_meta)

    if old_body == normalized_new:
        print(f"  [跳过] 版权头已是最新")
        return False

    # 构建新内容
    new_header_final = build_new_header(new_header, meta_lines, '///', new_author)
    new_content = new_header_final + rest

    if dry_run:
        print(f"  [预览] 将替换版权头 (风格: ///):")
        # 显示新头部的前几行
        preview_lines = new_header_final.strip().split('\n')
        for line in preview_lines[:6]:
            print(f"    {line}")
        if len(preview_lines) > 6:
            print(f"    ... (共 {len(preview_lines)} 行)")
        return True

    ensure_writable(file_path)
    with open(file_path, 'w', encoding='utf-8-sig') as f:
        f.write(new_content)
    print(f"  [已替换] (风格: ///)")
    return True


def replace_flexible_header_in_file(file_path, new_header, match_copyright_pattern=None, dry_run=False, new_author=None):
    """
    灵活替换文件版权头 — 支持多种注释风格，以及在旧版权头中查找并替换特定内容。

    当指定 match_copyright_pattern 时，需要旧版权头中匹配到该 pattern 才会执行替换。
    """
    try:
        with open(file_path, 'r', encoding='utf-8-sig') as f:
            content = f.read()
    except (UnicodeDecodeError, IOError) as e:
        print(f"  [跳过] 无法读取文件: {e}")
        return False

    if not content.strip():
        print(f"  [跳过] 空文件")
        return False

    style = detect_comment_style(content)
    if style not in COMMENT_HEADER_PATTERNS:
        print(f"  [跳过] 无法识别注释风格")
        return False

    pattern = COMMENT_HEADER_PATTERNS[style]
    match = pattern.match(content)
    if not match:
        print(f"  [跳过] 未找到注释块头部")
        return False

    header_block = match.group(0)
    rest = content[match.end():]

    # 提取要保留的元信息行
    meta_lines = extract_meta_lines(header_block)

    # 检查是否有实际变化
    normalized_new = ensure_comment_prefix(new_header, style)
    old_non_meta = [line for line in header_block.splitlines(keepends=True)
                    if not META_TAG_PATTERN.search(line)]
    old_body = ''.join(old_non_meta)

    if old_body == normalized_new:
        print(f"  [跳过] 版权头已是最新")
        return False

    # 如果指定了 copyright 匹配模式，检查旧头部是否包含
    if match_copyright_pattern:
        if not re.search(match_copyright_pattern, header_block):
            print(f"  [跳过] 旧版权头未匹配到 pattern: {match_copyright_pattern.pattern}")
            return False

    new_header_final = build_new_header(new_header, meta_lines, style, new_author)
    new_content = new_header_final + rest

    if dry_run:
        print(f"  [预览] 将替换版权头 (风格: {style}):")
        preview_lines = new_header_final.strip().split('\n')
        for line in preview_lines[:6]:
            print(f"    {line}")
        if len(preview_lines) > 6:
            print(f"    ... (共 {len(preview_lines)} 行)")
        return True

    ensure_writable(file_path)
    with open(file_path, 'w', encoding='utf-8-sig') as f:
        f.write(new_content)
    print(f"  [已替换] (风格: {style})")
    return True


def walk_source_files(root_dir, extensions, exclude_dirs):
    """遍历目录下的所有源文件"""
    for dirpath, dirnames, filenames in os.walk(root_dir):
        dirnames[:] = [d for d in dirnames if d not in exclude_dirs]
        for filename in filenames:
            ext = os.path.splitext(filename)[1].lower()
            if ext in extensions:
                yield os.path.join(dirpath, filename)


def main():
    parser = argparse.ArgumentParser(
        description='替换给定目录下所有源文件的版权头',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python replace-header.py E:\\src\\AstWeather --header new-header.txt
  python replace-header.py ./src --inline-header "/// @copyright 版权所有 (C) 2026, MyProject."
  python replace-header.py ./src --header header.txt --dry-run
  python replace-header.py ./src --header header.txt --ext .h,.cpp
  python replace-header.py ./src --header header.txt --match-copyright "版权所有"
        """
    )

    parser.add_argument(
        'directory',
        type=str,
        help='要处理的目标目录路径'
    )

    header_group = parser.add_mutually_exclusive_group(required=False)
    header_group.add_argument(
        '--header', '-f',
        type=str,
        default=None,
        help='包含新版权头的文件路径'
    )
    header_group.add_argument(
        '--inline-header', '-i',
        type=str,
        default=None,
        help='直接指定新的版权头文本（内联，默认为空，即仅保留元信息行）'
    )

    parser.add_argument(
        '--ext', '-e',
        type=str,
        default=','.join(DEFAULT_EXTENSIONS),
        help=f'处理的文件扩展名，逗号分隔 (默认: {",".join(DEFAULT_EXTENSIONS)})'
    )

    parser.add_argument(
        '--exclude-dirs', '-x',
        type=str,
        default=None,
        help='额外排除的目录名，逗号分隔'
    )

    parser.add_argument(
        '--match-copyright', '-m',
        type=str,
        default=None,
        help='仅在旧版权头能够匹配到指定正则 pattern 时才执行替换（例如 "版权所有"）'
    )

    parser.add_argument(
        '--flexible', '-F',
        action='store_true',
        help='启用灵活模式：自动检测多种注释风格（///, //, /**）而非仅限于 /// 风格'
    )

    parser.add_argument(
        '--new-author', '-a',
        type=str,
        default=None,
        help='替换元信息行中 @author 的作者名（例如: --new-author "张三"）'
    )

    parser.add_argument(
        '--dry-run', '-n',
        action='store_true',
        help='预览模式：仅显示将要修改的文件，不实际写入'
    )

    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='显示详细信息（包括跳过的文件）'
    )

    args = parser.parse_args()

    # 验证目录
    root_dir = os.path.abspath(args.directory)
    if not os.path.isdir(root_dir):
        print(f"错误: 目录不存在: {root_dir}")
        sys.exit(1)

    # 加载新头部
    if args.header:
        if not os.path.isfile(args.header):
            print(f"错误: 头部文件不存在: {args.header}")
            sys.exit(1)
        new_header = load_header_from_file(args.header)
    elif args.inline_header is not None:
        # 特殊标记 __EMPTY__ 表示空头部（仅保留元信息行）
        if args.inline_header == '__EMPTY__':
            new_header = '\n'
        else:
            new_header = args.inline_header.replace('\\n', '\n')
            if not new_header.endswith('\n'):
                new_header += '\n'
    else:
        # 默认：空头部，仅保留元信息行（@file/@brief/@details/@author/@date）
        new_header = '\n'

    # 解析扩展名
    extensions = [ext.strip() for ext in args.ext.split(',')]
    # 确保扩展名以点开头
    extensions = [ext if ext.startswith('.') else f'.{ext}' for ext in extensions]

    # 解析排除目录
    exclude_dirs = set(DEFAULT_EXCLUDE_DIRS)
    if args.exclude_dirs:
        extra_dirs = [d.strip() for d in args.exclude_dirs.split(',')]
        exclude_dirs.update(extra_dirs)

    # 编译 copyright 匹配 pattern
    match_pattern = None
    if args.match_copyright:
        match_pattern = re.compile(args.match_copyright)

    print(f"目录: {root_dir}")
    print(f"扩展名: {', '.join(extensions)}")
    print(f"排除目录: {', '.join(sorted(exclude_dirs))}")
    if args.header:
        print(f"头部文件: {args.header}")
    else:
        print(f"新头部 (内联): {new_header[:80].strip()}...")
    if match_pattern:
        print(f"匹配条件: 旧头部需匹配 /{match_pattern.pattern}/")
    print(f"模式: {'预览 (dry-run)' if args.dry_run else '实际替换'}")
    print(f"风格检测: {'灵活 (多种注释风格)' if args.flexible else '固定 (仅 /// 行注释)'}")
    print()

    # 选择替换函数
    if args.flexible:
        replace_func = lambda fp: replace_flexible_header_in_file(
            fp, new_header, match_copyright_pattern=match_pattern,
            dry_run=args.dry_run, new_author=args.new_author
        )
    else:
        replace_func = lambda fp: replace_header_in_file(
            fp, new_header, dry_run=args.dry_run, new_author=args.new_author
        )

    # 遍历并替换
    modified_count = 0
    skipped_count = 0
    total_count = 0

    print(f"--- 处理文件 ---")
    for file_path in walk_source_files(root_dir, extensions, exclude_dirs):
        total_count += 1
        rel_path = os.path.relpath(file_path, root_dir)
        print(f"[{total_count}] {rel_path}")

        result = replace_func(file_path)
        if result:
            modified_count += 1
        else:
            skipped_count += 1

    print()
    print(f"--- 处理完毕 ---")
    print(f"总文件数: {total_count}")
    print(f"已修改: {modified_count}")
    print(f"已跳过: {skipped_count}")
    if args.dry_run:
        print("注意: 这是预览模式，文件未被实际修改。去掉 --dry-run 参数以执行修改。")


if __name__ == '__main__':
    main()
