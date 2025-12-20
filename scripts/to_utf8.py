#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import codecs
import chardet
import argparse
from pathlib import Path

def convert_file_to_utf8(file_path, output_encoding='utf-8', backup=False):
    """
    将单个文件转换为UTF-8编码
    
    Args:
        file_path: 文件路径
        output_encoding: 输出编码，默认为utf-8
        backup: 是否创建备份文件
    """
    try:
        # 读取文件二进制内容
        with open(file_path, 'rb') as f:
            raw_data = f.read()
        
        # 检测文件编码
        result = chardet.detect(raw_data)
        original_encoding = result['encoding']
        confidence = result['confidence']
        
        # 如果检测不到编码或置信度太低，尝试常见编码
        if not original_encoding or confidence < 0.5:
            # 尝试常见编码列表
            encodings_to_try = ['utf-8', 'gbk', 'gb2312', 'gb18030', 'big5', 
                               'latin-1', 'iso-8859-1', 'cp1252', 'ascii']
            
            for enc in encodings_to_try:
                try:
                    # 尝试解码
                    content = raw_data.decode(enc, errors='strict')
                    original_encoding = enc
                    break
                except UnicodeDecodeError:
                    continue
            
            # 如果所有编码都失败，使用检测结果或默认编码
            if not original_encoding:
                original_encoding = result['encoding'] or 'utf-8'
        
        # 如果原始编码已经是目标编码，跳过
        if original_encoding.lower() in ['utf-8', 'utf8'] and output_encoding.lower() in ['utf-8', 'utf8']:
            print(f"✓ {file_path}: 已经是UTF-8编码，跳过")
            return True
        
        # 解码原始内容
        try:
            content = raw_data.decode(original_encoding, errors='replace')
        except (UnicodeDecodeError, LookupError) as e:
            print(f"✗ {file_path}: 解码失败 ({original_encoding}): {e}")
            return False
        
        # 如果需要备份，创建备份文件
        if backup:
            backup_path = file_path + '.bak'
            try:
                with open(backup_path, 'wb') as f:
                    f.write(raw_data)
                print(f"✓ {file_path}: 已创建备份文件 {backup_path}")
            except Exception as e:
                print(f"✗ {file_path}: 创建备份文件失败: {e}")
        
        # 写入新编码的内容
        try:
            with codecs.open(file_path, 'w', encoding=output_encoding, errors='replace') as f:
                f.write(content)
            
            # 获取原始文件权限
            original_stat = os.stat(file_path)
            os.chmod(file_path, original_stat.st_mode)
            
            print(f"✓ {file_path}: 成功从 {original_encoding} 转换为 {output_encoding}")
            return True
        except Exception as e:
            print(f"✗ {file_path}: 写入文件失败: {e}")
            return False
            
    except Exception as e:
        print(f"✗ {file_path}: 处理文件时出错: {e}")
        return False

def convert_files_in_directory(directory, extensions, output_encoding='utf-8', recursive=True, backup=False):
    """
    转换目录下的所有指定扩展名的文件
    
    Args:
        directory: 目录路径
        extensions: 文件扩展名列表
        output_encoding: 输出编码
        recursive: 是否递归处理子目录
        backup: 是否创建备份文件
    """
    directory = Path(directory)
    
    if not directory.exists():
        print(f"错误: 目录不存在: {directory}")
        return
    
    # 收集所有符合条件的文件
    files_to_convert = []
    
    if recursive:
        # 递归查找所有文件
        for ext in extensions:
            files_to_convert.extend(directory.rglob(f"*{ext}"))
    else:
        # 仅查找当前目录
        for ext in extensions:
            files_to_convert.extend(directory.glob(f"*{ext}"))
    
    if not files_to_convert:
        print(f"警告: 在 {directory} 中未找到指定扩展名的文件")
        return
    
    print(f"找到 {len(files_to_convert)} 个需要转换的文件")
    print("=" * 60)
    
    # 统计转换结果
    success_count = 0
    fail_count = 0
    
    # 处理每个文件
    for file_path in files_to_convert:
        if convert_file_to_utf8(file_path, output_encoding, backup):
            success_count += 1
        else:
            fail_count += 1
    
    print("=" * 60)
    print(f"转换完成: 成功 {success_count} 个, 失败 {fail_count} 个")

def main():
    parser = argparse.ArgumentParser(
        description='将C/C++源文件的编码转换为UTF-8',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
示例:
  %(prog)s /path/to/project
  %(prog)s /path/to/project --recursive
  %(prog)s /path/to/project --output gbk --backup
  %(prog)s /path/to/project --no-recursive --extensions .cpp .h
        '''
    )
    
    parser.add_argument('directory', help='要处理的目录路径')
    parser.add_argument('--output', '-o', default='utf-8',
                       help='目标编码 (默认: utf-8)')
    parser.add_argument('--extensions', '-e', nargs='+',
                       default=['.cpp', '.hpp', '.c', '.h'],
                       help='要处理的文件扩展名 (默认: .cpp .hpp .c .h)')
    parser.add_argument('--no-recursive', action='store_false', dest='recursive',
                       help='不递归处理子目录')
    parser.add_argument('--backup', '-b', action='store_true',
                       help='创建备份文件 (.bak)')
    parser.add_argument('--dry-run', action='store_true',
                       help='只显示将要处理的文件，不实际转换')
    
    args = parser.parse_args()
    
    # 确保扩展名以点开头
    extensions = []
    for ext in args.extensions:
        if not ext.startswith('.'):
            ext = '.' + ext
        extensions.append(ext)
    
    if args.dry_run:
        # 只显示文件列表
        directory = Path(args.directory)
        if directory.exists():
            files_to_convert = []
            
            if args.recursive:
                for ext in extensions:
                    files_to_convert.extend(directory.rglob(f"*{ext}"))
            else:
                for ext in extensions:
                    files_to_convert.extend(directory.glob(f"*{ext}"))
            
            if files_to_convert:
                print(f"将处理以下 {len(files_to_convert)} 个文件:")
                for file_path in files_to_convert:
                    print(f"  {file_path}")
            else:
                print("未找到符合条件的文件")
        else:
            print(f"错误: 目录不存在: {args.directory}")
    else:
        # 实际转换文件
        convert_files_in_directory(
            directory=args.directory,
            extensions=extensions,
            output_encoding=args.output,
            recursive=args.recursive,
            backup=args.backup
        )

if __name__ == "__main__":
    main()