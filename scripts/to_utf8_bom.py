import os
import chardet
from pathlib import Path

def is_utf8_bom(file_path):
    """检查文件是否已经是 UTF-8-BOM 编码"""
    try:
        with open(file_path, 'rb') as f:
            bom = f.read(3)
            # UTF-8-BOM 的前三个字节是: EF BB BF
            return bom == b'\xef\xbb\xbf'
    except:
        return False

def convert_to_utf8_bom(file_path, skip_if_already_utf8_bom=True):
    """将单个文件转换为 UTF-8-BOM 编码"""
    try:
        # 检查是否已经是 UTF-8-BOM
        if skip_if_already_utf8_bom and is_utf8_bom(file_path):
            print(f"  已经是 UTF-8-BOM，跳过: {file_path}")
            return False
        
        # 读取文件内容
        with open(file_path, 'rb') as f:
            raw_data = f.read()
        
        if not raw_data:  # 空文件
            print(f"  空文件，跳过: {file_path}")
            return False
        
        # 如果是 UTF-8-BOM 但 skip_if_already_utf8_bom=False，需要去除 BOM
        if raw_data.startswith(b'\xef\xbb\xbf'):
            raw_data = raw_data[3:]  # 去除 BOM
            detected_encoding = 'utf-8-sig'
        else:
            # 检测文件编码
            try:
                detected = chardet.detect(raw_data)
                detected_encoding = detected['encoding']
                confidence = detected['confidence']
                
                # 如果检测不到编码或置信度低，尝试常见编码
                if not detected_encoding or confidence < 0.7:
                    try_encodings = ['utf-8', 'gbk', 'gb2312', 'ascii', 'iso-8859-1', 'cp1252']
                    for enc in try_encodings:
                        try:
                            content = raw_data.decode(enc)
                            detected_encoding = enc
                            break
                        except UnicodeDecodeError:
                            continue
                    else:
                        print(f"  无法确定文件编码，跳过: {file_path}")
                        return False
            except Exception as e:
                print(f"  编码检测失败: {file_path}, 错误: {e}")
                return False
        
        # 解码文件内容
        try:
            if detected_encoding.lower() == 'utf-8-sig':
                content = raw_data.decode('utf-8')
            else:
                content = raw_data.decode(detected_encoding)
        except UnicodeDecodeError as e:
            print(f"  解码失败 ({detected_encoding}): {file_path}")
            print(f"  错误详情: {e}")
            return False
        
        # 确保内容末尾没有多余的空行（保留原始换行符格式）
        # 这里不自动添加或删除换行符，保持原样
        
        # 写入 UTF-8-BOM 格式
        with open(file_path, 'w', encoding='utf-8-sig', newline='') as f:
            f.write(content)
        
        print(f"  成功转换: {file_path} ({detected_encoding} → UTF-8-BOM)")
        return True
        
    except Exception as e:
        print(f"  处理文件时出错: {file_path}")
        print(f"  错误详情: {str(e)[:100]}")  # 只显示前100个字符
        return False

def should_process_file(file_path, extensions=None):
    """判断是否应该处理该文件"""
    file_path = Path(file_path)
    
    # 排除二进制文件和特定扩展名
    binary_extensions = {
        '.exe', '.dll', '.so', '.dylib', '.bin',
        '.jpg', '.jpeg', '.png', '.gif', '.bmp', '.ico', '.svg',
        '.pdf', '.zip', '.rar', '.7z', '.tar', '.gz',
        '.mp3', '.mp4', '.avi', '.mkv', '.mov',
        '.pyc', '.pyo', '.pyd', '.whl', '.egg',
        '.db', '.sqlite', '.mdb',
        '.obj', '.class', '.jar', '.war'
    }
    
    # 获取文件扩展名
    ext = file_path.suffix.lower()
    
    # 如果有指定的扩展名列表，只处理这些扩展名
    if extensions:
        ext_list = [e.lower() for e in extensions]
        if ext not in ext_list:
            return False
    
    # 排除二进制文件扩展名
    if ext in binary_extensions:
        return False
    
    # 检查文件大小，过大文件跳过（可调整阈值）
    try:
        file_size = file_path.stat().st_size
        if file_size > 10 * 1024 * 1024:  # 10MB
            print(f"  文件过大({file_size/1024/1024:.1f}MB)，跳过: {file_path}")
            return False
    except:
        pass
    
    return True

