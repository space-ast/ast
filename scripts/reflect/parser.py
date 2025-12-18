#!/usr/bin/env python3
"""
使用libclang从C++头文件中提取类型属性元信息
支持多类、多种属性类型
"""

import sys
import os
import re
from dataclasses import dataclass, field
from typing import Dict, List, Set, Optional, Tuple
from enum import Enum
from pathlib import Path

# 尝试导入clang，如果失败则使用简单版本
try:
    from clang.cindex import Index, TranslationUnit, Cursor, CursorKind, TypeKind
except ImportError:
    print("警告: libclang 不可用")
    print("安装: pip install clang")

class PropertyType(Enum):
    """属性类型枚举"""
    DOUBLE = "Double"
    INT = "Int"
    BOOL = "Bool"
    STRING = "String"
    UNKNOWN = "Unknown"

class PropertyInfo:
    """属性信息"""
    
    def __init__(self, name, original_name, type_name, property_type):
        self.name = name                    # 属性名（去掉m_前缀）
        self.original_name = original_name  # 原始成员名
        self.type_name = type_name          # C++类型名
        self.property_type = property_type  # 属性类型
        self.getter = None                  # getter函数名
        self.setter = None                  # setter函数名
        self.is_readonly = False            # 是否只读


    def __repr__(self):
        return (f"PropertyInfo(name={self.name}, cpp_type={self.type_name}, "
                f"prop_type={self.property_type}, getter={self.getter}, setter={self.setter})")

class ClassInfo:
    """类信息"""
    def __init__(self, name, file_path):
        self.name: str = name
        self.file_path:Optional[str] = file_path
        self.properties: Dict[str, PropertyInfo] = {}

    def add_property(self, prop: PropertyInfo):
        """添加属性"""
        self.properties[prop.name] = prop
    
    def get_property(self, name: str) -> Optional[PropertyInfo]:
        """获取属性"""
        return self.properties.get(name)

class TypeMapper:
    """类型映射器"""
    
    # 类型映射：C++类型 -> 属性类型
    TYPE_MAPPING = {
        # double类型
        'double': PropertyType.DOUBLE,
        'length_d': PropertyType.DOUBLE,
        'mass_d': PropertyType.DOUBLE,
        'time_d': PropertyType.DOUBLE,
        'area_d': PropertyType.DOUBLE,
        'speed_d': PropertyType.DOUBLE,
        'force_d': PropertyType.DOUBLE,
        'energy_d': PropertyType.DOUBLE,
        'power_d': PropertyType.DOUBLE,
        'angle_d': PropertyType.DOUBLE,
        'angvel_d': PropertyType.DOUBLE,
        'float': PropertyType.DOUBLE,
        
        # int类型
        'int': PropertyType.INT,
        'err_t': PropertyType.INT,
        'int32_t': PropertyType.INT,
        'int64_t': PropertyType.INT,
        'uint32_t': PropertyType.INT,
        'uint64_t': PropertyType.INT,
        'short': PropertyType.INT,
        'long': PropertyType.INT,
        
        # bool类型
        'bool': PropertyType.BOOL,
        'boolean': PropertyType.BOOL,
        
        # string类型
        'std::string': PropertyType.STRING,
        'string': PropertyType.STRING,
        'std::wstring': PropertyType.STRING,
        'wstring': PropertyType.STRING,
        'const char*': PropertyType.STRING,
        'char*': PropertyType.STRING,
    }
    
    @classmethod
    def map_type(cls, cpp_type: str) -> PropertyType:
        """映射C++类型到属性类型"""
        # 清理类型字符串
        cpp_type = cpp_type.strip()
        
        # 移除const修饰符
        if cpp_type.startswith('const '):
            cpp_type = cpp_type[6:]
        if cpp_type.endswith(' const'):
            cpp_type = cpp_type[:-6]
        
        # 移除引用和指针
        cpp_type = cpp_type.replace('&', '').replace('*', '').strip()
        
        # 查找映射
        if cpp_type in cls.TYPE_MAPPING:
            return cls.TYPE_MAPPING[cpp_type]
        
        # 尝试查找部分匹配（如std::string）
        for key, value in cls.TYPE_MAPPING.items():
            if key in cpp_type:
                return value
        
        # 默认返回UNKNOWN
        return PropertyType.UNKNOWN
    
    @classmethod
    def get_property_function(cls, prop_type: PropertyType) -> str:
        """获取属性创建函数名"""
        mapping = {
            PropertyType.DOUBLE: "aNewPropertyDouble",
            PropertyType.INT: "aNewPropertyInt",
            PropertyType.BOOL: "aNewPropertyBool",
            PropertyType.STRING: "aNewPropertyString",
            PropertyType.UNKNOWN: "aNewProperty"  # 默认
        }
        return mapping.get(prop_type, "aNewProperty")

