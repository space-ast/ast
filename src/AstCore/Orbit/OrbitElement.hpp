///
/// @file      OrbitElement.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      19.11.2025
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
#include "AstCore/Vector.hpp"
 
AST_NAMESPACE_BEGIN



class CartState
{
public:
    Vector3d pos;   ///< 位置
    Vector3d vel;   ///< 速度
};


class OrbElem
{
public:
    double	a;          ///< 长半轴 
    double	e;          ///< 偏心率 y
    double	inc;        ///< 轨道倾角 
    double	raan;       ///< 升交点赤经 
    double	argper;     ///< 近拱点角
    double	trueA;      ///< 真近点角
};

class ModOrbElem
{
public:
    double rp;         ///<近拱点半径 
    double e;          ///<偏心率 
    double inc;        ///<轨道倾角
    double raan;       ///<升交点赤经
    double argper;     ///<近拱点角
    double trueA;      ///<真近点角
};



class EquinElem
{
public:
    double a;          ///< semimajor axis length
    double h;          ///< e*sin(periArg + raan)   omegabar=periArg + raan
    double k;          ///< e*cos(periArg + raan)
    double p;          ///< tan(i/2)*sin(raan)
    double q;          ///< tan(i/2)*cos(raan)
    double MeanLon;    ///< mean longitude = M + raan + periArg
};

/// @brief 改进春分点轨道根数, 180度奇异
class ModEquinElem
{
public:
    double p;        ///< p = a(1-e^2) 半通径
    double f;        ///< f = e*cos(argper+RAAN)
    double g;        ///< g = e*sin(argper+RAAN)
    double h;        ///< h = tan(i/2)cos(RAAN)
    double k;        ///< k = tan(i/2)sin(RAAN)
    double L;        ///< L = RAAN + argper + f
};


 
 
AST_NAMESPACE_END
 
