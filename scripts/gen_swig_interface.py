import re
from pathlib import Path
from collections import defaultdict, deque

def filter_func_for_swig(header):
    exclude_list = [
        "AstCOM", "AstGUI", "AstGfx", "AstAnalyzer", "AstTest", "AstWasm",
        "\\ScStateCalculation\\",
        r"Platform\Posix.hpp",
        r"Array\MathOperator.hpp", 
        r"Util\Literals.hpp", r"Util\TypeTraits.hpp"
    ]
    if any(file in header for file in exclude_list):
        return False
    include_list = [
        "AstUtil", "AstMath", "AstScript", "AstCore", "AstSim", "AstGlobal", "AstCompiler"
    ]
    if any(file in header for file in include_list):
        return True
    return False



def resolve_forward_header(forward_header_path):
    """
    解析转发头文件，返回其实际指向的目标文件路径
    
    Args:
        forward_header_path (Path): 转发头文件路径
    
    Returns:
        Path: 实际的目标文件路径，如果解析失败则返回原路径
    """
    try:
        with open(forward_header_path, 'r', encoding='utf-8') as f:
            content = f.read().strip()
        
        line_count = len(content.strip().splitlines())
        

        # 匹配 #include "../../src/..." 这样的模式
        pattern = r'#\s*include\s+"([^"]+)"'
        matches = re.findall(pattern, content)

        if line_count > 1 or not matches:
            if not forward_header_path.exists():
                raise FileNotFoundError(f"Forward header does not exist: {forward_header_path}")
            else:
                return forward_header_path
        
        if matches:
            # 获取第一个包含的路径
            relative_path = matches[0]
            # 将相对路径转换为实际路径
            actual_path = (forward_header_path.parent / relative_path).resolve()
            
            if not actual_path.exists():
                raise FileNotFoundError(f"Forward header points to non-existent file: {actual_path}")
            
            return resolve_forward_header(actual_path)
        else:
            # 如果没有找到 #include 语句，返回原路径
            return forward_header_path
    except Exception:
        # 如果解析过程中出现错误，返回原路径
        return forward_header_path


