# AST (Astrodynamics Toolkit) 项目编码指南

本 skill 适用于为 AST 航天动力学工具库编写代码。AST 是一个 C++11 航天动力学计算库，使用 xmake 构建，支持 Windows/Linux/WebAssembly。

**触发条件**: 当用户要求在工程中编写、修改、调试代码，或涉及轨道力学、坐标转换、时间系统、轨道预报等任务时，使用此 skill。

---

## 1. 命名空间与头文件

### 1.1 命名空间宏

所有代码在 `ast` 命名空间内，通过宏控制：

```cpp
#include "AstGlobal.h"

AST_NAMESPACE_BEGIN   // → namespace ast {
// ... 代码 ...
AST_NAMESPACE_END     // → }
```

使用宏：
```cpp
AST_USING_NAMESPACE   // → using namespace ast;
```

### 1.2 头文件包含

- **转发头文件**: `include/ast/` 和 `include/AstCore/` 等目录下的是转发头文件，内容为 `#include "../../src/..."`
- **实际源文件**: `src/` 目录下，如 `src/AstCore/Orbit/OrbitElement.hpp`
- **聚合头文件**: `src/AstCore/AstCore.hpp` 包含了所有核心模块头文件
- 新代码写头文件时放在 `src/模块名/` 下，同时在 `include/ast/` 或 `include/模块名/` 下创建转发头文件
- 头文件保护统一使用 `#pragma once`
- 文件编码使用 UTF-8 with BOM (在 MSVC 下编译 ast 库时使用 `#pragma execution_character_set("utf-8")`)

### 1.3 引用方式

在示例和测试中，使用简短头文件引用：
```cpp
#include "ast/OrbitElement.hpp"    // 转发到 src/AstCore/Orbit/OrbitElement.hpp
#include "ast/Vector.hpp"          // 转发到 src/AstMath/Array/Vector.hpp
#include "ast/TimePoint.hpp"       // 转发到 src/AstCore/Time/TimePoint.hpp
#include "ast/Constants.h"         // 转发到 src/AstUtil/Util/Constants.h
#include "ast/Literals.hpp"        // 转发到 src/AstUtil/Util/Literals.hpp
```

---

## 2. 命名规范

### 2.1 核心命名规则

| 类别 | 规则 | 示例 |
|------|------|------|
| 类/结构体 | PascalCase | `TimePoint`, `OrbElem`, `Vector3d`, `HPOP`, `FrameAssembly` |
| C风格API函数 | `a` 前缀 + PascalCase | `aCartToOrbElem()`, `aInitialize()`, `aTwoBodyProp()` |
| 成员函数 | camelCase | `getSMA()`, `setForceModel()`, `propagate()` |
| 成员变量 | 尾部下划线 | `pos_`, `vel_`, `a_`, `duration_`, `forceModel_` |
| 常量 | `k` 前缀 + PascalCase | `kPI`, `kEarthGrav`, `kJ2000Epoch`, `kDegToRad` |
| 宏 | `A_` (通用) 或 `AST_` (项目专用) | `A_ALWAYS_INLINE`, `AST_CORE_API` |
| 枚举类型 | `E` 前缀，枚举值 `e` 前缀 | `enum EError { eError, eNoError, eErrorInvalidParam }` |
| 枚举类 | `E` 前缀 + `enum class` | `enum class EObjectFlags: uint32_t { eNone, eReadOnly }` |
| 类型别名 | typedef + 后缀 | `errc_t`, `color_t`, `ObjectId`, `length_d`, `angle_d` |
| 命名空间 | 全小写 | `ast`, `literals`, `units` |

### 2.2 量纲类型别名

用于属性元信息标注：
```cpp
typedef double length_d, mass_d, time_d, area_d, speed_d, force_d, energy_d,
    power_d, angle_d, angvel_d, temperature_d, density_d, pressure_d;
```

### 2.3 常用类型

```cpp
typedef int errc_t;              // 错误码类型
typedef double real_t;           // 实数类型
typedef uint32_t color_t;        // 颜色值
typedef uint32_t ObjectId;       // 对象ID
typedef double ImpreciseJD;      // 不精确儒略日
typedef VectorN<double, 3> Vector3d;
typedef MatrixMN<double, 3, 3> Matrix3d;
typedef MatrixMN<double, 6, 6> Matrix6d;
typedef std::array<double, 6> array6d;
typedef std::array<double, 3> array3d;
```

---

## 3. 错误处理

### 3.1 错误码枚举

```cpp
enum EError {
    eError = -1,            // 通用错误
    eNoError = 0,           // 没有错误
    eErrorNullPtr = 1,      // 空指针错误
    eErrorNullInput,        // 输入参数是空指针
    eErrorInvalidParam,     // 非法输入参数
    eErrorNotInit,          // 没有初始化
    eErrorInvalidFile,      // 文件格式错误
    eErrorInvalidType,      // 类型错误
    eErrorInvalidValue,     // 值错误
    eErrorParse,            // 解析错误
    eErrorReadonly,         // 只读属性
    eErrorMaxIter,          // 最大迭代次数
    eErrorDivideByZero,     // 除零错误
    eErrorOutOfRange,       // 超出范围
    eErrorNotFound,         // 未找到
    eErrorNotImplemented,   // 未实现
    eErrorUnsupported,      // 不支持的类型
};
```

