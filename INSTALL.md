# SpaceAST Python 绑定 (AstPy) - 安装与使用指南

## 概述

AstPy 是 SpaceAST 航天仿真算法库的 Python 绑定，使用 pybind11 实现。
主要功能模块包括：
- 时间系统（TimePoint, JulianDate, DateTime）
- 数学基础（Vector3d, Matrix3d, Quaternion, Euler, AngleAxis）
- 轨道力学（OrbElem, CartState, TwoBody, 轨道预报器）
- 坐标系（Frame, Axes, Transform）
- 天体系统（CelestialBody, SolarSystem, Planet）
- 物理常量（带单位）

## 快速开始

### 1. 安装方法

#### 方法一：从源码编译（推荐）

```bash
# 克隆仓库
git clone https://gitee.com/space-ast/ast.git
cd ast

# 创建构建目录
mkdir build && cd build

# 配置 CMake
cmake .. -DAST_BUILD_PYTHON=ON -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release

# 安装
cmake --install .
```

#### 方法二：使用 xmake

```bash
# 配置 pybind11 路径
xmake f -m release --pybind11=/path/to/pybind11

# 编译安装
xmake
xmake install
```

#### 方法三：使用 pip 安装（需先编译）

```bash
pip install .
# 或开发模式安装
pip install --editable .
```

### 2. Python 导入

```python
# 方法1：直接导入模块
import astpy

# 方法2：从包导入
from astpy import Vector3d, TimePoint, OrbElem

# 检查版本
print(astpy.__version__)  # 应该输出 "1.0.0"
```

## 核心功能使用

### 时间系统

```python
import astpy

# 创建时间点
tp1 = astpy.TimePoint.FromUTC(2024, 1, 1, 12, 0, 0)
print(f"TimePoint: {tp1}")

# J2000 历元
j2000 = astpy.TimePoint.J2000TT()
print(f"J2000: {j2000}")

# 时间运算
tp2 = tp1 + 3600  # 加1小时
dt = tp2 - tp1    # 时间差（秒）
print(f"时间差: {dt} 秒")

# 转换到不同时间尺度
jd_tt = tp1.toTT()   # TT时间
jd_tdb = tp1.toTDB()  # TDB时间
print(f"TT JD: {jd_tt}")
print(f"TDB JD: {jd_tdb}")

# 字符串表示
print(f"字符串: {tp1.toString(6)}")
```

### 数学运算

```python
import astpy
import math

# 向量操作
v1 = astpy.Vector3d(1.0, 2.0, 3.0)
v2 = astpy.Vector3d(4.0, 5.0, 6.0)

# 基本运算
v_sum = v1 + v2            # 加法
v_diff = v1 - v2           # 减法
v_scaled = v1 * 2          # 数乘
v_norm = v1.norm()        # 模长

# 向量运算
dot = v1.dot(v2)         # 点积
cross = v1.cross(v2)     # 叉积
angle = v1.angleTo(v2)   # 夹角（弧度）

# 单位向量
vx = astpy.Vector3d.UnitX()
vy = astpy.Vector3d.UnitY()
vz = astpy.Vector3d.UnitZ()

# 从列表创建
v_list = astpy.Vector3d([1, 2, 3])

# 矩阵运算
I = astpy.Matrix3d.Identity()  # 单位矩阵
trace = I.trace()            # 迹
det = I.determinant()      # 行列式

# 四元数旋转
# 创建90度绕Z轴旋转的旋转
angle = math.pi / 2  # 90度
axis = astpy.Vector3d(0, 0, 1)
q = astpy.Quaternion.FromAxisAngle(angle, axis)

# 转换为旋转矩阵
R = q.toRotationMatrix()

# 欧拉角
euler = astpy.Euler(math.pi/6, math.pi/4, math.pi/2)  # 30, 45, 90度
R_euler = euler.toMatrix(astpy.Euler.eXYZ)
```

### 轨道力学

```python
import astpy
import math

# 经典轨道根数
oe = astpy.OrbElem(
    a=6778.0,      # 半长轴 [km]
    e=0.001,        # 偏心率
    i=0.9,           # 轨道倾角 [rad] ~51.6度
    raan=0.5,        # 升交点赤经 [rad]
    argper=0.3,      # 近地点幅角 [rad]
    trueA=0.0         # 真近点角 [rad]
)

# 计算平均角速度和轨道周期
GM = astpy.GM_EARTH
n = oe.getMeanMotion(GM)
T = 2 * math.pi / n

print(f"半长轴: {oe.a:.1f} km")
print(f"偏心率: {oe.e:.4f}")
print(f"轨道倾角: {math.degrees(oe.i):.2f}°")
print(f"轨道周期: {T/60:.1f} 分钟")

# 转换为笛卡尔坐标
cs = astpy.moe2rv(oe, GM)
print(f"位置: ({cs.x:.3f}, {cs.y:.3f}, {cs.z:.3f}) km")
print(f"速度: ({cs.vx:.3f}, {cs.vy:.3f}, {cs.vz:.3f}) km/s")

# 二体问题传播
dt = 3600  # 推进1小时
cs_future = astpy.TwoBody.propagate(cs, dt, GM)

print(f"\n推进后位置: ({cs_future.x:.3f}, {cs_future.y:.3f}, {cs_future.z:.3f}) km")
```