def convert_folder_to_utf8_bom(folder_path, extensions=None, recursive=True):
    """
    转换文件夹下所有文件的编码为 UTF-8-BOM
    
    参数:
        folder_path: 要处理的文件夹路径
        extensions: 要处理的文件扩展名列表，None 表示处理所有文本文件
        recursive: 是否递归处理子文件夹
    """
    folder_path = Path(folder_path)
    
    if not folder_path.exists():
        print(f"错误: 文件夹不存在 - {folder_path}")
        return
    
    if not folder_path.is_dir():
        print(f"错误: 路径不是文件夹 - {folder_path}")
        return
    
    print(f"开始处理文件夹: {folder_path}")
    print("-" * 50)
    
    # 统计信息
    total_files = 0
    converted_files = 0
    skipped_files = 0
    error_files = 0
    already_utf8_bom = 0
    
    # 遍历文件
    if recursive:
        file_iter = folder_path.rglob('*')
    else:
        file_iter = folder_path.glob('*')
    
    for file_path in file_iter:
        if not file_path.is_file():
            continue
        
        # 检查是否应该处理该文件
        if not should_process_file(file_path, extensions):
            skipped_files += 1
            continue
        
        total_files += 1
        print(f"[{total_files}] 处理: {file_path}")
        
        # 检查是否已经是 UTF-8-BOM
        if is_utf8_bom(file_path):
            already_utf8_bom += 1
            print(f"  已经是 UTF-8-BOM，跳过: {file_path}")
            continue
        
        try:
            if convert_to_utf8_bom(file_path, skip_if_already_utf8_bom=False):
                converted_files += 1
            else:
                error_files += 1
        except Exception as e:
            print(f"  处理文件时出现异常: {file_path}")
            print(f"  异常详情: {str(e)[:100]}")
            error_files += 1
    
    # 打印统计信息
    print("\n" + "=" * 50)
    print("转换完成！")
    print(f"总扫描文件数: {total_files}")
    print(f"成功转换: {converted_files}")
    print(f"已经是 UTF-8-BOM: {already_utf8_bom}")
    print(f"跳过文件: {skipped_files}")
    print(f"失败文件: {error_files}")

def preview_conversion(folder_path, extensions=None):
    """
    预览转换，不实际修改文件
    """
    folder_path = Path(folder_path)
    
    if not folder_path.exists() or not folder_path.is_dir():
        print(f"错误: 文件夹不存在 - {folder_path}")
        return
    
    print(f"预览模式 - 不会实际修改文件")
    print(f"扫描文件夹: {folder_path}")
    print("-" * 50)
    
    # 统计信息
    total_files = 0
    would_convert = 0
    already_utf8_bom = 0
    skipped_files = 0
    
    # 遍历文件
    for file_path in folder_path.rglob('*'):
        if not file_path.is_file():
            continue
        
        if not should_process_file(file_path, extensions):
            skipped_files += 1
            continue
        
        total_files += 1
        
        # 检查是否已经是 UTF-8-BOM
        if is_utf8_bom(file_path):
            already_utf8_bom += 1
            print(f"[{total_files}] 已经是 UTF-8-BOM: {file_path}")
        else:
            would_convert += 1
            print(f"[{total_files}] 需要转换: {file_path}")
    
    print("\n" + "=" * 50)
    print("预览完成！")
    print(f"总扫描文件数: {total_files}")
    print(f"需要转换的文件: {would_convert}")
    print(f"已经是 UTF-8-BOM: {already_utf8_bom}")
    print(f"跳过文件: {skipped_files}")
    
    if would_convert > 0:
        print(f"\n提示: 运行转换脚本将修改 {would_convert} 个文件")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='将文件夹下所有文件转换为 UTF-8-BOM 编码')
    parser.add_argument('folder', help='要处理的文件夹路径')
    parser.add_argument('-e', '--extensions', nargs='+', 
                       help='要处理的文件扩展名（如 .txt .py .html）')
    parser.add_argument('-n', '--non-recursive', action='store_true',
                       help='不递归处理子文件夹')
    parser.add_argument('-p', '--preview', action='store_true',
                       help='预览模式，不实际修改文件')
    
    args = parser.parse_args()
    
    if args.preview:
        preview_conversion(args.folder, args.extensions)
    else:
        convert_folder_to_utf8_bom(
            args.folder, 
            args.extensions, 
            recursive=not args.non_recursive
        )
    
    print("\n提示:")
    print("1. 转换前建议使用 -p 参数预览")
    print("2. 重要文件建议先备份")
    print("3. 转换后可验证几个文件确保无误")

if __name__ == "__main__":
    main()