class BaseHeaderAnalyzer:
    """头文件分析器基类"""
    
    def __init__(self, header_files: List[str], target_classes: Optional[List[str]] = None):
        self.header_files = [Path(f).resolve() for f in header_files]
        self.target_classes = set(target_classes) if target_classes else None
        self.classes: Dict[str, ClassInfo] = {}
        
    def analyze(self) -> bool:
        """执行分析（子类实现）"""
        raise NotImplementedError
    
    def generate_code(self) -> str:
        """生成初始化代码"""
        code = []
        
        for class_name, class_info in self.classes.items():
            if self.target_classes and class_name not in self.target_classes:
                continue
            
            class_code = self._generate_class_init(class_info)
            code.append(class_code)
        
        return "\n\n".join(code)
    
    def _generate_class_init(self, class_info: ClassInfo) -> str:
        """生成单个类的初始化代码"""
        lines = []
        lines.append(f"void {class_info.name}_ClassInit(Class* type)")
        lines.append("{")
        
        lines.append("")  # 空行
        
        # 生成addProperty调用
        for prop_name, prop_info in class_info.properties.items():
            prop_func = TypeMapper.get_property_function(prop_info.property_type)
            
            if prop_info.getter and prop_info.setter:
                # 有getter和setter
                lines.append(f'    type->addProperty{prop_info.property_type.value}("{prop_name}", {prop_func}<{class_info.name}, &{class_info.name}::{prop_info.getter}, &{class_info.name}::{prop_info.setter}>());')
            elif prop_info.getter and not prop_info.setter:
                # 只有getter（只读）
                lines.append(f'    type->addProperty{prop_info.property_type.value}("{prop_name}", {prop_func}<{class_info.name}, &{class_info.name}::{prop_info.getter}>());')
            elif not prop_info.getter and prop_info.setter:
                # 只有setter（不常见）
                lines.append(f'    type->addProperty{prop_info.property_type.value}("{prop_name}", {prop_func}<{class_info.name}, nullptr, &{class_info.name}::{prop_info.setter}>());')
            else:
                lines.append(f"    constexpr auto member_{prop_name} = &{class_info.name}::{prop_info.original_name};")
                # 只有成员变量
                lines.append(f'    type->addProperty{prop_info.property_type.value}("{prop_name}", {prop_func}<{class_info.name}, member_{prop_name}>());')
            lines.append("")  # 空行
        
        lines.append("}")
        
        return "\n".join(lines)
    
    def print_summary(self):
        """打印分析摘要"""
        print(f"分析完成:")
        print(f"头文件: {[str(f) for f in self.header_files]}")
        print(f"目标类: {self.target_classes or '所有类'}")
        print(f"找到的类: {len(self.classes)}")
        
        for class_name, class_info in self.classes.items():
            if self.target_classes and class_name not in self.target_classes:
                continue
            
            print(f"\n类: {class_name}")
            print(f"属性数量: {len(class_info.properties)}")
            
            for prop_name, prop_info in class_info.properties.items():
                print(f"  - {prop_name}: {prop_info.type_name} -> {prop_info.property_type.value}")
                if prop_info.getter:
                    print(f"    getter: {prop_info.getter}")
                if prop_info.setter:
                    print(f"    setter: {prop_info.setter}")