### 3.2 使用模式

```cpp
// 函数返回 errc_t
errc_t err = aCartToOrbElem(pos, vel, gm, elem);
if (err == eNoError) {
    // 成功
} else {
    // 失败，处理错误
}

// 可能返回错误码的函数签名模式
AST_CORE_CAPI errc_t aFunctionName(const Type& input, Type& output);
```

---

## 4. DLL 导出声明

### 4.1 模块导出宏

每个模块有两个导出宏：C++版和C版

| 模块 | C++导出宏 | C导出宏 | 编译定义 |
|------|----------|--------|---------|
| AstCore | `AST_CORE_API` | `AST_CORE_CAPI` | `AST_BUILD_LIB_CORE` |
| AstMath | `AST_MATH_API` | `AST_MATH_CAPI` | `AST_BUILD_LIB_MATH` |
| AstUtil | `AST_UTIL_API` | `AST_UTIL_CAPI` | `AST_BUILD_LIB_UTIL` |
| AstCalc | `AST_CALC_API` | `AST_CALC_CAPI` | `AST_BUILD_LIB_CALC` |
| AstSim | `AST_SIM_API` | `AST_SIM_CAPI` | `AST_BUILD_LIB_SIM` |
| AstScript | `AST_SCRIPT_API` | `AST_SCRIPT_CAPI` | `AST_BUILD_LIB_SCRIPT` |
| AstPlot | `AST_PLOT_API` | `AST_PLOT_CAPI` | `AST_BUILD_LIB_PLOT` |
| AstChart | `AST_CHART_API` | `AST_CHART_CAPI` | `AST_BUILD_LIB_CHART` |
| AstGUI | `AST_GUI_API` | `AST_GUI_CAPI` | `AST_BUILD_LIB_GUI` |
| AstGfx | `AST_GFX_API` | `AST_GFX_CAPI` | `AST_BUILD_LIB_GFX` |
| AstWeather | `AST_WEATHER_API` | `AST_WEATHER_CAPI` | `AST_BUILD_LIB_WEATHER` |
| AstSPICE | `AST_SPICE_API` | `AST_SPICE_CAPI` | `AST_BUILD_LIB_SPICE` |
| AstTest | `AST_TEST_API` | `AST_TEST_CAPI` | `AST_BUILD_LIB_TEST` |
| AstLoader | `AST_LOADER_API` | `AST_LOADER_CAPI` | `AST_BUILD_LIB_LOADER` |
| AstReport | `AST_REPORT_API` | `AST_REPORT_CAPI` | `AST_BUILD_LIB_REPORT` |
| AstAI | `AST_AI_API` | `AST_AI_CAPI` | `AST_BUILD_LIB_AI` |
| AstCmd | `AST_CMD_API` | `AST_CMD_CAPI` | `AST_BUILD_LIB_CMD` |
| AstCOM | `AST_COM_API` | `AST_COM_CAPI` | `AST_BUILD_LIB_COM` |
| AstAnalyzer | `AST_ANALYZER_API` | `AST_ANALYZER_CAPI` | `AST_BUILD_LIB_ANALYZER` |
| AstMock | `AST_MOCK_API` | `AST_MOCK_CAPI` | `AST_BUILD_LIB_MOCK` |
| AstOpt | `AST_OPT_API` | `AST_OPT_CAPI` | `AST_BUILD_LIB_OPT` |
| AstFault | `AST_FAULT_API` | `AST_FAULT_CAPI` | `AST_BUILD_LIB_FAULT` |
| AstUiPilot | `AST_UIPILOT_API` | `AST_UIPILOT_CAPI` | `AST_BUILD_LIB_UIPILOT` |
| AstUiAI | `AST_UIAI_API` | `AST_UIAI_CAPI` | `AST_BUILD_LIB_UIAI` |
| AstUiUtil | `AST_UIUTIL_API` | `AST_UIUTIL_CAPI` | `AST_BUILD_LIB_UIUTIL` |

### 4.2 使用方式

```cpp
// C++ 函数导出
AST_CORE_API void someFunction();

// C 函数导出 (extern "C" + dllexport)
AST_CORE_CAPI errc_t aSomeCFunction(const double* input, double* output);
```

---

## 5. 注释风格 (Doxygen)

```cpp
/// @brief 简要描述
/// @param paramName 参数说明 [单位]
/// @return 返回值说明
/// @note 注意事项
/// @details 详细描述
/// @code
/// // 示例代码
/// @endcode

// 多行块注释使用：
/*!
    @addtogroup GroupName
    @{
*/
// ... 代码 ...
/*! @} */

// 模块分组标注：
/// @ingroup   Core
/// @defgroup Orbit 轨道计算
/// @brief 提供轨道根数、轨道参数计算及转换相关接口。
```

---

## 6. 关键宏速查

