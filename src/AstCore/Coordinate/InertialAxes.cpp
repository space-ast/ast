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

void aJ2000ToB1950Matrix(Matrix3d &m)
{
    /*!
    参考GMAT代码 (vallado 2001, 227-228)  
    详细请参见：gmat-git/prototype/Navigation/TDRSS_Doppler/utilities/Vallado/fk4i.m
    */
    m = {
         0.9999256794956877,   0.0111814832391717,  0.0048590037723143,
        -0.0111814832204662,   0.9999374848933135, -0.000027170293744,
	    -0.0048590038153592,  -0.0000271625947142,  0.9999881946043742,
    };
}

void aJ2000ToB1950Transform(Rotation &rotation)
{
    aJ2000ToB1950Matrix(rotation.getMatrix());
}

void aB1950ToJ2000Matrix(Matrix3d &m)
{
    m = {
        0.9999256794956877,  -0.0111814832204662, -0.0048590038153592,
        0.0111814832391717,   0.9999374848933135, -0.0000271625947142,
        0.0048590037723143,  -0.000027170293744,   0.9999881946043742,
    };
}

void aB1950ToJ2000Transform(Rotation &rotation)
{
    aB1950ToJ2000Matrix(rotation.getMatrix());
}

void aJ2000ToB1950Matrix_SPICE(Matrix3d &m)
{
    /*!
    参考SPICE程序 src/spicelib/chgirf.for
    */
    static Matrix3d mtx = aEuler323ToMatrix({
         1153.04066200330_arcsec,
        -1002.26108439117_arcsec,
         1152.84248596724_arcsec, 
    });
    m = mtx;
}

void aJ2000ToB1950Transform_SPICE(Rotation &rotation)
{
    aJ2000ToB1950Matrix_SPICE(rotation.getMatrix());
}

void aB1950ToJ2000Matrix_SPICE(Matrix3d &m)
{
    aJ2000ToB1950Matrix_SPICE(m);
    m.transposeInPlace();
}

void aB1950ToJ2000Transform_SPICE(Rotation &rotation)
{
    aB1950ToJ2000Matrix_SPICE(rotation.getMatrix());
}

void aB1950ToFK4Transform(Rotation &rotation)
{
    /*!
    参考SPICE程序 src/spicelib/chgirf.for
    */
    static Matrix3d mtx = aRotationZMatrix(0.525_arcsec);
    rotation.setMatrix(mtx);
}



AST_NAMESPACE_END
