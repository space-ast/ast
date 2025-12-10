///
/// @file      Color.hpp
/// @brief     颜色工具类
/// @details   提供颜色值的预定义枚举和常量，方便在代码中使用。
/// @author    jinke18
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


typedef uint32_t Color;

/// @brief 将 RGB 颜色值转换为 Color 类型
/// @param r 红色分量（0-255）
/// @param g 绿色分量（0-255）
/// @param b 蓝色分量（0-255）
/// @return 对应的 Color 值
constexpr inline Color rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (r << 24) | (g << 16) | (b << 8) | 0xFF;
}

/// @brief 将 RGBA 颜色值转换为 Color 类型
/// @param r 红色分量（0-255）
/// @param g 绿色分量（0-255）
/// @param b 蓝色分量（0-255）
/// @param a 透明度分量（0-255）
/// @return 对应的 Color 值
constexpr inline Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (r << 24) | (g << 16) | (b << 8) | a;
}

/// @brief 将 Color 类型转换为 RGB 颜色值
/// @param color 输入 Color 值
/// @param r 输出红色分量（0-255）
/// @param g 输出绿色分量（0-255）
/// @param b 输出蓝色分量（0-255）
inline void aColorToRGB(Color color, uint8_t& r, uint8_t& g, uint8_t& b)
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
inline void aColorToRGBA(Color color, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
	r = (color >> 24) & 0xFF;
	g = (color >> 16) & 0xFF;
	b = (color >> 8) & 0xFF;
	a = color & 0xFF;
}

/// @brief 预定义的颜色枚举
/// @see https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Values/named-color
enum EColor{
	// 基础颜色（16个标准颜色）
    eBlack           	    = rgb(0, 0, 0),                  	///< 黑色
    eSilver          	    = rgb(192, 192, 192),            	///< 银色
    eGray            	    = rgb(128, 128, 128),            	///< 灰色
    eWhite           	    = rgb(255, 255, 255),            	///< 白色
    eMaroon          	    = rgb(128, 0, 0),                	///< 栗色
    eRed             	    = rgb(255, 0, 0),                	///< 红色
    ePurple          	    = rgb(128, 0, 128),              	///< 紫色
    eFuchsia         	    = rgb(255, 0, 255),              	///< 紫红色
    eGreen           	    = rgb(0, 128, 0),                	///< 绿色
    eLime            	    = rgb(0, 255, 0),                	///< 酸橙色
    eOlive           	    = rgb(128, 128, 0),              	///< 橄榄色
    eYellow          	    = rgb(255, 255, 0),              	///< 黄色
    eNavy            	    = rgb(0, 0, 128),                	///< 藏青色
    eBlue            	    = rgb(0, 0, 255),                	///< 蓝色
    eTeal            	    = rgb(0, 128, 128),              	///< 蓝绿色
    eAqua            	    = rgb(0, 255, 255),              	///< 水蓝色

    // 特殊颜色 
    eTransparent     	    = rgba(0, 0, 0, 0),              	///< 透明
    