### 6.1 命名空间与对象

| 宏 | 展开为 | 用途 |
|---|---|---|
| `AST_NAMESPACE_BEGIN` | `namespace ast{` | 命名空间开始 |
| `AST_NAMESPACE_END` | `}` | 命名空间结束 |
| `AST_USING_NAMESPACE` | `using namespace ast;` | 引入命名空间 |
| `_AST` | `::ast::` | 命名空间前缀 |
| `AST_OBJECT(TYPE)` | RTTI类声明 | 对象RTTI注册 |
| `AST_PROPERT(NAME)` | `static constexpr const char* _prop_##NAME = #NAME;` | 属性声明 |
| `PROPERTIES` | `public` (或 `AST_PROPERTIES_MARK public`) | 属性区段标签 |

### 6.2 访问器与迭代器

```cpp
// 自动生成 getter/setter
AST_DEF_ACCESS_METHOD(double, a)
// 展开为: double a() const {return a_;} double& a() {return a_;}

// POD类型迭代器 (基于 this 指针和 sizeof)
A_DEF_POD_ITERABLE(double)
// 提供: size(), data(), begin(), end(), operator[]

// 容器迭代器
A_DEF_ITERABLE(Scalar, Data, Size)
```

### 6.3 编译器与平台

| 宏 | 用途 |
|---|---|
| `A_ALWAYS_INLINE` | 强制内联 |
| `A_STRONG_INLINE` | 强力内联 |
| `A_NO_INLINE` | 禁止内联 |
| `A_UNUSED(x)` | 抑制未使用警告 |
| `A_LIKELY(expr)` | 分支预测：大概率 |
| `A_UNLIKELY(expr)` | 分支预测：小概率 |
| `A_CONST` | 纯函数属性 |
| `A_PURE` | 纯读函数属性 |
| `A_NODISCARD` | 返回值不可忽略 |
| `A_ENUM_CLASS_FLAGS(Enum)` | 枚举位运算符 |
| `A_DISABLE_COPY(Class)` | 禁用拷贝构造和赋值 |
| `A_THREAD_LOCAL` | 线程本地存储 |
| `A_CXX11` / `A_CXX14` / `A_CXX17` / `A_CXX20` | C++版本检测 |
| `A_WINDOWS` / `A_LINUX` / `A_APPLE` / `A_WASM` | 平台检测 |
| `A_MSVC` / `A_GCC` / `A_CLANG` | 编译器检测 |

---

## 7. 核心数据类型与接口

### 7.1 向量 (Vector3d)

```cpp
// 定义在 src/AstMath/Array/Vector.hpp
// 聚合初始化（成员变量为public仅为此目的）
Vector3d pos{7000e3, 0.0, 0.0};
Vector3d vel{0.0, 7500.0, 0.0};

// 分量访问
pos.x(); pos.y(); pos.z();           // const 访问
pos.x() = 100.0;                     // 引用修改
pos[0]; pos[1]; pos[2];              // 索引访问
pos(0); pos(1); pos(2);              // 函数访问

// 向量运算
pos.norm();                           // 范数
pos.squaredNorm();                    // 平方范数
pos.normalized();                     // 返回归一化副本
pos.normalize();                      // 原地归一化
pos.cross(other);                     // 叉积
pos.dot(other);                       // 点积
pos.angle(other);                     // 夹角
Vector3d::Angle(v1, v2);              // 静态方法求夹角
Vector3d::Zero();                     // 零向量
Vector3d::UnitX();                    // {1,0,0}
Vector3d::UnitY();                    // {0,1,0}
Vector3d::UnitZ();                    // {0,0,1}

// 运算符
pos + vel; pos - vel; pos * 2.0; pos / 2.0; -pos;
pos += vel; pos -= vel;

// 变长向量
VectorXd vec(size);
vec.resize(n); vec.setZero();
```

### 7.2 矩阵 (Matrix3d / MatrixXd)

```cpp
// 定义在 src/AstMath/Array/Matrix.hpp
// Matrix3d = MatrixMN<double, 3, 3> (固定大小)
// MatrixXd = Matrix<double> (动态大小)

Matrix3d mtx;
mtx(0, 0) = 1.0;  // 行列访问

MatrixXd dynMtx(3, 3);
dynMtx.resize(4, 4);
dynMtx.setZero();
dynMtx.row(); dynMtx.col(); dynMtx.size();
```

### 7.3 四元数 (Quaternion)

```cpp
// 定义在 src/AstMath/Attitude/Quaternion.hpp
// 内部存储: qs(实部), qx, qy, qz(虚部)
Quaternion q{1, 0, 0, 0};  // 单位四元数 (qs, qx, qy, qz)

Quaternion::Identity();                    // 单位四元数
Quaternion::FromRotationMatrix(mtx);       // 从旋转矩阵创建
q.norm(); q.squaredNorm(); q.normalize(); q.normalized();
q.conjugate(); q.inverse();
q.w(); q.x(); q.y(); q.z();               // Eigen兼容接口 (w=qs)

// 四元数乘积
Quaternion result = aQuatProduct(q1, q2);
```