def analyze_header_dependencies(header_path, project_include_dir=None):
    """
    分析头文件的直接依赖头文件
    
    Args:
        header_path (str): 头文件路径
        project_include_dir (str, optional): 项目include目录路径，默认为当前项目根目录下的include
    
    Returns:
        list: 包含直接依赖头文件路径的列表，只返回存在于项目include目录下的头文件
    """
    header_path = Path(header_path)
    
    # 检查文件是否存在
    if not header_path.exists():
        raise FileNotFoundError(f"Header file does not exist: {header_path}")
    
    # 设置默认的项目include目录
    if project_include_dir:
        project_include_dir = Path(project_include_dir)
    
        # 确保include目录存在
        if not project_include_dir.exists():
            raise FileNotFoundError(f"Project include directory does not exist: {project_include_dir}")
    
    
    # 读取头文件内容
    with open(header_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # 正则表达式匹配 #include "..." 形式的包含指令
    # 这种形式表示本地头文件，而非系统头文件
    pattern = r'#\s*include\s+"([^"]+)"'

    # 查找所有匹配项
    matches = re.findall(pattern, content)

    # 获取依赖文件的绝对路径（相对于当前头文件所在目录）
    dependencies = []

    for dep in matches:
        # 构造依赖文件的完整路径
        dep_path = (header_path.parent / dep).resolve()
        
        # 检查依赖文件是否存在
        if dep_path.exists():
            forward_header_path = resolve_forward_header(dep_path)
            dependencies.append(str(forward_header_path))
        elif project_include_dir:  # 如果在同目录找不到，尝试在项目include目录中查找
            include_dep_path = (project_include_dir / dep).resolve()
            if include_dep_path.exists():
                forward_header_path = resolve_forward_header(include_dep_path)
                dependencies.append(str(forward_header_path))

    return dependencies


def find_all_header_files(src_dir):
    """
    递归查找src目录中的所有头文件
    
    Args:
        src_dir (str): 源代码目录路径
    
    Returns:
        list: 包含所有头文件路径的列表
    """
    src_path = Path(src_dir)
    header_extensions = ['.hpp']
    
    headers = []
    for ext in header_extensions:
        headers.extend(list(src_path.rglob(f'*{ext}')))
    
    return [str(h.resolve()) for h in headers]


def topological_sort(headers, dependency_func):
    """
    使用拓扑排序对头文件进行排序，以确保依赖关系正确
    
    Args:
        headers (list): 头文件路径列表
        dependency_func (function): 用于获取头文件依赖的函数
    
    Returns:
        list: 按依赖顺序排列的头文件路径列表
    """
    # 构建依赖图 - 被依赖的节点指向依赖它的节点
    
    headers = set(headers)

    graph = defaultdict(list)  # 被依赖的 -> 依赖它的
    dependenciesmap = {}
    for header in headers:
        dependenciesmap[header] = dependency_func(header)
    

    headers = list(headers)
    headers.sort()
    
    in_degree = {header: 0 for header in headers}  # 入度: 这里表示每个头文件的直接依赖它的数量
    out_degree = {header: 0 for header in headers} # 出度: 这里表示每个头文件的依赖数量
    
    # 计算每个头文件的依赖关系
    for header in headers:
        try:
            if header in dependenciesmap:
                dependencies = dependenciesmap[header]  # 获取 header 依赖的文件列表
            else:
                dependencies = dependency_func(header)
            for dep in dependencies:
                # 只考虑在我们的头文件列表中的依赖
                if dep in in_degree:
                    # 被依赖的文件指向依赖它的文件
                    # 例如：如果 A 依赖 B，则 B -> A
                    graph[dep].append(header)
                    in_degree[dep] += 1
                    out_degree[header] += 1
        except Exception as e:
            print(f"Warning: Could not analyze dependencies for {header}: {e}")
            continue
    
    # 拓扑排序 (Kahn's algorithm)
    # 出度为0的节点是没有任何依赖的节点
    queue = deque([header for header in headers if out_degree[header] == 0])
    result = []
    
    while len(result) != len(headers):
        # Kahn 阶段1: 从出度为0的节点开始处理
        while queue:
            current = queue.popleft()
            result.append(current)
            
            # 当前节点被处理后，更新其邻居的入度
            for neighbor in graph[current]:
                out_degree[neighbor] -= 1
                in_degree[current] -= 1
                if out_degree[neighbor] == 0:
                    queue.append(neighbor)
        remaining = [h for h in headers if h not in result]
        if len(remaining) == 0:
            break
        # 存在循环依赖
        print("Warning: Detected circular dependencies in header files.")
        # 找到最大入度的节点和最小出度的节点
        min_out_degree = min(out_degree[h] for h in remaining)
        candidates = [h for h in remaining if out_degree[h] == min_out_degree]
        candidates.sort(key = lambda x: in_degree[x], reverse=True)
        
        chosen = None
        for candidate in candidates:
            # 检查候选节点是否在循环链中
            if is_node_in_cycle(candidate, graph):
                chosen = candidate
                break
        
        if chosen is None:
            chosen = candidates[0]  # 如果没找到明确的循环节点，选择第一个候选节点
        
        # 强制打破循环依赖的边
        out_degree[chosen] = 0
        queue.append(chosen)

    return result


def group_headers_by_module(headers):
    """
    按模块对头文件进行分组
    
    Args:
        headers (list): 头文件路径列表
    
    Returns:
        dict: 模块名作为键，头文件列表作为值的字典
    """
    modules = defaultdict(list)
    
    for header in headers:
        path_parts = Path(header).parts
        # 查找src之后的第一个目录作为模块名
        try:
            src_index = path_parts.index('src')
            if src_index + 1 < len(path_parts):
                module = path_parts[src_index + 1]
                modules[module].append(header)
        except ValueError:
            # 如果路径中没有src，将其归类为其他
            modules['Global'].append(header)
    
    return dict(modules)


def generate_swig_interface_files(headers, output_dir):
    """
    为每个模块生成SWIG接口文件(.i)
    
    Args:
        headers (dict): 按模块分组的头文件字典
        output_dir (str): 输出目录路径
    """
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)

    if isinstance(headers, dict):
        grouped_headers = headers
    
        for module, headers in grouped_headers.items():
            if not headers:
                continue

            # 生成SWIG接口文件
            swig_file_path = output_path / f"{module}.i"

            with open(swig_file_path, 'w', encoding='utf-8') as f:
                f.write("// Auto-generated SWIG interface file\n")
                f.write(f"// Module: {module}\n")
                f.write("// This file was auto-generated by analyze_header_deps.py\n")
                f.write("\n")

                # 写入头文件包含指令
                for header in headers:
                    # 计算相对于AstPy目录的路径
                    header_path = Path(header)
                    try:
                        # 计算从AstPy目录到头文件的相对路径
                        src_dir = Path(r"src").resolve()
                        rel_path = header_path.relative_to(src_dir)
                        # 将路径分隔符统一为正斜杠
                        swig_rel_path = f"../{rel_path}".replace('\\', '/')

                       
                    except ValueError:
                        include_dir = Path(project_include_dir).resolve()
                        rel_path = header_path.relative_to(include_dir)
                        # 将路径分隔符统一为正斜杠
                        swig_rel_path = f"../../include/{rel_path}".replace('\\', '/')

                    # 根据文件类型决定使用%include还是%import
                    if header.endswith(('.i',)):
                        directive = "%include"
                    else:
                        directive = "%include"

                    f.write(f'{directive} "{swig_rel_path}"\n')
                f.write("\n")

            print(f"已生成 SWIG 接口文件: {swig_file_path}")
    else:
        # 同时生成一个总的接口文件
        all_headers_path = output_path / "AstAllHeaders.i"
        with open(all_headers_path, 'w', encoding='utf-8') as f:
            f.write("// Auto-generated SWIG interface file\n")
            f.write("// Combined headers from all modules\n")
            f.write("// This file was auto-generated by analyze_header_deps.py\n")
            f.write("\n")

            all_headers_sorted = headers
            

            # 按照拓扑排序的顺序写入
            for header in all_headers_sorted:
                header_path = Path(header)
                try:
                    src_dir = Path(r"src").resolve()
                    rel_path = header_path.relative_to(src_dir)
                    # 将路径分隔符统一为正斜杠
                    swig_rel_path = f"../{rel_path}".replace('\\', '/')

                    # 确定使用哪个SWIG指令
                    directive = "%include"

                except ValueError:
                    include_dir = Path(project_include_dir).resolve()
                    rel_path = header_path.relative_to(include_dir)
                    # 将路径分隔符统一为正斜杠
                    swig_rel_path = f"../../include/{rel_path}".replace('\\', '/')

                    # 确定使用哪个SWIG指令
                    directive = "%include"

                f.write(f'{directive} "{swig_rel_path}"\n')

            f.write("\n")

        print(f"已生成总接口文件: {all_headers_path}")


