///
/// @file      Color.hpp
/// @brief     颜色工具类
/// @details   提供颜色值的预定义枚举和常量，方便在代码中使用。
/// @author    axel
/// @date      2025-12-09
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include <stdint.h>

AST_NAMESPACE_BEGIN

typedef uint32_t color_t;


/// @brief 将 RGB 颜色值转换为 Color 类型
/// @param r 红色分量（0-255）
/// @param g 绿色分量（0-255）
/// @param b 蓝色分量（0-255）
/// @return 对应的 Color 值
constexpr inline color_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (r << 24) | (g << 16) | (b << 8) | 0xFF;
}

/// @brief 将 RGBA 颜色值转换为 Color 类型
/// @param r 红色分量（0-255）
/// @param g 绿色分量（0-255）
/// @param b 蓝色分量（0-255）
/// @param a 透明度分量（0-255）
/// @return 对应的 Color 值
constexpr inline color_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (r << 24) | (g << 16) | (b << 8) | a;
}

/// @brief 将 Color 类型转换为 RGB 颜色值
/// @param color 输入 Color 值
/// @param r 输出红色分量（0-255）
/// @param g 输出绿色分量（0-255）
/// @param b 输出蓝色分量（0-255）
constexpr inline void aColorToRGB(color_t color, uint8_t& r, uint8_t& g, uint8_t& b) noexcept
{
	r = (color >> 24) & 0xFF;
	g = (color >> 16) & 0xFF;
	b = (color >> 8) & 0xFF;
}

/// @brief 将 Color 类型转换为 RGBA 颜色值
/// @param color 输入 Color 值
/// @param r 输出红色分量（0-255）
/// @param g 输出绿色分量（0-255）
/// @param b 输出蓝色分量（0-255）
/// @param a 输出透明度分量（0-255）
constexpr inline void aColorToRGBA(color_t color, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) noexcept
{
	r = (color >> 24) & 0xFF;
	g = (color >> 16) & 0xFF;
	b = (color >> 8) & 0xFF;
	a = color & 0xFF;
}


// typedef uint32_t Color;
class Color
{
public:
    constexpr Color() noexcept: value_(0) {}
    constexpr Color(color_t value) noexcept: value_(value) {}
    constexpr Color(uint8_t r, uint8_t g, uint8_t b) noexcept: value_(rgb(r, g, b)) {}
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept: value_(rgba(r, g, b, a)) {}
    constexpr operator color_t() const noexcept { return value_; }
private:
    color_t value_;
};


AST_NAMESPACE_END