### 7.4 欧拉角 (Euler)

```cpp
// 定义在 src/AstMath/Attitude/Euler.hpp
Euler euler;
euler.angle1_ = 0.1;  // 第一个角 [rad]
euler.angle2_ = 0.2;  // 第二个角 [rad]
euler.angle3_ = 0.3;  // 第三个角 [rad]

// 旋转序
Euler::eXYZ (123), Euler::eZYX (321), Euler::eZXZ (313) 等

// 转换
Matrix3d mtx;
euler.toMatrix(Euler::eZYX, mtx);       // 欧拉角→矩阵
Quaternion quat;
euler.toQuat(Euler::eZYX, quat);        // 欧拉角→四元数
euler.fromMatrix(mtx, Euler::eZYX);     // 矩阵→欧拉角
euler.fromQuat(quat, Euler::eZYX);      // 四元数→欧拉角
```

### 7.5 姿态转换函数

```cpp
// 定义在 src/AstMath/Attitude/AttitudeConvertProto.hpp
aRotationMatrix(angle, axis, mtx);          // 绕轴旋转矩阵 (axis: 1=X, 2=Y, 3=Z)
aRotationXMatrix(angle, mtx);               // 绕X轴旋转
aRotationYMatrix(angle, mtx);               // 绕Y轴旋转
aRotationZMatrix(angle, mtx);               // 绕Z轴旋转
aQuatToMatrix(quat, mtx);                   // 四元数→矩阵
aMatrixToQuat(mtx, quat);                   // 矩阵→四元数
aMatrixToEuler(mtx, seq, euler);            // 矩阵→欧拉角
aEulerToMatrix(euler, seq, mtx);            // 欧拉角→矩阵
aEulerToQuat(euler, seq, quat);             // 欧拉角→四元数
aQuatToEuler(quat, seq, euler);             // 四元数→欧拉角
```

### 7.6 时间 (TimePoint)

```cpp
// 定义在 src/AstCore/Time/TimePoint.hpp

// 创建时间点
TimePoint tp = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
TimePoint now = TimePoint::CurrentTime();
TimePoint today = TimePoint::TodayUTC();
TimePoint j2000 = TimePoint::J2000TT();
TimePoint tp = TimePoint::Parse("2026-01-01 00:00:00");
TimePoint tp = TimePoint::ParseUTC("2026-01-01 00:00:00");
TimePoint tp = TimePoint::ParseBJT("2026-01-01 08:00:00");

// 字面量
TimePoint tp = "2026-01-01 00:00:00"_utc;
TimePoint tp = "2026-01-01 08:00:00"_bjt;

// 时间运算
TimePoint later = tp + 3600.0;          // 加3600秒
TimePoint earlier = tp - 3600.0;        // 减3600秒
double diff = later - tp;                // 时间差（秒）
double days = tp.daysFromJ2000TT();      // 距J2000的天数
double centuries = tp.julianCenturyFromJ2000TT();  // 儒略世纪

// 时间系统转换
JulianDate jdTT = tp.toTT();             // 转TT儒略日
JulianDate jdTDB = tp.toTDB();           // 转TDB儒略日
JulianDate jdTAI = tp.toTAI();           // 转TAI儒略日
double epochSec = tp.toEpochSecond();    // 相对历元秒数

// 格式化
std::string str = tp.toString();         // 默认精度6位
std::string str = tp.toString(3);        // 3位精度

// 其他静态方法
TimePoint::Default(); TimePoint::Epoch();
TimePoint::TomorrowUTC();
TimePoint::FromTT(jdTT); TimePoint::FromTDB(jdTDB);
TimePoint::FromTAI(jdTAI);
```

### 7.7 轨道根数

```cpp
// 定义在 src/AstCore/Orbit/OrbitElement.hpp

// 经典轨道根数 (OrbElem)
OrbElem coe;
coe.a_ = 7000000.0;     // 长半轴 [m]
coe.e_ = 0.01;          // 偏心率
coe.i_ = 0.1;           // 轨道倾角 [rad]
coe.raan_ = 0.2;        // 升交点赤经 [rad]
coe.argper_ = 0.3;      // 近拱点角 [rad]
coe.trueA_ = 0.4;       // 真近点角 [rad]
// 或聚合初始化: OrbElem coe = {7e6, 0.01, 0.1, 0.2, 0.3, 0.4};
coe.getSMA();           // 长半轴
coe.getE();             // 偏心率
coe.getMeanMotion(gm);  // 平均角速度

// 修正轨道根数 (ModOrbElem) - 近拱点半径版
ModOrbElem moe;
moe.rp_ = ...;          // 近拱点半径 [m]
moe.getSMA();           // = rp_ / (1 - e_)
moe.getPeriod(gm);      // 轨道周期
moe.getApoRad();        // 远拱点半径
moe.getApoAlt(bodyRadius);  // 远拱点高度

// 春分点根数 (EquinElem): a, h, k, p, q, lambda
// 改进春分点根数 (ModEquinElem): p, f, g, h, k, L  (180度奇异)
// 德洛奈根数 (DelaunayElem): L, G, H, l, g, h

// 直角坐标状态 (CartState)
CartState state;
state.pos_ = Vector3d{7000e3, 0, 0};   // 位置 [m]
state.vel_ = Vector3d{0, 7500, 0};     // 速度 [m/s]
state.x(); state.y(); state.z();        // 位置分量
state.vx(); state.vy(); state.vz();     // 速度分量
CartState::Zero();                       // 零状态
```

