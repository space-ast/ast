#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
release to module nt
"""

import os
import re
import zipfile
import shutil
import subprocess
import argparse
import stat

# 项目根目录
ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

# 版本声明正则表达式
# 只匹配文件开头的版权声明，不匹配函数注释
VERSION_HEADER_PATTERN = re.compile(r'\A(\s*///[^\n]*\n?)+')

# 需要处理的文件扩展名
SOURCE_EXTENSIONS = ['.cpp', '.h', '.hpp', '.c', ".cxx", ".inl", ".cpp0", ".0cpp", ".bak", ".cpp.bak"]

# 需要转换编码的目录
ENCODE_DIRS = ['examples', 'test']

# 需要删除的文件
FILES_TO_DELETE = [
    os.path.join(ROOT_DIR, 'README_zh.md'),
    os.path.join(ROOT_DIR, 'README.md'),
    os.path.join(ROOT_DIR, 'src/README.dox'),
    os.path.join(ROOT_DIR, 'test/Util/testIO.cpp'),
    os.path.join(ROOT_DIR, "xpack.lua"),
    os.path.join(ROOT_DIR, "doxyfile"),
    os.path.join(ROOT_DIR, ".gitmodules")
]

# 压缩包名称
ZIP_NAME = 'ast-nt.zip'

# 排除的目录
EXCLUDE_DIRS = ['data', "doc", 'docs', 'build', "thirdparty", "artifacts",
                ".claude", ".github", ".trae", ".vs", ".vscode", '.xmake', '.git', 
                "__pycache__", 
                "vs2015", "vs2026", "vsxmake2022", "vsxmake2026",
                "node_modules"]

# 空行替换内容（可配置）
EMPTY_LINE_REPLACEMENT = '// 警告：请不要修改此文件，所进行的修改都会被覆盖。为避免丢失，不要修改此文件的代码。'  # 默认替换为空字符串，即删除空行


def ensure_writable(file_path):
    """确保文件可写（Windows 下清除只读属性）"""
    if not os.access(file_path, os.W_OK):
        os.chmod(file_path, stat.S_IWRITE | stat.S_IREAD)


# 合并模块源文件
def merge_module_sources():
    """合并每个模块的.cpp文件为一个文件"""
    src_dir = os.path.join(ROOT_DIR, 'src')
    
    # 遍历src目录下的每个模块
    for module_name in os.listdir(src_dir):
        module_path = os.path.join(src_dir, module_name)
        
        # 只处理目录
        if not os.path.isdir(module_path):
            continue
        
        # 收集该模块下的所有.cpp文件
        cpp_files = []
        for root, dirs, files in os.walk(module_path):
            for file in files:
                if file.endswith('.cpp'):
                    cpp_files.append(os.path.join(root, file))
        
        cpp_files.sort()
        # 如果有.cpp文件，合并为一个文件
        if cpp_files:
            merged_file_path = os.path.join(module_path, f'{module_name}.cpp')
            print(f"Merging {len(cpp_files)} .cpp files in {module_name} to {merged_file_path}")
            
            # 合并文件内容
            with open(merged_file_path, 'w', encoding='utf-8-sig') as merged_file:
                for cpp_file in cpp_files:
                    with open(cpp_file, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                        # 写入文件内容，添加文件分隔符
                        merged_file.write(f"// ====== {os.path.relpath(cpp_file, module_path).replace("\\", "/")} ======\n")
                        merged_file.write(content)
                        merged_file.write('\n\n')
            
            # 删除原.cpp文件
            for cpp_file in cpp_files:
                os.remove(cpp_file)
                print(f"Deleted original file: {cpp_file}")

# 恢复模块源文件
def restore_module_sources():
    """恢复被合并的模块源文件"""
    src_dir = os.path.join(ROOT_DIR, 'src')
    
    # 遍历src目录下的每个模块
    for module_name in os.listdir(src_dir):
        module_path = os.path.join(src_dir, module_name)
        
        # 只处理目录
        if not os.path.isdir(module_path):
            continue
        
        # 检查是否存在合并后的文件
        merged_file_path = os.path.join(module_path, f'{module_name}.cpp')
        if os.path.exists(merged_file_path):
            os.remove(merged_file_path)
            print(f"Removed merged file: {merged_file_path}")

def remove_version_header(file_path):
    """删除文件开头的连续注释块（以 /// 开头）以及中间的空行"""
    with open(file_path, 'r', encoding='utf-8-sig') as f:
        content = f.read()

    match = VERSION_HEADER_PATTERN.match(content)
    if not match:
        return

    new_content = content[match.end():]
    if not new_content:
        # 整个文件都是注释/空行，保留原文件不变
        print(f"All lines are comments/empty, skipping: {file_path}")
        return

    ensure_writable(file_path)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print(f"Removed version header from: {file_path}")

