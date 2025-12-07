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
 
#include "AstGlobal.h"
 
AST_NAMESPACE_BEGIN

/// @brief 四元数转矩阵
/// @param quat 
/// @param mtx 
AST_MATH_CAPI void aQuatToMatrix(const Quaternion& quat, Matrix3d& mtx);


/// @brief 矩阵转四元数
/// @param mtx 
/// @param quat 
AST_MATH_CAPI void aMatrixToQuat(const Matrix3d& mtx, Quaternion& quat);


/// @brief 矩阵转欧拉角
/// @param mtx 矩阵
/// @param seq 转序
/// @param euler 欧拉角 	
/// 如果旋转顺序为ABC，angle2在区间[-PI/2, PI/2]，其他两个角在区间[-PI,PI]
///	如果旋转顺序为ABA，angle2在区间[0, PI]，其他两个角在区间[-PI,PI]
/// @return
AST_MATH_CAPI err_t aMatrixToEuler(const Matrix3d&mtx, int seq, Euler& euler);

/// @brief 欧拉角转矩阵
/// @param euler 
/// @param seq 
/// @param mtx 
/// @return 
AST_MATH_CAPI err_t aEulerToMatrix(const Euler&euler, int seq, Matrix3d& mtx);
AST_MATH_CAPI err_t _aEulerToMatrix(const Euler& euler, int seq, Matrix3d& mtx);


AST_MATH_CAPI void aMatrixToEuler123(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler132(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler213(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler231(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler312(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler321(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler121(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler131(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler212(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler232(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler313(const Matrix3d& mtx, Euler& euler);
AST_MATH_CAPI void aMatrixToEuler323(const Matrix3d& mtx, Euler& euler);

AST_MATH_CAPI void aEuler123ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler132ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler213ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler231ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler312ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler321ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler121ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler131ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler212ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler232ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler313ToMatrix(const Euler& euler, Matrix3d& mtx);
AST_MATH_CAPI void aEuler323ToMatrix(const Euler& euler, Matrix3d& mtx);


AST_MATH_CAPI err_t aEulerToQuat(const Euler& euler, int seq, Quaternion& quat);
AST_MATH_CAPI err_t aQuatToEuler(const Quaternion& quat, int seq, Euler& euler);


AST_NAMESPACE_END