### 7.8 轨道根数转换 API

```cpp
// 所有转换函数均为 C API 风格，返回 errc_t (void 表示不会失败)

// 直角坐标 ↔ 经典轨道根数
errc_t aCartToOrbElem(const Vector3d& pos, const Vector3d& vel, double gm, OrbElem& elem);
errc_t aOrbElemToCart(const OrbElem& elem, double gm, Vector3d& pos, Vector3d& vel);

// 直角坐标 ↔ 修正轨道根数
errc_t aCartToModOrbElem(const Vector3d& pos, const Vector3d& vel, double gm, ModOrbElem& modOrb);
errc_t aModOrbElemToCart(const ModOrbElem& modOrb, double gm, Vector3d& pos, Vector3d& vel);

// 直角坐标 ↔ 春分点根数
void aCartToEquinElem(const Vector3d& pos, const Vector3d& vel, double gm, EquinElem& equinElem);
void aEquinElemToCart(const EquinElem& equinElem, double gm, Vector3d& pos, Vector3d& vel);

// 直角坐标 ↔ 改进春分点根数
errc_t rv2mee(const double* pos, const double* vel, double gm, double* mee);
void aCartToModEquinElem(const Vector3d& pos, const Vector3d& vel, double gm, ModEquinElem& mee);
void aModEquinElemToCart(const ModEquinElem& mee, double gm, Vector3d& pos, Vector3d& vel);

// 经典 ↔ 德洛奈
errc_t aOrbElemToDelaunay(const OrbElem& elem, double gm, DelaunayElem& delaunay);
errc_t aDelaunayToOrbElem(const DelaunayElem& delaunay, double gm, OrbElem& elem);

// 各根数之间互转: coe↔moe, coe↔ee, coe↔mee, moe↔ee, moe↔mee, ee↔mee 等
// C风格指针版: coe2rv, rv2coe, coe2mee, mee2rv, rv2mee 等
```

### 7.9 轨道参数转换 API (OrbitParam)

```cpp
// 定义在 src/AstCore/Orbit/OrbitParam.hpp
// 命名规则: a{源}To{目标}(参数...)
// 所有角度单位为 rad，长度单位为 m，时间单位为 s，质量为 kg

// 近点角转换
double aTrueToMean(double trueAnomaly, double eccentricity);
double aMeanToTrue(double meanAnomaly, double eccentricity, double eps=1e-14, int maxIter=100);
double aTrueToEcc(double trueAnomaly, double eccentricity);
double aEccToTrue(double eccAnomaly, double eccentricity);
double aMeanToEcc(double meanAnomaly, double eccentricity, double eps=1e-14, int maxIter=100);
double aEccToMean(double eccAnomaly, double eccentricity);

// 长半轴转换
double aSMAToMeanMotion(double semiMajorAxis, double gm);
double aMeanMotionToSMA(double meanMotion, double gm);
double aSMAToPeriod(double semiMajorAxis, double gm);
double aPeriodToSMA(double period, double gm);
double aSMAToPeriRad(double semiMajorAxis, double eccentricity);
double aSMAToApoRad(double semiMajorAxis, double eccentricity);
double aPeriRadToSMA(double perigeeRad, double eccentricity);
double aApoRadToSMA(double apogeeRad, double eccentricity);

// 高度转换
double aPeriRadToPeriAlt(double perigeeRad, double bodyRadius);
double aApoRadToApoAlt(double apogeeRad, double bodyRadius);
double aPeriAltToPeriRad(double perigeeAlt, double bodyRadius);
double aApoAltToApoRad(double apogeeAlt, double bodyRadius);

// 复合转换
double aPeriRadToPeriod(double perigeeRad, double eccentricity, double gm);
double aApoRadToPeriod(double apogeeRad, double eccentricity, double gm);
double aPeriAltToMeanMotion(double perigeeAlt, double eccentricity, double bodyRadius, double gm);
double aApoAltToMeanMotion(double apogeeAlt, double eccentricity, double bodyRadius, double gm);

// 角度组合
double aTrueToArgLat(double trueAnomaly, double argPeri);
double aArgLatToTrue(double argLat, double argPeri);
double aTrueToTrueLong(double trueAnomaly, double argPeri, double raan);
double aArgPeriToLongPeri(double argPeri, double raan);

// 时间转换
double aTrueToTimePastPeri(double trueAnomaly, double semiMajorAxis, double eccentricity, double gm);
double aTrueToTimePastAscNode(double trueAnomaly, double argPeri, double semiMajorAxis, double eccentricity, double gm);

// 扁率转换
double aEccToFlat(double eccentricity);
double aFlatToEcc(double flatFactor);

// 重复轨道
double aRepeatGroundTrackSMA(int daysToRepeat, int revsToRepeat, double gm, double bodyRotRate);
```

