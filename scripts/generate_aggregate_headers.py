#!/usr/bin/env python3
"""
生成聚合头文件的脚本
为src目录下的每个工程生成一个聚合头文件
注意：聚合头文件将包含include目录下的转发头文件
"""

import os
from pathlib import Path


def find_forward_header_files(include_dir, project_name):
    """查找include目录下该工程的所有转发头文件"""
    project_include_dir = include_dir / project_name
    if not project_include_dir.exists():
        return []
    
    header_files = []
    for file in project_include_dir.iterdir():
        if file.is_file() and file.suffix in ['.hpp', '.h']:
            header_files.append(file.name)
    
    return sorted(header_files)


def generate_aggregate_header(project_name, header_files):
    """生成聚合头文件内容"""
    content = f'// {project_name} 工程聚合头文件\n'
    
    for header in header_files:
        # 排除聚合头文件自身
        if header != f"{project_name}.hpp":
            content += f'#include "{project_name}/{header}"\n'
    
    return content


def main():
    # 获取src目录和include目录路径
    src_dir = Path("src")
    include_dir = Path("include")
    
    # 获取所有子目录（工程）
    projects = [d for d in src_dir.iterdir() if d.is_dir()]
    
    for project in projects:
        project_name = project.name
        
        print(f"处理工程: {project_name}")
        
        # 查找include目录下该工程的所有转发头文件
        header_files = find_forward_header_files(include_dir, project_name)
        
        if not header_files:
            print(f"  在include/{project_name}/目录下没有找到转发头文件，跳过...")
            continue
        
        print(f"  找到 {len(header_files)} 个转发头文件")
        
        # 生成聚合头文件内容
        aggregate_content = generate_aggregate_header(project_name, header_files)
        
        # 确定聚合头文件的路径
        aggregate_header_path = project / f"{project_name}.hpp"
        
        # 写入聚合头文件
        with open(aggregate_header_path, 'w', encoding='utf-8') as f:
            f.write(aggregate_content)
        
        print(f"  已生成聚合头文件: {aggregate_header_path}")
    
    print("\n所有工程的聚合头文件已生成完成！")


if __name__ == "__main__":
    main()