---
name: icon-design
description: 航天任务软件图标设计与集成。生成新图标、修改现有图标、或将图标集成到 Qt 界面部件中时使用。
arg-model: prompt
---

# 图标设计技能

为 ast 项目创建和维护图标。图标源文件为 32x32 SVG，存于 `data/icons/`。

## 风格规范

- **扁平极简**：无渐变、无阴影、无高光，符号化几何图形
- **色板**：
  | 颜色 | 色值 | 用途 |
  |------|------|------|
  | 天蓝 | `#3399FF` | 航天器、传感器、轨道元素 |
  | 鲜绿 | `#33CC33` | 地面设施（暂未使用） |
  | 琥珀 | `#FF9933` | 机动、火焰、高亮标记 |
  | 蓝灰 | `#5A8BAC` | 抽象/泛用概念（如 Axes、Segment） |
- **画布**：`32x32` viewBox，重心居中，对称优先
- **线宽**：主轮廓 1.5-2px，辅助元素 1-1.5px
- **留白**：主体占画布 70-80%
- **元素数**：单图标不超过 3 个图形元素

## 重要：禁止使用 SVG marker

**Qt 的 SVG 渲染器不支持 `<marker>` 元素**，箭头必须用显式 `<polygon>` + `transform` 实现。

### 箭头模板

```xml
<!-- 标准箭头：6px长、3.5px底宽，挂载点在中点 -->
<polygon points="-3,-1.75 3,0 -3,1.75" fill="#3399FF" transform="translate(x,y) rotate(deg)"/>
```

`translate(x,y)` 为箭头挂载点（线段的终点），`rotate(deg)` 为线段方向角度：
- 向右：`rotate(0)`
- 向上：`rotate(-90)`
- 向左：`rotate(180)`
- 向下：`rotate(90)`
- 右上方45°：`rotate(-45)`

## SVG 模板

```xml
<?xml version="1.0" encoding="UTF-8"?>
<svg width="32" height="32" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg">
  <g class="layer">
    <title>Layer 1</title>
    <!-- 图形元素 -->
  </g>
</svg>
```

**惯例**：
- 路径用 `m...` 小写相对坐标，方便对称计算
- 先画辅助/背景元素，再画前景主体
- 火焰/推力用曲线 `C`，几何形状用折线 `L`
- 箭头用 `<polygon>` + `transform`，禁止用 `<marker>`

## 已有图标

| 文件 | 用途 | 关键设计 |
|------|------|----------|
| `InitialState.svg` | 初始状态段 | 旗标：竖杆+三角旗+基点圆 |
| `Propagate.svg` | 轨道预报段 | 从中心出发的逆时针螺旋线+C曲线+末端箭头 |
| `Maneuver.svg` | 机动段 | 轨道弧线+推力矢量+机动点 |
| `Sequence.svg` | 序列段 | 阶梯方块+对角线方向箭头 |
| `TargeterSequence.svg` | 打靶序列段 | 同心圆靶环+琥珀靶心 |
| `Segment.svg` | 泛用轨道段 | 轨道弧线+两端琥珀端点圆 |
| `Object.svg` | 通用对象 | 等轴测立方体，顶面淡填充 |
| `Body.svg` | 天体 | 轨道环+实心球体 |
| `Force.svg` | 力模型 | 矢量箭头+尾端圆点 |
| `Frame.svg` | 坐标系 | 三轴+X-Z参考平面 |
| `Axes.svg` | 轴集合 | 三根轴线+箭头 |
| `Point.svg` | 点 | 虚线十字准线+实心点 |
| `Vector.svg` | 矢量 | 带箭头线段+尾端圆点 |
| `Plane.svg` | 平面 | 平行四边形+法向量 |
| `Angle.svg` | 角度 | 顶点+两条边+圆弧 |
| `Spacecraft.svg` | 航天器 | — |
| `Satellite.svg` | 卫星 | — |
| `Burn.svg` | 推力器 | 拉瓦尔喷管4段灰阶+内外双层火焰，整体旋转45° |

## 代码集成

### 新增图标后
1. 将 SVG 放入 `data/icons/`
2. 运行 `xmake cpdata` 拷贝到构建输出目录
3. 控件通过 `loadIcon("图标名")` 或 `missionIcon("图标名")` 加载

### 图标加载工具

`missionIcon`（定义在 `src/AstGUI/Mission/MissionIcons.hpp`）：
```cpp
inline QIcon missionIcon(const QString& name) {
    QString path = QCoreApplication::applicationDirPath()
                   + "/data/icons/" + name + ".svg";
    return QIcon(path);
}
```

`objectIcon`（定义在 `src/AstGUI/Foundation/ObjectIcons.hpp`）：
根据对象 RTTI 类型名自动匹配对应图标，未匹配回退到 `Object` 图标。

### 控件中使用
```cpp
// 树节点图标
item->setIcon(0, objectIcon(obj));

// 菜单/工具栏动作图标
action->setIcon(missionIcon("Propagate"));
```

## 设计流程

1. 先查看参考图标 `data/icons/reference/{atk,stk,gmat}/`
2. 用最简单的几何形状表达核心概念
3. 保持和已有图标集风格一致
4. 对称检查：所有坐标关于 x=16 镜像验证
5. 缩放到 16x16 仍可辨识
6. **箭头用 `<polygon>` + `transform`，禁止用 `<marker>`（Qt 不识别）**