### 7.10 轨道预报器

```cpp
// 二体预报 (src/AstCore/Propagator/TwoBody.hpp)
errc_t aTwoBodyProp(double duration, double gm, Vector3d& r, Vector3d& v);

// J2 分析预报 (src/AstCore/Propagator/J2Analytical.hpp)
// J4 分析预报 (src/AstCore/Propagator/J4Analytical.hpp)
// J2J4 分析预报 (src/AstCore/Propagator/J2J4Analytical.hpp)

// HPOP 高精度轨道预报器 (src/AstCore/Propagator/HPOP/HPOP.hpp)
HPOP hpop;
HPOPForceModel forceModel;
forceModel.gravity().model_ = "JGM3";     // 重力场模型
forceModel.gravity().maxDegree_ = 2;       // 阶数
forceModel.gravity().maxOrder_ = 0;        // 次数
hpop.setForceModel(forceModel);
hpop.setSpacecraftParam(spacecraftParam);  // 航天器参数
hpop.setPropagationFrame(frame);           // 预报坐标系
hpop.setIntegrator(integrator);            // 积分器
hpop.addEventDetector(detector);           // 事件检测器
errc_t err = hpop.initialize();
errc_t err = hpop.propagate(startTime, targetTime, position, velocity);
```

### 7.11 坐标系

```cpp
// 定义在 src/AstCore/Geometry/Euclid/Frame.hpp
// Frame = Point(原点) + Axes(轴系)，继承 ObjectNamed

// 内置坐标系
Frame* eci  = aFrameECI();          // 地球惯性系
Frame* ecf  = aFrameECF();          // 地球固连系
Frame* icrf = aFrameEarthICRF();    // ICRF系
Frame* j2000 = aFrameEarthJ2000();  // J2000系

// 组装坐标系
PFrameAssembly frame = FrameAssembly::New(point, axes);
HFrameAssembly hFrame = FrameAssembly::MakeShared(point, axes);

// 带历元的坐标系
PFrameWithEpoch fwe = FrameWithEpoch::New(origin, axes);
PFrameWithEpoch fwe = FrameWithEpoch::New(origin, axes, epoch);

// 坐标系变换
Transform transform;
errc_t err = aFrameTransform(source, target, tp, transform);

// 运动学变换（含速度）
KinematicTransform kt;
errc_t err = aFrameTransform(source, target, tp, kt);

// 便捷转换函数
Vector3d vecECF;
aJ2000ToECF(tp, vecJ2000, vecECF);

// 解析坐标系/轴系/点/天体（通过名称）
Frame* frame = aResolveFrame("Earth/J2000");
Axes* axes = aResolveAxes("J2000");
Point* point = aResolvePoint("Earth");
Body* body = aResolveBody("Earth");

// 字面量操作符
Frame* f = "Earth/J2000"_frame;
Body* b = "Earth"_body;
Point* p = "Earth"_point;
```

### 7.12 初始化与运行时

```cpp
// 定义在 src/AstCore/Data/Context/RunTime.hpp

// 全局初始化（加载EOP、跳秒、JPL DE等数据）
errc_t aInitialize();

// 通过配置文件初始化
errc_t aInitializeByConfig(StringView configfile);

// 数据目录管理
errc_t aDataDirSet(StringView dirpath);
std::string aDataDirGet();

// 数据上下文
DataContext* aDataContext_GetDefault();
DataContext* aDataContext_GetCurrent();
DataContext* aDataContext_EnsureCurrent();
void aDataContext_SetCurrent(DataContext* context);

// 获取数据
EOP* aDataContext_GetEOP();
SolarSystem* aDataContext_GetSolarSystem();

// 清理
errc_t aUninitialize();

// 默认数据文件路径
// AST_DEFAULT_FILE_LEAPSECOND    = "Time/Leap_Second.dat"
// AST_DEFAULT_FILE_JPLDE         = "SolarSystem/plneph.430"
// AST_DEFAULT_FILE_EOP           = "SolarSystem/Earth/EOP-All.txt"
// AST_DEFAULT_FILE_SPACEWEATHER  = "SolarSystem/Earth/SW-Last5Years.txt"
// AST_DEFAULT_DIR_SOLARSYSTEM    = "SolarSystem/"
```

---

## 8. 物理常量

