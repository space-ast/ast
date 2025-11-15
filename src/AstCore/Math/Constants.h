///
/// @file      Constants.h
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.hpp"
 
AST_NAMESPACE_BEGIN
 

constexpr double kJ2000Epoch = 2451545.0;
constexpr double kB1950Epoch = 2433282.4234591;
constexpr double kJ1950Epoch = 2433282.5;

constexpr double kPI         =  3.141592653589793238462643383279502884197169399375105;  ///< PI
constexpr double kTwoPI      =  6.283185307179586476925286766558	                 ;	///< 2*PI
constexpr double kFourPI     =  12.566370614359172953850573533116	                 ;	///< 4*PI
constexpr double kHalfPI     =  1.5707963267948966192313216916395	                 ;	///< 0.5*kAsPI
constexpr double kThirdPI    =  1.047197551196597746154214461093	                 ;	///< PI/3
constexpr double kQuarterPI  =  0.78539816339744830961566084581975	                 ;	///< PI/4
constexpr double kSqrPI      =  9.869604401089358618834490999873	                 ;	///< PI*PI, PI squared



AST_NAMESPACE_END
