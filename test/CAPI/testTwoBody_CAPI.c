/// @file      testTwoBody.c
/// @brief     
/// @details   ~
/// @author    axel
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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
 
#include "AstCore/AstCoreC.h" 
#ifdef __cplusplus
#include "AstCore/Vector.hpp"
#include "AstCore/Matrix.hpp"
#endif
#include <stdio.h>

int main()
{
	AVector3d pos = { 6678137.000000000000, 0, 0 };
	AVector3d vel = { 0, 6789.5302977176515924, 3686.4141730136516344 };
	double dt = 86400;
	double gm = 3.986004418e14;
	aTwoBodyProp(dt, gm, &pos, &vel);
	AVector3d posExpect = {
			5596646.34376213,
			-3201966.68063242,
			-1738526.05930174
	};
	AVector3d velExpect = {
		4215.06430913537,
		5690.00005775394,
		3089.41796230054
	};
	printf("x,y,z=%lf, %lf, %lf\n", pos.x_, pos.y_, pos.z_);


    return 0;
}