```cpp
// 定义在 src/AstUtil/Util/Constants.h

// PI
constexpr double kPI = 3.14159265358979...;
constexpr double kTwoPI = 6.28318530717958...;
constexpr double kHalfPI = 1.57079632679489...;
constexpr double kDegToRad = 0.01745329251994...;
constexpr double kRadToDeg = 57.29577951308232...;
constexpr double kArcSecToRad = 4.84813681109536e-6;

// 天文常量
constexpr double kAU = 1.49597870691e11;          // 天文单位 [m]
constexpr double kLightSpeed = 2.99792458e8;      // 光速 [m/s]
constexpr double kGrav = 6.673e-11;                // 万有引力常数 [m^3/kg*s^2]

// 地球
constexpr double kEarthGrav = 3.986004418e14;     // 地球引力常数 [m^3/s^2]
constexpr double kEarthRadius = 6378137.0;         // 地球赤道半径 [m]
constexpr double kEarthMinRadius = 6356752.31424;  // 地球极半径 [m]
constexpr double kEarthAngVel = 7.292115146706979e-5; // 地球角速度 [rad/s]
constexpr double kEarthSiderealDay = 86164.09054;  // 恒星日 [s]

// 其他天体
constexpr double kMoonGrav = 4.9028029535968e+12;  // 月球引力常数 [m^3/s^2]
constexpr double kMoonRadius = 1737400.0;           // 月球半径 [m]
constexpr double kSunGrav = 1.327122E20;            // 太阳引力常数 [m^3/s^2]
constexpr double kSunRadius = 695990000.0;          // 太阳半径 [m]
// ... 火星、木星、金星等均有定义

// 时间
constexpr double kSecondsPerDay = 86400.0;
constexpr double kDaysPerJulianCentury = 36525.0;
constexpr double kTTMinusTAI = 32.184;             // TT相对TAI偏置 [s]

// 历元
constexpr double kJ2000Epoch = 2451545.0;          // J2000儒略日
```

---

## 9. 自定义字面量

```cpp
// 定义在 src/AstUtil/Util/Literals.hpp
// 使用前: using namespace _AST literals; 或 AST_USING_NAMESPACE (已自动导入)

// 角度 → 弧度
double a = 90.0_deg;       // 90度 → 1.5708 rad
double b = 3.14159_rad;    // 弧度
double c = 1.0_arcsec;     // 弧秒 → rad
double d = 1.0_revs;       // 周期 → 2π rad

// 长度 → 米
double d1 = 7000_km;       // → 7e6 m
double d2 = 100_m;         // → 100 m
double d3 = 1.0_au;        // → 1.496e11 m

// 时间 → 秒
double t1 = 3600_s;        // → 3600 s
double t2 = 1_h;           // → 3600 s
double t3 = 30_min;        // → 1800 s
double t4 = 1_day;         // → 86400 s
double t5 = 500_ms;        // → 0.5 s

// 速度 → m/s
double v1 = 7.5_km_s;      // → 7500 m/s
double v2 = 100_km_h;      // → 27.78 m/s
double v3 = 1_m_s;         // → 1 m/s

// 角速度 → rad/s
double w1 = 30_deg_s;      // → 0.5236 rad/s
double w2 = 0.1_rad_s;     // → 0.1 rad/s

// 质量 → kg
double m1 = 100_kg;        // → 100 kg
double m2 = 500_g;         // → 0.5 kg

// 面积 → m²
double area = 10_m2;       // → 10 m²

// 常量变量 (在 literals 命名空间中)
constexpr double deg = 1_deg;
constexpr double km = 1_km;
constexpr double s = 1_s;
// ...
```

---

## 10. 对象系统 (RTTI)

```cpp
// 定义在 src/AstUtil/RTTI/Object.hpp
// Object 是所有支持RTTI的对象基类，继承自 Referenced (引用计数)

// 声明 RTTI 类
class MyClass : public ObjectNamed {
public:
    AST_OBJECT(MyClass)           // RTTI宏
    AST_PROPERT(ForceModel)       // 属性声明
    AST_PROPERT(Integrator)
    // ...
};

// 实现 RTTI (在 .rtti.cpp 中自动生成或手写)
// _AST_IMPL_OBJECT(MyClass)  // Class MyClass::staticType;

// 智能指针
ScopedPtr<T>   // 独占所有权 (类似 unique_ptr)
SharedPtr<T>   // 共享所有权 (类似 shared_ptr)
WeakPtr<T>     // 弱引用

// 对象标志
enum class EObjectFlags: uint32_t {
    eNone, eReadOnly, eInActive, eComponent, eEntity
};

// 使用 typedef 简化指针类型
using PFrameAssembly = FrameAssembly*;
using HFrameAssembly = SharedPtr<FrameAssembly>;
```

---

## 11. xmake 构建配置

### 11.1 库目标模板

```lua
-- src/AstCore/xmake.lua
target("AstCore")
    set_kind("shared")                          -- 共享库
    add_files("**.cpp")                         -- 所有cpp文件
    add_headerfiles("**.hpp", {prefixdir="AstCore"})  -- 头文件安装
    add_headerfiles("**.h", {prefixdir="AstCore"})
    add_defines("AST_BUILD_LIB_CORE")           -- 编译定义
    add_deps("AstUtil", "AstMath", "AstWeather", "AstScript")  -- 依赖
target_end()
```

### 11.2 示例目标

```lua
-- examples/xmake.lua
-- 每个cpp文件自动编译为独立可执行文件
add_deps("AstUtil", "AstCore", "AstMath", "AstSim", "AstTest", ...)
```

### 11.3 常用编译选项

