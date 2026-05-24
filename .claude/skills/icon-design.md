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
  | 蓝灰 | `#5A8BAC` | 抽象/泛用概念（如 Segment） |
- **画布**：`32x32` viewBox，重心居中，对称优先
- **线宽**：主轮廓 1.5-2px，辅助元素 1-1.5px
- **留白**：主体占画布 70-80%
- **元素数**：单图标不超过 3 个图形元素

## SVG 模板

```xml
<?xml version="1.0" encoding="UTF-8"?>
<svg width="32" height="32" xmlns="http://www.w3.org/2000/svg" xmlns:svg="http://www.w3.org/2000/svg">
  <g class="layer">
    <title>Layer 1</title>
    <!-- 图形元素 -->
  </g>
  <defs>
    <marker id="arrow" viewBox="0 0 10 10" refX="10" refY="5"
            markerWidth="4" markerHeight="4" orient="auto">
      <line x1="4" y1="1" x2="10" y2="5" stroke="#3399FF" stroke-width="1.5" stroke-linecap="round"/>
      <line x1="4" y1="9" x2="10" y2="5" stroke="#3399FF" stroke-width="1.5" stroke-linecap="round"/>
    </marker>
  </defs>
</svg>
```

**惯例**：
- 路径用 `m...` 小写相对坐标，方便对称计算
- 先画辅助/背景元素，再画前景主体
- 火焰/推力用曲线 `C`，几何形状用折线 `L`
- marker 用于路径末端箭头，`orient="auto"` 自动对齐切线

## 已有图标

| 文件 | 用途 | 关键设计 |
|------|------|----------|
| `InitialState.svg` | 初始状态段 | 旗标：竖杆+三角旗+基点圆 |
| `Propagate.svg` | 轨道预报段 | 从中心出发的逆时针螺旋线+C曲线+末端箭头 |
| `Maneuver.svg` | 机动段 | 轨道弧线+推力矢量+机动点 |
| `Burn.svg` | 推力器 | 拉瓦尔喷管4段灰阶+内外双层火焰，整体旋转45° |
| `Sequence.svg` | 序列段 | 阶梯方块+对角线方向箭头 |
| `TargeterSequence.svg` | 打靶序列段 | 同心圆靶环+琥珀靶心 |
| `Segment.svg` | 泛用轨道段 | 轨道弧线+两端琥珀端点圆 |

## 代码集成

### 新增图标后
1. 将 SVG 放入 `data/icons/`
2. 运行 `xmake cpdata` 拷贝到构建输出目录
3. 各控件通过 `missionIcon("图标名")` 加载

### missionIcon 工具
定义在 `src/AstGUI/Mission/MissionIcons.hpp`：
```cpp
inline QIcon missionIcon(const QString& name) {
    QString path = QCoreApplication::applicationDirPath()
                   + "/data/icons/" + name + ".svg";
    return QIcon(path);
}
```

### 控件中使用
```cpp
// 树节点图标
item->setIcon(0, missionIcon("InitialState"));

// 菜单/工具栏动作图标
action->setIcon(missionIcon("Propagate"));
```

## 设计流程

1. 先查看参考图标 `data/icons/reference/{atk,stk,gmat}/`
2. 用最简单的几何形状表达核心概念
3. 保持和已有图标集风格一致
4. 对称检查：所有坐标关于 x=16 镜像验证
5. 缩放到 16x16 仍可辨识
