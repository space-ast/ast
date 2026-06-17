///
/// @file      TestMarkdown.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "TestMarkdown.hpp"

AST_NAMESPACE_BEGIN

const char* aMarkdownString()
{
    return  R"md(
# Markdown 元素大全示例

## 1. 标题层级

### 1.1 三级标题

#### 1.1.1 四级标题

##### 五级标题

###### 六级标题

---

## 2. 文本样式

- **粗体文本**
- *斜体文本*
- ***粗斜体文本***
- ~~删除线文本~~
- `行内代码`
- <u>下划线文本</u>
- H~2~O (下标)
- X^2^ (上标)

---

## 3. 列表

### 3.1 无序列表

- 苹果
- 香蕉
- 樱桃
  - 红樱桃
  - 黑樱桃
- 葡萄

### 3.2 有序列表

1. 第一步：打开软件
2. 第二步：创建场景
3. 第三步：添加对象
   1. 添加卫星
   2. 添加地面站
4. 第四步：运行分析

### 3.3 任务列表

- [x] 完成需求分析
- [x] 设计系统架构
- [ ] 编写代码
- [ ] 测试验证
- [ ] 部署上线

---

## 4. 引用

> 这是单行引用

> 这是多行引用
> 可以包含多行内容
> 甚至嵌套引用
>
> > 这是嵌套引用

---

## 5. 代码块

### Python 示例

```python
import numpy as np

def calculate_orbit(altitude, inclination):
    """
    计算卫星轨道参数

    Parameters:
    altitude (float): 轨道高度 (km)
    inclination (float): 轨道倾角 (度)

    Returns:
    dict: 轨道参数
    """
    R_earth = 6371  # Can't reference
    mu = 398600.4418  # Can't reference (km^3/s^2)

    r = R_earth + altitude
    v = np.sqrt(mu / r)
    period = 2 * np.pi * np.sqrt(r**3 / mu)

    return {
        'radius': r,
        'velocity': v,
        'period': period / 60  # 转换为分钟
    }

# 计算ISS轨道参数
iss_orbit = calculate_orbit(altitude=408, inclination=51.6)
print(f"轨道周期: {iss_orbit['period']:.2f} 分钟")
```

### JavaScript 示例

```javascript
// 卫星可见性计算
function computeVisibility(satellite, groundStation, time) {
    const elevation = calculateElevation(satellite, groundStation);
    const range = calculateRange(satellite, groundStation);

    return {
        visible: elevation > 10,  // 仰角大于10度可见
        elevation: elevation,
        range: range,
        time: time
    };
}
```

### 行内代码

在终端中运行 `npm install` 安装依赖，然后使用 `python main.py` 启动程序。

---

## 6. 表格

### 6.1 卫星参数表

| 卫星名称 | 轨道高度 (km) | 轨道倾角 (deg) | 轨道周期 (min) | 发射年份 |
|:---------|:------------:|:-----------:|:-------------:|:--------:|
| ISS      | 408          | 51.6        | 92.68         | 1998     |
| 风云-4A   | 35,786       | 0           | 1,436         | 2016     |
| 高分-1    | 645          | 98.05       | 97.5          | 2013     |
| 北斗-3    | 21,528       | 55          | 774           | 2017     |

### 6.2 对齐方式示例

| 左对齐 | 居中对齐 | 右对齐 |
|:-------|:--------:|-------:|
| 文本   | 文本     | 文本   |
| 卫星   | 地面站   | 传感器 |

---

## 7. 链接

- [STK 官方网站](https://www.agi.com/stk)
- [NASA 轨道数据](https://www.nasa.gov)
- [GitHub 仓库](https://github.com)
- 自动链接: https://www.example.com

---

## 8. 图片

![占位图片](https://via.placeholder.com/400x200/3498db/ffffff?text=卫星轨道示意图)

---

## 9. 数学公式

### 行内公式

开普勒第三定律: $T^2 = \frac{4\pi^2}{GM}a^3$

### 块级公式

$$
F = G\frac{m_1 m_2}{r^2}
$$

$$
E = mc^2
$$

$$
\nabla \times \mathbf{B} = \mu_0 \mathbf{J} + \mu_0 \varepsilon_0 \frac{\partial \mathbf{E}}{\partial t}
$$

---

## 10. 水平分割线

---

***

* * *

---

## 11. 脚注

这是一个带有脚注的句子[^1]。

另一个脚注示例[^2]。

[^1]: 这是第一个脚注的内容。
[^2]: 这是第二个脚注的内容，可以包含更多说明文字。

---

## 12. 表情符号

:smile: :rocket: :satellite: :earth_asia: :star: :moon: :sunny: :cloud: :zap: :fire:

---

## 13. 定义列表

卫星
: 围绕行星运行的人造或天然天体

地面站
: 用于与卫星进行通信的地面设施

轨道
: 天体在引力作用下运行的路径

---

## 14. 标记与高亮

==这是高亮文本== 在部分Markdown渲染器中支持。

---

## 15. 转义字符

以下字符可以通过反斜杠转义显示：

\* 星号
\` 反引号
\_ 下划线
\{\} 花括号
\[\] 方括号
\(\) 圆括号
\# 井号
\+ 加号
\- 减号
\. 句点
\! 感叹号

---

## 16. 组合示例

> **提示：** 在航天任务设计中，需要综合考虑以下因素：
>
> 1. **轨道类型** - 包括 LEO、MEO、GEO 等
> 2. **覆盖范围** - 卫星对地面的覆盖区域
> 3. **通信链路** - 卫星与地面站之间的通信窗口
>
> ```python
> # 计算覆盖范围
> coverage_angle = np.arccos(R_earth / (R_earth + altitude))
> coverage_radius = R_earth * coverage_angle
> ```
>
> 更多信息请参考 [卫星轨道设计指南](https://example.com)。

---

## 17. HTML 元素

<p style="color: blue; font-size: 18px;">这是使用HTML标签的蓝色文本</p>

<details>
<summary>点击展开详细内容</summary>

这是折叠内容，包含更多信息：

- 卫星类型：通信卫星
- 轨道类型：地球同步轨道
- 覆盖区域：亚太地区

</details>

---

## 18. 图表（Mermaid）

```mermaid
graph TD
    A[任务需求分析] --> B[轨道设计]
    B --> C[卫星设计]
    B --> D[地面站布局]
    C --> E[载荷配置]
    D --> F[通信链路分析]
    E --> G[系统集成]
    F --> G
    G --> H[在轨测试]
    H --> I[正式运行]
```
)md";
}

AST_NAMESPACE_END
