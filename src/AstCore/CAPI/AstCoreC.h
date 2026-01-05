///
/// @file      AstCoreC.h
/// @brief     AstCore的C语言接口，确保与C++的实现二进制兼容
/// @details   因为CAPI使用较少，该文件不会包含所有功能，只会按需增加
/// @author    axel
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

#if !defined __cplusplus

typedef struct AVector3d
{
	double x_, y_, z_;
} AVector3d;

typedef struct AMatrix3d
{
	double data_[3][3];
} AMatrix3d;

#endif


AST_CORE_CAPI int aTwoBodyProp(double dt, double gm, AVector3d* pos, AVector3d* vel);