class ClangHeaderAnalyzer(BaseHeaderAnalyzer):
    """使用libclang的头文件分析器"""
    
    def __init__(self, header_files: List[str], target_classes: Optional[List[str]] = None):
        super().__init__(header_files, target_classes)
        self.index = Index.create()
        self.translation_units: Dict[str, TranslationUnit] = {}
        
    def parse_files(self) -> bool:
        """解析所有头文件"""
        for header_file in self.header_files:
            if not header_file.exists():
                print(f"错误: 文件不存在: {header_file}")
                return False
            
            try:
                # 解析头文件
                args = [
                    '-x', 'c++',
                    '-std=c++11',
                    '-DAST_PROPERTIES_MARK=struct __properties_begin__;',
                    '-I.',  # 包含当前目录
                    '-I' + str(header_file.parent),  # 包含头文件所在目录
                ]
                
                tu = self.index.parse(
                    str(header_file),
                    args=args,
                    # options=TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD |  TranslationUnit.PARSE_INCOMPLETE
                )
                
                self.translation_units[str(header_file)] = tu
                
            except Exception as e:
                print(f"解析文件 {header_file} 时出错: {e}")
                return False
        
        return True
    
    
    def extract_classes(self):
        """提取所有类信息"""
        for file_path, tu in self.translation_units.items():
            self._extract_classes_from_cursor(tu.cursor, file_path)
    
    def _extract_classes_from_cursor(self, cursor: Cursor, file_path: str):
        """从游标中提取类信息"""
        # 提取typedef
        if cursor.location.file:
            path = Path(cursor.location.file.name).resolve()
            if not path in self.header_files:
                return
        
        # 检查是否是类声明
        if cursor.kind in [CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL]:
            class_name = cursor.spelling
            
            # 如果指定了目标类，只处理目标类
            if self.target_classes and class_name not in self.target_classes:
                return
            
            # 创建类信息
            if class_name and class_name not in self.classes:
                class_info = ClassInfo(name=class_name, file_path=file_path)
                self.classes[class_name] = class_info
                
                # 分析类的属性
                self._analyze_class_properties(class_info, cursor)
        else:
            # 递归遍历子节点
            for child in cursor.get_children():
                self._extract_classes_from_cursor(child, file_path)
    
    def _analyze_class_properties(self, class_info: ClassInfo, cursor: Cursor):
        """分析类的属性"""
        
        found_mark = False
        in_property_area = False
        
        def process_child(child: Cursor):
            nonlocal in_property_area, found_mark
            
            # 检查是否是属性标记
            if child.kind == CursorKind.STRUCT_DECL and child.spelling == '__properties_begin__':
                found_mark = True
                return
            
            # 处理访问说明符
            if child.kind == CursorKind.CXX_ACCESS_SPEC_DECL:
                if found_mark:
                    in_property_area = True
                    found_mark = False
                else:
                    in_property_area = False


            # 如果在属性区域
            if in_property_area:
                # 处理成员变量
                if child.kind == CursorKind.FIELD_DECL:
                    self._process_member_variable(child, class_info)
                
                # 处理成员函数
                elif child.kind == CursorKind.CXX_METHOD:
                    self._process_member_function(child, class_info)
        
        # 遍历类的所有子节点
        for child in cursor.get_children():
            process_child(child)
    
    def _process_member_variable(self, cursor: Cursor, class_info: ClassInfo):
        """处理成员变量"""
        var_name = cursor.spelling
        type_name = self._get_type_name(cursor)
        
        # 去掉m_前缀
        prop_name = var_name[2:] if var_name.startswith('m_') else var_name
        
        # 映射类型
        prop_type = TypeMapper.map_type(type_name)
        
        # 创建属性信息
        prop_info = PropertyInfo(
            name=prop_name,
            original_name=var_name,
            type_name=type_name,
            property_type=prop_type
        )
        
        class_info.add_property(prop_info)
    
    def _process_member_function(self, cursor: Cursor, class_info: ClassInfo):
        """处理成员函数"""
        method_name = cursor.spelling
        
        # 检查是否是getter
        if self._is_getter(cursor):
            prop_name = self._get_property_name_from_getter(method_name)
            return_type = self._get_type_name(cursor, is_return_type=True)
            
            # 映射类型
            prop_type = TypeMapper.map_type(return_type)
            
            # 获取或创建属性信息
            prop_info = class_info.get_property(prop_name)
            if not prop_info:
                prop_info = PropertyInfo(
                    name=prop_name,
                    original_name=prop_name,
                    type_name=return_type,
                    property_type=prop_type
                )
                class_info.add_property(prop_info)
            
            prop_info.getter = method_name
            prop_info.is_readonly = True  # 默认只读，除非找到setter
        
        # 检查是否是setter
        elif self._is_setter(cursor):
            prop_name = self._get_property_name_from_setter(method_name)
            
            # 获取参数类型
            args = list(cursor.get_arguments())
            if args:
                param_type = self._get_type_name(args[0])
                
                # 映射类型
                prop_type = TypeMapper.map_type(param_type)
                
                # 获取或创建属性信息
                prop_info = class_info.get_property(prop_name)
                if not prop_info:
                    prop_info = PropertyInfo(
                        name=prop_name,
                        original_name=prop_name,
                        type_name=param_type,
                        property_type=prop_type
                    )
                    class_info.add_property(prop_info)
                
                prop_info.setter = method_name
                prop_info.is_readonly = False
        else:
            raise Exception("member function is not valid")
    
    def _get_type_name(self, cursor: Cursor, is_return_type: bool = False) -> str:
        """获取类型名"""
        try:
            if is_return_type:
                type_obj = cursor.result_type
            else:
                type_obj = cursor.type
            
            # 获取规范类型
            canonical_type = type_obj.get_canonical()
            type_name = canonical_type.spelling
            return type_name
        except:
            return "unknown"
    
    def _is_getter(self, cursor: Cursor) -> bool:
        """判断是否是getter函数"""
        if cursor.kind != CursorKind.CXX_METHOD:
            return False
        
        # 检查是否是const成员函数
        if not cursor.is_const_method():
            return False
        
        # 参数
        params = []
        for child in cursor.get_children():
            if child.kind == CursorKind.PARM_DECL:
                params.append(f"{child.spelling}: {child.type.spelling}")

        # 检查参数个数为0
        if len(params) != 0:
            return False
        
        # 检查函数名模式
        method_name = cursor.spelling
        if method_name.startswith('get'):
            return True
        
        # 简单的getter（没有set前缀）
        if not method_name.startswith('set'):
            return True
        
        return False
    
    def _is_setter(self, cursor: Cursor) -> bool:
        """判断是否是setter函数"""
        if cursor.kind != CursorKind.CXX_METHOD:
            return False
        # 参数
        params = []
        for child in cursor.get_children():
            if child.kind == CursorKind.PARM_DECL:
                params.append(f"{child.spelling}: {child.type.spelling}")

        # 检查参数个数为1
        if len(params) != 1:
            return False
        
        # 检查函数名模式
        method_name = cursor.spelling
        if method_name.startswith('set'):
            return True
        
        return False
    
    def _get_property_name_from_getter(self, getter_name: str) -> str:
        """从getter函数名提取属性名"""
        if getter_name.startswith('get'):
            prop_name = getter_name[3:]
            if prop_name:
                return prop_name[0].lower() + prop_name[1:]
        
        return getter_name
    
    def _get_property_name_from_setter(self, setter_name: str) -> str:
        """从setter函数名提取属性名"""
        if setter_name.startswith('set'):
            prop_name = setter_name[3:]
            if prop_name:
                return prop_name[0].lower() + prop_name[1:]
        
        return setter_name
    
    def analyze(self) -> bool:
        """执行完整分析"""
        if not self.parse_files():
            return False
        
        self.extract_classes()
        return len(self.classes) > 0

