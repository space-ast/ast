///
/// @file      InertialAxes.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "InertialAxes.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/Rotation.hpp"
#include "AstUtil/Literals.hpp"

AST_NAMESPACE_BEGIN


using namespace literals;

void aJ2000ToB1950Transform_Vallado(Rotation &rotation)
{
    /*!
    参考GMAT代码 (vallado 2001, 227-228)  
    详细请参见：gmat-git/prototype/Navigation/TDRSS_Doppler/utilities/Vallado/fk4i.m
    */
    rotation.setMatrix({
         0.9999256794956877,   0.0111814832391717,  0.0048590037723143,
        -0.0111814832204662,   0.9999374848933135, -0.000027170293744,
	    -0.0048590038153592,  -0.0000271625947142,  0.9999881946043742,
    });
}

void aJ2000ToB1950Transform_SPICE(Rotation &rotation)
{
    /*!
    参考SPICE程序 src/spicelib/chgirf.for
    */
    static Matrix3d mtx = aEuler323ToMatrix({
         1153.04066200330_arcsec,
        -1002.26108439117_arcsec,
         1152.84248596724_arcsec, 
    });
    rotation.setMatrix(mtx);
}


/*!
    以下转换参考SPICE程序 src/spicelib/chgirf.for
*/

void aB1950ToFK4Transform(Rotation &rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.525_arcsec);
    rotation.setMatrix(mtx);
}

void aB1950ToDE118Transform(Rotation &rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.53155_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToDE96Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.4107_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToDE102Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.1359_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToDE108Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.4775_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToDE111Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.5880_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToDE114Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.5529_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToDE122Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.5316_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToDE125Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.5754_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToDE130Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationZMatrix(0.5247_arcsec);
    rotation.setMatrix(mtx);
}


void aFK4ToGalacticTransform(Rotation& rotation)
{
    static Matrix3d mtx = aEuler313ToMatrix({
        1016100.0_arcsec,
        225360.0_arcsec,
        1177200.0_arcsec,
    });
    rotation.setMatrix(mtx);
}


void aJ2000ToDE200Transform(Rotation& rotation)
{
    rotation = Rotation::Identity();
}


void aJ2000ToDE202Transform(Rotation& rotation)
{
    rotation = Rotation::Identity();
}


void aJ2000ToMarsIAUTransform(Rotation& rotation)
{
    static Matrix3d mtx = aEuler323ToMatrix({
        -152348.4e0_arcsec,
        133610.4e0_arcsec,
        324000.0e0_arcsec,
    });
    rotation.setMatrix(mtx);
}


void aJ2000ToEclipJ2000Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationXMatrix(84381.448_arcsec);
    rotation.setMatrix(mtx);
}


void aB1950ToEclipB1950Transform(Rotation& rotation)
{
    static Matrix3d mtx = aRotationXMatrix(84404.836_arcsec);
    rotation.setMatrix(mtx);
}


void aJ2000ToDE140Transform(Rotation& rotation)
{
    static Matrix3d mtx = aEuler323ToMatrix({
        1153.75719544491_arcsec,
        -1002.25042010533_arcsec,
        1152.71013777252_arcsec,
    });
    rotation.setMatrix(mtx);
}


void aJ2000ToDE142Transform(Rotation& rotation)
{
    static Matrix3d mtx = aEuler323ToMatrix({
        1153.74663857521_arcsec,
        -1002.25052830351_arcsec,
        1152.72061453864_arcsec,
    });
    rotation.setMatrix(mtx);
}


void aJ2000ToDE143Transform(Rotation& rotation)
{
    static Matrix3d mtx = aEuler323ToMatrix({
        1153.42900222357_arcsec,
        -1002.24822382286_arcsec,
        1153.03919093833_arcsec,
    });
    rotation.setMatrix(mtx);
}


AST_NAMESPACE_END