def convert_to_utf8_bom(file_path):
    """将文件转换为utf8-bom编码"""
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    ensure_writable(file_path)
    with open(file_path, 'w', encoding='utf-8-sig') as f:
        f.write(content)
    print(f"Converted to utf8-bom: {file_path}")

# C++ 原始字符串字面量正则：R"delimiter(content)delimiter"
RAW_STRING_PATTERN = re.compile(r'R"([^(\s]*)\(.*?\)\1"', re.DOTALL)


def replace_empty_lines(file_path):
    """替换文件中的空行为指定内容，跳过原始字符串字面量内部的空行"""
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    # 找出所有原始字符串字面量的起止位置
    raw_spans = [(m.start(), m.end()) for m in RAW_STRING_PATTERN.finditer(content)]

    lines = content.splitlines(keepends=True)
    new_lines = []
    pos = 0

    for line in lines:
        line_start = pos
        line_end = pos + len(line)
        pos = line_end

        if line.strip() == '':
            # 检查该空行是否位于某个原始字符串字面量内部
            in_raw = any(line_start >= start and line_end <= end
                         for start, end in raw_spans)
            if not in_raw:
                if EMPTY_LINE_REPLACEMENT:
                    new_lines.append(EMPTY_LINE_REPLACEMENT + '\n')
                # 如果替换内容为空，则不添加该行
                continue

        new_lines.append(line)

    ensure_writable(file_path)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)
    print(f"Replaced empty lines in: {file_path}")

def remove_version_headers():
    """遍历所有源文件，删除文件开头的版本声明注释块"""
    for root, dirs, files in os.walk(ROOT_DIR):
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        for file in files:
            if os.path.splitext(file)[1] in SOURCE_EXTENSIONS:
                remove_version_header(os.path.join(root, file))


def convert_encoding_to_utf8_bom():
    """将所有源文件转换为 UTF-8-BOM 编码"""
    for root, dirs, files in os.walk(ROOT_DIR):
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        for file in files:
            if os.path.splitext(file)[1] in SOURCE_EXTENSIONS:
                convert_to_utf8_bom(os.path.join(root, file))


def replace_empty_lines_in_cpp():
    """遍历所有 .cpp 文件，将空行替换为警告注释"""
    for root, dirs, files in os.walk(ROOT_DIR):
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        for file in files:
            if file.endswith('.cpp'):
                replace_empty_lines(os.path.join(root, file))

def delete_files():
    """删除指定文件"""
    for file_path in FILES_TO_DELETE:
        if os.path.exists(file_path):
            os.remove(file_path)
            print(f"Deleted file: {file_path}")

def create_zip():
    """创建压缩包"""
    zip_path = os.path.join(ROOT_DIR, ZIP_NAME)
    
    with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for root, dirs, files in os.walk(ROOT_DIR):
            # 跳过不需要压缩的目录
            dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
            
            for file in files:
                file_path = os.path.join(root, file)
                # 计算相对路径
                arcname = os.path.relpath(file_path, ROOT_DIR)
                # 跳过压缩包本身
                if arcname == ZIP_NAME:
                    continue
                zipf.write(file_path, arcname)
                print(f"Added to zip: {arcname}")
    
    print(f"Created zip file: {zip_path}")

