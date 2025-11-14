/// @file      test-TwoBody.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 根据 Apache 许可证 2.0 版本（"许可证"）授权；
/// 除非符合许可证，否则不得使用此文件。
/// 您可以在以下网址获取许可证副本：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 除非适用法律要求或书面同意，按"原样"分发的软件
/// 没有任何明示或暗示的担保或条件。
/// 请参阅许可证了解特定语言的权限和限制。
 
 
 

#include "AstCore/TwoBody.hpp"
#include "AstCore/Vector.hpp"
#include "AstTest/AstTestMacro.h"


int main()
{
	AST_USING_NAMESPACE
	{
		Vector3d pos{6678137.000000000000, 0, 0};
		Vector3d vel { 0, 6789.5302977176515924, 3686.4141730136516344 };

		double dt = 86400;
		double gm = 3.986004418e14;
		aTwoBodyProp(dt, gm, pos, vel);

		Vector3d posExpect{
			5596646.34376213,
			-3201966.68063242,
			-1738526.05930174
		};
		Vector3d velExpect{
			4215.06430913537,
			5690.00005775394,
			3089.41796230054
		};

		for (int i = 0; i < 3; i++)
		{
			ASSERT_NEAR(pos[i], posExpect[i], 1e-5);
			ASSERT_NEAR(vel[i], velExpect[i], 1e-5);
		}
	}
}