- `--with_test=true` 编译测试
- `--with_examples=true` 编译示例
- `--with_projects=true` 编译项目
- `--check_warnings=true` 警告作为错误
- `--mode=debug` / `--mode=release` / `--mode=releasedbg`
- Unity Build 已开启 (batchsize=20)

---

## 12. 测试框架

```cpp
// 基于 GoogleTest，定义在 include/AstTest/AstTestMacro.h
#include "ast/AstTestMacro.h"
AST_USING_NAMESPACE

// 测试用例
TEST(OrbitElement, CartConversion) {
    OrbElem coe = {7000000.0, 0.01, 0.1, 0.2, 0.3, 0.4};
    Vector3d pos, vel;
    errc_t err = aOrbElemToCart(coe, GM, pos, vel);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(result, expected, EPS);
}

// 主函数
GTEST_MAIN()
```

---

## 13. 标准代码模板

### 13.1 示例程序模板

```cpp
#include "ast/OrbitElement.hpp"
#include "ast/Vector.hpp"
#include "ast/Literals.hpp"
#include "ast/Constants.h"
#include <iostream>

AST_USING_NAMESPACE
using namespace _AST literals;

int main()
{
    // 1. 初始化运行时（如果需要EOP、星历等数据）
    aInitialize();

    // 2. 定义物理量
    const double gm = kEarthGrav;
    Vector3d pos{7000_km, 0.0, 0.0};
    Vector3d vel{0.0, 7.5_km_s, 0.0};

    // 3. 计算
    OrbElem elem;
    errc_t err = aCartToOrbElem(pos, vel, gm, elem);
    if (err != eNoError) {
        std::cerr << "Error: " << err << std::endl;
        aUninitialize();
        return -1;
    }

    // 4. 输出结果
    std::cout << "长半轴: " << elem.a() << " m" << std::endl;
    std::cout << "偏心率: " << elem.e() << std::endl;

    // 5. 清理
    aUninitialize();
    return 0;
}
```

### 13.2 新头文件模板

```cpp
// src/AstCore/SomeModule/SomeClass.hpp

#pragma once

#include "AstGlobal.h"
#include "AstCore/SomeDependency.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup SomeGroup
    @{
*/

/// @brief 类的简要描述
class AST_CORE_API SomeClass : public ObjectNamed
{
public:
    AST_OBJECT(SomeClass)
    AST_PROPERT(SomeProperty)

    SomeClass();
    ~SomeClass() override;

    /// @brief 方法描述
    /// @param param 参数说明 [单位]
    /// @return 返回值说明
    errc_t doSomething(double param);

    /// @brief 获取属性
    double getValue() const { return value_; }
    /// @brief 设置属性
    void setValue(double value) { value_ = value; }

PROPERTIES:
    AST_DEF_ACCESS_METHOD(double, value)

protected:
    double value_{0.0};    ///< 属性说明
};

/// @brief C API 函数
/// @param input 输入参数
/// @param output 输出参数
/// @return 错误码
AST_CORE_CAPI errc_t aSomeFunction(const Vector3d& input, Vector3d& output);

/*! @} */

AST_NAMESPACE_END
```

### 13.3 新源文件模板

```cpp
// src/AstCore/SomeModule/SomeClass.cpp

#include "SomeClass.hpp"
#include "AstCore/Constants.h"

AST_NAMESPACE_BEGIN

SomeClass::SomeClass() = default;
SomeClass::~SomeClass() = default;

errc_t SomeClass::doSomething(double param)
{
    if (param < 0) {
        return eErrorInvalidParam;
    }
    value_ = param;
    return eNoError;
}

errc_t aSomeFunction(const Vector3d& input, Vector3d& output)
{
    output = input;
    return eNoError;
}

AST_NAMESPACE_END
```

---

## 14. 代码风格要点总结

1. **命名空间**: 所有代码包裹在 `AST_NAMESPACE_BEGIN` / `AST_NAMESPACE_END` 中
2. **C API风格**: 全局函数以 `a` 开头，使用 `AST_xxx_CAPI` 导出，返回 `errc_t`
3. **类成员**: 成员变量尾部下划线，通过 `AST_DEF_ACCESS_METHOD` 或手写 getter/setter
4. **聚合初始化**: POD 类型的成员变量设为 public 仅为了支持聚合初始化，注释标注"不要直接访问"
5. **PROPERTIES 标签**: 用 `PROPERTIES:` 分隔属性区和功能方法区
6. **Doxygen注释**: 所有公共接口必须有 `/// @brief` 注释，参数标注单位
7. **错误处理**: 使用 `errc_t` 返回错误码，`eNoError` 表示成功
8. **字面量**: 优先使用 `_km`, `_deg`, `_s` 等字面量提高可读性
9. **常量**: 使用 `k` 前缀的 `constexpr` 常量，如 `kEarthGrav`, `kPI`
10. **头文件**: 使用 `#pragma once`，转发头文件在 `include/ast/` 下
11. **初始化**: 需要天文数据的程序开头调用 `aInitialize()`，结尾调用 `aUninitialize()`
12. **单位约定**: 内部统一使用 SI 国际单位制（米、秒、弧度、千克等）