    // 扩展颜色（按字母排序）
    eAquaMarine      	    = rgb(127, 255, 212),            	///< 海绿色
    eAliceBlue       	    = rgb(240, 248, 255),            	///< 爱丽丝蓝
    eAntiqueWhite    	    = rgb(250, 235, 215),            	///< 古董白
    eAzure           	    = rgb(240, 255, 255),            	///< 天蓝色
    eBeige           	    = rgb(245, 245, 220),            	///< 米色
    eBisque          	    = rgb(255, 228, 196),            	///< 陶土色
    eBlanchedAlmond  	    = rgb(255, 235, 205),            	///< 杏仁白
    eBlueViolet      	    = rgb(138, 43, 226),             	///< 蓝紫色
    eBrown           	    = rgb(165, 42, 42),              	///< 棕色
    eBurlyWood       	    = rgb(222, 184, 135),            	///< 硬木色
    eCadetBlue       	    = rgb(95, 158, 160),             	///< 军蓝色
    eChartreuse      	    = rgb(127, 255, 0),              	///< 黄绿色
    eChocolate       	    = rgb(210, 105, 30),             	///< 巧克力色
    eCoral           	    = rgb(255, 127, 80),             	///< 珊瑚色
    eCornflowerBlue  	    = rgb(100, 149, 237),            	///< 矢车菊蓝
    eCornsilk        	    = rgb(255, 248, 220),            	///< 玉米丝色
    eCrimson         	    = rgb(220, 20, 60),              	///< 深红色
    eCyan            	    = eAqua,                             ///< 青色（与 eAqua 相同）
    eDarkBlue        	    = rgb(0, 0, 139),                	///< 深蓝色
    eDarkCyan        	    = rgb(0, 139, 139),              	///< 深青色
    eDarkGoldenRod   	    = rgb(184, 134, 11),             	///< 深金黄色
    eDarkGray        	    = rgb(169, 169, 169),            	///< 深灰色
    eDarkGreen       	    = rgb(0, 100, 0),                	///< 深绿色
    eDarkGrey        	    = rgb(169, 169, 169),            	///< 深灰色（与 eDarkGray 相同）
    eDarkKhaki       	    = rgb(189, 183, 107),            	///< 深卡其色
    eDarkMagenta     	    = rgb(139, 0, 139),              	///< 深洋红色
    eDarkOliveGreen  	    = rgb(85, 107, 47),              	///< 深橄榄绿
    eDarkOrange      	    = rgb(255, 140, 0),              	///< 深橙色
    eDarkOrchid      	    = rgb(153, 50, 204),             	///< 深紫色
    eDarkRed         	    = rgb(139, 0, 0),                	///< 深红色
    eDarkSalmon      	    = rgb(233, 150, 122),            	///< 深鲑鱼色
    eDarkSeaGreen    	    = rgb(143, 188, 143),            	///< 深海绿色
    eDarkSlateBlue   	    = rgb(72, 61, 139),              	///< 深石板蓝
    eDarkSlateGray   	    = rgb(47, 79, 79),               	///< 深石板灰
    eDarkSlateGrey   	    = rgb(47, 79, 79),               	///< 深石板灰（与 eDarkSlateGray 相同）
    eDarkTurquoise   	    = rgb(0, 206, 209),              	///< 深青绿色
    eDarkViolet      	    = rgb(148, 0, 211),              	///< 深紫色
    eDeepPink        	    = rgb(255, 20, 147),             	///< 深粉色
    eDeepSkyBlue     	    = rgb(0, 191, 255),              	///< 深天蓝色
    eDimGray         	    = rgb(105, 105, 105),            	///< 暗灰色
    eDimGrey         	    = rgb(105, 105, 105),            	///< 暗灰色（与 eDimGray 相同）
    eDodgerBlue      	    = rgb(30, 144, 255),             	///< 道奇蓝
    eFireBrick       	    = rgb(178, 34, 34),              	///< 火砖红
    eFloralWhite     	    = rgb(255, 250, 240),            	///< 花白色
    eForestGreen     	    = rgb(34, 139, 34),              	///< 森林绿
    eGainsboro       	    = rgb(220, 220, 220),            	///< 浅灰色
    eGhostWhite      	    = rgb(248, 248, 255),            	///< 幽灵白
    eGold            	    = rgb(255, 215, 0),              	///< 金色
    eGoldenRod       	    = rgb(218, 165, 32),             	///< 金棕色
    eGreenYellow     	    = rgb(173, 255, 47),             	///< 绿黄色
    eGrey            	    = rgb(128, 128, 128),            	///< 灰色（与 eGray 相同）
    eHoneydew        	    = rgb(240, 255, 240),            	///< 蜜瓜色
    eHotPink         	    = rgb(255, 105, 180),            	///< 亮粉色
    eIndianRed       	    = rgb(205, 92, 92),              	///< 印度红
    eIndigo          	    = rgb(75, 0, 130),               	///< 靛蓝色
    eIvory           	    = rgb(255, 255, 240),            	///< 象牙白
    eKhaki           	    = rgb(240, 230, 140),            	///< 卡其色
    eLavender        	    = rgb(230, 230, 250),            	///< 淡紫色
    eLavenderBlush   	    = rgb(255, 240, 245),            	///< 淡紫红
    eLawnGreen       	    = rgb(124, 252, 0),              	///< 草坪绿
    eLemonChiffon    	    = rgb(255, 250, 205),            	///< 柠檬绸色
    eLightBlue       	    = rgb(173, 216, 230),            	///< 浅蓝色
    eLightCoral      	    = rgb(240, 128, 128),            	///< 浅珊瑚色
    eLightCyan       	    = rgb(224, 255, 255),            	///< 浅青色
    eLightGoldenRodYellow   = rgb(250, 250, 210),              ///< 浅金棕色
    eLightGray       	    = rgb(211, 211, 211),            	///< 浅灰色
    eLightGreen      	    = rgb(144, 238, 144),            	///< 浅绿色
    eLightGrey       	    = rgb(211, 211, 211),            	///< 浅灰色（与 eLightGray 相同）
    eLightPink       	    = rgb(255, 182, 193),            	///< 浅粉色
    eLightSalmon     	    = rgb(255, 160, 122),            	///< 浅鲑鱼色
    eLightSeaGreen   	    = rgb(32, 178, 170),             	///< 浅海绿色
    eLightSkyBlue    	    = rgb(135, 206, 250),            	///< 浅天蓝色
    eLightSlateGray  	    = rgb(119, 136, 153),            	///< 浅石板灰
    eLightSlateGrey  	    = rgb(119, 136, 153),            	///< 浅石板灰（与 eLightSlateGray 相同）
    eLightSteelBlue  	    = rgb(176, 196, 222),            	///< 亮钢蓝
    eLightYellow     	    = rgb(255, 255, 224),            	///< 浅黄色
    eLimeGreen       	    = rgb(50, 205, 50),              	///< 酸橙绿
    eLinen           	    = rgb(250, 240, 230),            	///< 亚麻色
    eMagenta         	    = eFuchsia,               			 ///< 品红色（与 eFuchsia 相同）
    eMediumAquaMarine  	    = rgb(102, 205, 170),           	///< 中海洋绿
    eMediumBlue      	    = rgb(0, 0, 205),                	///< 中蓝色
    eMediumOrchid    	    = rgb(186, 85, 211),             	///< 中兰紫色
    eMediumPurple    	    = rgb(147, 112, 219),            	///< 中紫色
    eMediumSeaGreen  	    = rgb(60, 179, 113),             	///< 中海绿
    eMediumSlateBlue 	    = rgb(123, 104, 238),            	///< 中石板蓝
    eMediumSpringGreen      = rgb(0, 250, 154),            	///< 中春绿
    eMediumTurquoise  	    = rgb(72, 209, 204),             	///< 中青绿色
    eMediumVioletRed  	    = rgb(199, 21, 133),             	///< 中紫红色
    eMidnightBlue    	    = rgb(25, 25, 112),              	///< 午夜蓝
    eMintCream       	    = rgb(245, 255, 250),            	///< 薄荷奶油色
    eMistyRose       	    = rgb(255, 228, 225),            	///< 薄雾玫瑰色
    eMoccasin        	    = rgb(255, 228, 181),            	///< 鹿皮鞋色
    eNavajoWhite     	    = rgb(255, 222, 173),            	///< 纳瓦霍白
    eOldLace         	    = rgb(253, 245, 230),            	///< 老蕾丝色
    eOliveDrab       	    = rgb(107, 142, 35),             	///< 橄榄褐
    eOrange          	    = rgb(255, 165, 0),              	///< 橙色
    eOrangeRed       	    = rgb(255, 69, 0),               	///< 橙红色
    eOrchid          	    = rgb(218, 112, 214),            	///< 兰花紫
    ePaleGoldenRod   	    = rgb(238, 232, 170),            	///< 苍金棕色
    ePaleGreen       	    = rgb(152, 251, 152),            	///< 苍绿色
    ePaleTurquoise   	    = rgb(175, 238, 238),            	///< 苍青色
    ePaleVioletRed   	    = rgb(219, 112, 147),            	///< 苍紫红色
    ePapayaWhip      	    = rgb(255, 239, 213),            	///< 番木瓜色
    ePeachPuff       	    = rgb(255, 218, 185),            	///< 桃肉色
    ePeru            	    = rgb(205, 133, 63),             	///< 秘鲁色
    ePink            	    = rgb(255, 192, 203),            	///< 粉色
    ePlum            	    = rgb(221, 160, 221),            	///< 李子色
    ePowderBlue      	    = rgb(176, 224, 230),            	///< 粉末蓝
    eRebeccaPurple   	    = rgb(102, 51, 153),             	///< 丽贝卡紫
    eRosyBrown       	    = rgb(188, 143, 143),            	///< 玫瑰棕色
    eRoyalBlue       	    = rgb(65, 105, 225),             	///< 皇家蓝
    eSaddleBrown     	    = rgb(139, 69, 19),              	///< 马鞍棕
    eSalmon          	    = rgb(250, 128, 114),            	///< 鲑鱼色
    eSandyBrown      	    = rgb(244, 164, 96),             	///< 沙棕色
    eSeaGreen        	    = rgb(46, 139, 87),              	///< 海绿色
    eSeaShell        	    = rgb(255, 245, 238),            	///< 海贝壳色
    eSienna          	    = rgb(160, 82, 45),              	///< 赭石色
    eSkyBlue         	    = rgb(135, 206, 235),            	///< 天蓝色
    eSlateBlue       	    = rgb(106, 90, 205),             	///< 石板蓝
    eSlateGray       	    = rgb(112, 128, 144),            	///< 石板灰
    eSlateGrey       	    = rgb(112, 128, 144),            	///< 石板灰（与 eSlateGray 相同）
    eSnow            	    = rgb(255, 250, 250),            	///< 雪白色
    eSpringGreen     	    = rgb(0, 255, 127),              	///< 春绿色
    eSteelBlue       	    = rgb(70, 130, 180),             	///< 钢蓝色
    eTan             	    = rgb(210, 180, 140),            	///< 棕褐色
    eThistle         	    = rgb(216, 191, 216),            	///< 蓟色
    eTomato          	    = rgb(255, 99, 71),              	///< 番茄色
    eTurquoise       	    = rgb(64, 224, 208),             	///< 绿松石色
    eViolet          	    = rgb(238, 130, 238),            	///< 紫罗兰色
    eWheat           	    = rgb(245, 222, 179),            	///< 小麦色
    eWhiteSmoke      	    = rgb(245, 245, 245),            	///< 烟白色
    eYellowGreen     	    = rgb(154, 205, 50),             	///< 黄绿色
};


AST_NAMESPACE_END