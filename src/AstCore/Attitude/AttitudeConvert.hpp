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

/// @brief 四元数转矩阵
/// @param quat 
/// @param mtx 
AST_CORE_CAPI void aQuatToMatrix(const Quaternion& quat, Matrix3d& mtx);


/// @brief 矩阵转四元数
/// @param mtx 
/// @param quat 
AST_CORE_CAPI void aMatrixToQuat(const Matrix3d& mtx, Quaternion& quat);


/// @brief 矩阵转欧拉角
/// @param mtx 矩阵
/// @param seq 转序
/// @param euler 欧拉角
AST_CORE_CAPI void aMatrixToEuler(const Matrix3d&mtx, int seq, Euler& euler);
AST_CORE_CAPI void aEulerToMatrix(const Euler&euler, int seq, Matrix3d& mtx);

AST_CORE_CAPI void aMatrixToEuler123(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler132(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler213(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler231(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler312(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler321(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler121(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler131(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler212(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler232(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler313(const Matrix3d& mtx, Euler& euler);
AST_CORE_CAPI void aMatrixToEuler323(const Matrix3d& mtx, Euler& euler);

AST_NAMESPACE_END