def analyze_project_headers(src_dir, project_include_dir=None):
    """
    分析整个项目的头文件及其依赖关系
    
    Args:
        src_dir (str): 源代码目录路径
        project_include_dir (str, optional): 项目include目录路径
    
    Returns:
        list: 按依赖顺序排列的头文件路径列表
    """
    print(f"正在扫描目录: {src_dir}")
    
    # 查找所有头文件
    all_headers = find_all_header_files(src_dir)
    print(f"找到 {len(all_headers)} 个头文件")

    all_headers.extend([str(Path("include/AstGlobal.h").resolve()), str(Path("include/AstCompiler.h").resolve())])
    
    if not all_headers:
        return []
    
    # 按拓扑顺序排序头文件
    print("正在分析依赖关系并进行拓扑排序...")
    sorted_headers = topological_sort(all_headers, 
                                     lambda h: analyze_header_dependencies(h, project_include_dir))
    
    return sorted_headers


def is_node_in_cycle(start_node, graph):
    """
    检查节点是否在一个循环依赖链中
    
    Args:
        start_node: 要检查的节点
        graph: 依赖图 (被依赖的 -> 依赖它的)
    
    Returns:
        bool: 如果节点在循环链中返回True，否则返回False
    """
    # 根据前面的代码，graph[dep] 包含所有依赖dep的节点
    # 即如果 A 依赖 B，则 graph[B] 包含 A
    # 为了找到循环，我们从start_node开始，按照依赖关系向前走
    # 即如果当前节点是X，我们要找所有X依赖的节点Y（即满足graph[Y]包含X的Y）
    
    
    # 使用DFS来检测从start_node开始是否有循环
    visited = set()
    rec_stack = set()
    
    def dfs(current_node):
        visited.add(current_node)
        rec_stack.add(current_node)
        
        # 获取当前节点直接依赖的节点
        deps = graph.get(current_node, [])
        
        for dep_node in deps:
            if dep_node not in visited:
                if dfs(dep_node):
                    return True
            elif dep_node in rec_stack:
                # 发现了回边，说明存在循环
                return True
        
        rec_stack.remove(current_node)
        return False
    
    return dfs(start_node)



# 示例用法
if __name__ == "__main__":
    # 分析整个项目
    src_directory = r"src"
    project_include_dir = r"include"
    
    sorted_headers = analyze_project_headers(src_directory, project_include_dir)
    
    print(f"\n按依赖顺序排列的所有头文件 ({len(sorted_headers)} 个):")
    print("="*60)
    for i, header in enumerate(sorted_headers, 1):
        print(f"{i:4d}. {header}")
    
    print(f"\n按模块分组的头文件:")
    print("="*60)
    grouped_headers = group_headers_by_module(sorted_headers)
    
    for module, headers in grouped_headers.items():
        print(f"\n{module} ({len(headers)} 个头文件):")
        for header in headers:
            print(f"  - {header}")
    
    # 生成SWIG接口文件
    print(f"\n生成SWIG接口文件到 AstPy 目录:")
    print("="*60)
    swig_output_dir = r"src/AstPy"
    # generate_swig_interface_files(grouped_headers, swig_output_dir)
    sorted_headers = list(filter(filter_func_for_swig, sorted_headers))
    generate_swig_interface_files(sorted_headers, swig_output_dir)