def count_code_lines():
    """统计src目录下每个模块的代码行数，分为总行数、头文件行数、源文件行数"""
    src_dir = os.path.join(ROOT_DIR, 'src')
    total_lines = 0
    total_lines_header = 0
    total_lines_source = 0
    total_files = 0
    module_stats = {}
    
    print("\nCounting code lines in src directory...")
    
    for root, dirs, files in os.walk(src_dir):
        # 跳过不需要统计的目录
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        
        # 确定当前模块名称
        rel_path = os.path.relpath(root, src_dir)
        if rel_path == '.':
            # 处理src根目录下的文件
            for file in files:
                if file.endswith('.hpp') or file.endswith('.cpp'):
                    file_path = os.path.join(root, file)
                    try:
                        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                            lines = len(f.readlines())
                            total_lines += lines
                            total_files += 1
                            if file.endswith('.hpp'):
                                total_lines_header += lines
                            else:
                                total_lines_source += lines
                    except Exception as e:
                        print(f"Error reading {file_path}: {e}")
            continue
        
        # 获取模块名称（第一个目录）
        module_name = rel_path.split(os.sep)[0]
        
        # 初始化模块统计数据
        if module_name not in module_stats:
            module_stats[module_name] = {
                'total_lines': 0,
                'header_lines': 0,
                'source_lines': 0,
                'total_files': 0,
                'header_files': 0,
                'source_files': 0
            }
        
        for file in files:
            if file.endswith('.hpp') or file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        lines = len(f.readlines())
                        total_lines += lines
                        total_files += 1
                        if file.endswith('.hpp'):
                            total_lines_header += lines
                        else:
                            total_lines_source += lines
                        
                        # 更新模块统计数据
                        module_stats[module_name]['total_lines'] += lines
                        module_stats[module_name]['total_files'] += 1
                        if file.endswith('.hpp'):
                            module_stats[module_name]['header_lines'] += lines
                            module_stats[module_name]['header_files'] += 1
                        elif file.endswith('.cpp'):
                            module_stats[module_name]['source_lines'] += lines
                            module_stats[module_name]['source_files'] += 1
                except Exception as e:
                    print(f"Error reading {file_path}: {e}")
    
    # 打印每个模块的统计结果
    print("\nModule code line statistics:")
    print("-" * 100)
    print(f"{'Module':<20} {'Total Files':<12} {'Header Files':<12} {'Source Files':<12} {'Total Lines':<12} {'Header Lines':<12} {'Source Lines':<12}")
    print("-" * 100)
    
    for module_name, stats in sorted(module_stats.items()):
        print(f"{module_name:<20} {stats['total_files']:<12} {stats['header_files']:<12} {stats['source_files']:<12} {stats['total_lines']:<12} {stats['header_lines']:<12} {stats['source_lines']:<12}")
    
    print("-" * 100)
    print(f"{'Total':<20} {total_files:<12} {'-':<12} {'-':<12} {total_lines:<12} {total_lines_header:<12} {total_lines_source:<12}")
    
    return total_lines

def restore_all_files():
    """通过 git checkout 复原所有被修改的文件，并清理合并产生的临时文件"""
    script_rel_path = os.path.relpath(__file__, ROOT_DIR)
    subprocess.run(['git', 'add', script_rel_path], cwd=ROOT_DIR, check=True)
    subprocess.run(['git', 'checkout', '--', '.'], cwd=ROOT_DIR, check=True)
    restore_module_sources()
    print("Files restored successfully!")


def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='Release to module nt')
    parser.add_argument('--no-merge', action='store_false', dest='merge', default=True,
                        help='不合并模块源文件')
    parser.add_argument('--no-replace-empty', action='store_false', dest='replace_empty', default=True,
                        help='不替换 .cpp 文件中的空行')
    args = parser.parse_args()

    print("Starting release process...")

    print("Deleting specified files...")
    delete_files()

    print("Removing version headers...")
    remove_version_headers()

    if args.replace_empty:
        print("Replacing empty lines in .cpp files...")
        replace_empty_lines_in_cpp()

    if args.merge:
        print("Merging module sources...")
        merge_module_sources()

    print("Converting encoding to UTF-8-BOM...")
    convert_encoding_to_utf8_bom()

    print("Creating zip archive...")
    create_zip()

    print("Restoring modified files...")
    restore_all_files()

    print("Counting code lines...")
    count_code_lines()

    print("Release process completed!")

if __name__ == '__main__':
    main()