### 坐标变换

```python
import astpy
import math

# 创建旋转变换
angle = math.pi / 4  # 45度
axis = astpy.Vector3d(0, 0, 1)
R = astpy.aRotationZMatrix(angle)

# 应用旋转向量
v = astpy.Vector3d(1, 0, 0)
v_rotated = R * v
print(f"旋转后: {v_rotated}")

# 四元数方式
q = astpy.Quaternion.FromAxisAngle(angle, axis)
R_from_q = q.toRotationMatrix()

# 坐标系获取（需要 SpaceAST 已初始化）
# ss = astpy.SolarSystem.Get()
# earth = ss.getEarth()
# frame = earth.getFrame()
```

### 天体系统

```python
import astpy

# 获取太阳系
ss = astpy.SolarSystem.Get()
earth = ss.getEarth()
sun = ss.getSun()

# 天体参数
print(f"地球 GM: {earth.gm:.3e} km³/s²")
print(f"地球半径: {earth.radius:.1f} km")
print(f"太阳 GM: {sun.gm:.3e} km³/s²")

# 物理常量
print(f"天文单位: {astpy.AU:.1f} km")
print(f"光速: {astpy.SPEED_OF_LIGHT:.1f} km/s")
```

## 高级用法

### 轨道预报

```python
# 创建 ISS 类轨道
import astpy
import math

# 使用轨道设计器
designer = astpy.CircularOrbitDesigner(radius=6778.0 + 400.0, i=0.9)
oe = designer.getOrbElem()
cs = designer.getCartState(trueAnomaly=0.0, gm=astpy.GM_EARTH)

# 推进多圈
dt = 5400  # 轨道周期/4
states = []
for i in range(5):
    cs = astpy.TwoBody.propagate(cs, dt, astpy.GM_EARTH)
    states.append(cs)
    print(f"第{i+1}圈: 位置=({cs.x:.1f}, {cs.y:.1f}, {cs.z:.1f}) km")
```

### 姿态转换

```python
import astpy
import math

# 3-2-1 欧拉角序列（滚转-偏航-俯仰）
roll = math.radians(10)
pitch = math.radians(20)
yaw = math.radians(30)

# 创建欧拉角
euler = astpy.Euler(roll, pitch, yaw)

# 转换到四元数
q = euler.toQuat(astpy.Euler.eZYX)

# 转换到旋转矩阵
R = euler.toMatrix(astpy.Euler.eZYX)

print(f"ZYX 旋转矩阵:\n{R}")
```

## 示例代码位置

- `examples/python/basic_usage.py` - 基础使用示例
- `examples/python/orbit_propagation.py` - 轨道传播示例
- `examples/python/attitude_examples.py` - 姿态表示示例
- `examples/python/tests/` - 单元测试

运行示例：

```bash
# 进入 examples 目录
cd examples/python

# 运行基础示例
python basic_usage.py

# 运行单元测试
pytest tests/ -v
```

## 常见问题

### 编译错误

1. **找不到 Python.h**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install python3-dev
   
   # macOS
   brew install python
   ```

2. **找不到 pybind11**
   ```bash
   # 方法1：下载并编译
   git clone https://github.com/pybind/pybind11.git
   cd pybind11
   mkdir build && cd build
   cmake ..
   cmake --install .
   
   # 方法2：使用包管理器
   pip install pybind11
   ```

3. **C++ 版本不兼容**
   SpaceAST 需要 C++17 或更高版本
   确认编译器版本：`g++ --version` 或 `clang++ --version`

### 运行时错误

1. **ImportError: No module named '_ast'**
   - 检查编译是否成功
   - 检查模块路径：`sys.path`
   - 检查是否安装到正确位置

2. **找不到共享库**
   - Linux: 检查 `LD_LIBRARY_PATH`
   - macOS: 检查 `DYLD_LIBRARY_PATH`
   - Windows: 检查 PATH

## 性能优化建议

1. **减少 Python-C++ 调用次数**
   - 在 C++ 中完成复杂计算
   - 只在必要时调用 Python

2. **使用 NumPy 数组**
   - 考虑添加 NumPy 绑定以支持向量化操作
   - 批量处理时性能更好

3. **避免不必要的类型转换**
   - 尽量使用原生类型
   - 减少对象创建开销

## 贡献指南

欢迎为 AstPy 项目做出贡献！

1. Fork 仓库
2. 创建功能分支
3. 提交代码更改
4. 推送到主分支
5. 创建 Pull Request

## 许可证

AstPy 使用 Apache License 2.0，与 SpaceAST 主库保持一致。

## 联系方式

- 项目地址：https://gitee.com/space-ast/ast
- 文档：https://space-ast.github.io/ast/api/
- 问题反馈：在 Gitee 提交 Issue

## 版本历史

### v1.0.0 (2024-01)
- 初始版本发布
- 实现时间系统绑定
- 实现数学基础类型绑定
- 实现轨道力学绑定
- 实现坐标系绑定
- 实现天体系统绑定
- 添加单元测试
