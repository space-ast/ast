///
/// @file      AttitudeConvert.hpp
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

void aQuatToMatrix(const Quaternion& quat, Matrix3d& mtx);
void aMatrixToQuat(const Matrix3d& mtx, Quaternion& quat);


void aMatrixToEuler(const Matrix3d&mtx, int seq, Euler& euler);
void aEulerToMatrix(const Euler&euler, int seq, Matrix3d& mtx);

void aMatrixToEuler123(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler132(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler213(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler231(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler312(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler321(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler121(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler131(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler212(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler232(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler313(const Matrix3d& mtx, Euler& euler);
void aMatrixToEuler323(const Matrix3d& mtx, Euler& euler);

AST_NAMESPACE_END