def test():
    header_files = [r"E:/0-codes/ast/scripts/reflect/test.hpp"]
    analyzer = ClangHeaderAnalyzer(header_files, [])
    
    # 执行分析
    print(f"分析 {len(header_files)} 个头文件...")
    if not analyzer.analyze():
        print("分析失败")
        sys.exit(1)
    
    # 打印摘要
    analyzer.print_summary()
    
    # 生成代码
    code = analyzer.generate_code()
    
    print(code)

def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description='从C++头文件中提取类型属性元信息',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  %(prog)s headers/test.h
  %(prog)s headers/*.h -c Test,AnotherClass
  %(prog)s headers/ -r -c Test -o output.cpp
        """
    )
    
    parser.add_argument('input', nargs='+', help='输入头文件或目录')
    parser.add_argument('-c', '--classes', help='要分析的类名（逗号分隔，如：Test,AnotherClass）')
    parser.add_argument('-o', '--output', help='输出文件（默认为stdout）')
    parser.add_argument('-r', '--recursive', action='store_true', help='递归查找头文件')
    parser.add_argument('--simple', action='store_true', help='使用简化版本（即使libclang可用）')
    parser.add_argument('--add-typedefs', action='store_true', 
                       help='在输出中添加typedef映射（用于调试）')
    
    args = parser.parse_args()
    
    # 收集头文件
    header_files = []
    for input_path in args.input:
        path = Path(input_path)
        
        if path.is_file():
            if path.suffix in ['.h', '.hpp', '.hh', '.hxx']:
                header_files.append(path)
        elif path.is_dir():
            if args.recursive:
                pattern = '**/*.[hH][pPpP]*'
            else:
                pattern = '*.[hH][pPpP]*'
            
            for header in path.glob(pattern):
                if header.is_file():
                    header_files.append(header)
    
    if not header_files:
        print("错误: 未找到头文件")
        sys.exit(1)
    
    # 解析类名列表
    target_classes = None
    if args.classes:
        target_classes = [c.strip() for c in args.classes.split(',')]
        print(f"目标类: {target_classes}")
    
    analyzer = ClangHeaderAnalyzer(header_files, target_classes)
    
    # 执行分析
    print(f"分析 {len(header_files)} 个头文件...")
    if not analyzer.analyze():
        print("分析失败")
        sys.exit(1)
    
    # 打印摘要
    analyzer.print_summary()
    
    # 生成代码
    code = analyzer.generate_code()
    
    # 添加typedef映射（调试用）
    if args.add_typedefs and hasattr(analyzer, 'typedefs'):
        typedef_code = "\n\n// typedef映射:\n"
        for typedef, actual in analyzer.typedefs.items():
            typedef_code += f"//   {typedef} -> {actual}\n"
        code = typedef_code + code
    
    # 输出结果
    if args.output:
        output_path = Path(args.output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write("// 自动生成的属性初始化代码\n")
            f.write("// 警告: 不要手动修改此文件\n\n")
            f.write(code)
        
        print(f"\n代码已生成到: {args.output}")
    else:
        print("\n生成的代码:")
        print("=" * 80)
        print(code)
        print("=" * 80)

if __name__ == "__main__":
    # test()
    main()