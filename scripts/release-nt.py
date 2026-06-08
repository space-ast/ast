#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
release to module nt
"""

import os
import re
import zipfile
import shutil

# 项目根目录
ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

# 版本声明正则表达式
# 只匹配文件开头的版权声明，不匹配函数注释
VERSION_HEADER_PATTERN = re.compile(r'\A\s*(///.*(?:\r?\n))+')

# 需要处理的文件扩展名
SOURCE_EXTENSIONS = ['.cpp', '.h', '.hpp', '.c', ".cpp0", ".0cpp"]

# 需要转换编码的目录
ENCODE_DIRS = ['examples', 'test']

# 需要删除的文件
FILES_TO_DELETE = [
    os.path.join(ROOT_DIR, 'README_zh.md'),
    os.path.join(ROOT_DIR, 'README.md')
]

# 压缩包名称
ZIP_NAME = 'ast-nt.zip'

# 排除的目录
EXCLUDE_DIRS = ['data', 'docs', '.git', 'build', '.xmake', ".trae", ".vscode", ".github", 
                ".vs", "thirdparty", "vs2015", "vs2026", "vsxmake2022", "vsxmake2026",
                "node_modules"]

# 空行替换内容（可配置）
EMPTY_LINE_REPLACEMENT = '// 警告：请不要修改此文件，所进行的修改都会被覆盖。为避免丢失，不要修改此文件的代码。'  # 默认替换为空字符串，即删除空行

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
    # 用 utf-8-sig 读取，自动跳过可能存在的 BOM
    with open(file_path, 'r', encoding='utf-8-sig') as f:
        lines = f.readlines()

    # 找到第一个既不是 /// 注释也不是空行的行的索引
    start_idx = 0
    for i, line in enumerate(lines):
        stripped = line.lstrip()  # 去除行首空白，便于判断
        if stripped.startswith('///') or stripped == '' or stripped.isspace():
            # 注释行或空行，继续跳过
            continue
        else:
            # 遇到有效代码行，从此处开始保留
            start_idx = i
            break
    else:
        # 如果整个文件都是注释/空行，则保留原文件不变（防止清空文件）
        print(f"All lines are comments/empty, skipping: {file_path}")
        return

    # 如果有被跳过的行，则写回文件（使用无 BOM 的 UTF-8 编码）
    if start_idx > 0:
        new_lines = lines[start_idx:]
        with open(file_path, 'w', encoding='utf-8') as f:
            f.writelines(new_lines)
        print(f"Removed version header from: {file_path}")

def convert_to_utf8_bom(file_path):
    """将文件转换为utf8-bom编码"""
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    with open(file_path, 'w', encoding='utf-8-sig') as f:
        f.write(content)
    print(f"Converted to utf8-bom: {file_path}")

def replace_empty_lines(file_path):
    """替换文件中的空行为指定内容"""
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
    
    new_lines = []
    for line in lines:
        if line.strip() == '':
            # 空行，替换为指定内容
            if EMPTY_LINE_REPLACEMENT:
                new_lines.append(EMPTY_LINE_REPLACEMENT + '\n')
            # 如果替换内容为空，则不添加该行
        else:
            new_lines.append(line)
    
    with open(file_path, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)
    print(f"Replaced empty lines in: {file_path}")

def process_files():
    """处理所有文件"""
    for root, dirs, files in os.walk(ROOT_DIR):
        # 跳过不需要处理的目录
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        
        for file in files:
            file_path = os.path.join(root, file)
            ext = os.path.splitext(file)[1]
            
            # 删除版本声明
            if ext in SOURCE_EXTENSIONS:
                remove_version_header(file_path)
            
            # 转换编码
            if any(encode_dir in root for encode_dir in ENCODE_DIRS) and ext in SOURCE_EXTENSIONS:
                convert_to_utf8_bom(file_path)
            
            # 替换空行（只处理.hpp和.cpp文件）
            if ext in ['.cpp']:
                replace_empty_lines(file_path)

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

def main():
    """主函数"""
    print("Starting release process...")
    
    # 处理文件
    process_files()
    
    # 删除指定文件
    delete_files()

    # 合并模块源文件
    merge_module_sources()
    
    
    # 创建压缩包
    create_zip()
    
    # 复原所有被修改的文件，但排除脚本本身
    import subprocess
    print("Restoring modified files...")
    # 首先获取脚本的相对路径
    script_rel_path = os.path.relpath(__file__, ROOT_DIR)
    # 执行git checkout，排除脚本本身
    subprocess.run(['git', 'add', script_rel_path], cwd=ROOT_DIR, check=True)
    subprocess.run(['git', 'checkout', '--', '.'], cwd=ROOT_DIR, check=True)
    restore_module_sources()
    print("Files restored successfully!")

    # 统计代码行数
    count_code_lines()
    
    print("Release process completed!")

if __name__ == '__main__':
    main()