///
/// @file      testDataGroupMagField.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-08-11
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/Test.hpp"
#include "ast/DataGroupMagField.hpp"
#include "ast/EphemerisTwoBody.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/Literals.hpp"

AST_USING_NAMESPACE

TEST(TestDataGroupMagField, TestDataGroupMagField)
{
    auto earth = aGetEarth();
    auto eci = earth->getFrameInertial();
    double gm = earth->getGM();
    ModOrbElem elem{6678137, 0, 28.5_deg, 0, 0, 0};
    CartState initstate;
    aModOrbElemToCart(elem, gm, initstate.pos(), initstate.vel());
    auto epoch = "13 Aug 2026 04:00:00.000 UTCG"_utc;
    SharedPtr<EphemerisTwoBody> ephemeris = EphemerisTwoBody::New(eci, epoch, initstate);
    ScopedPtr<DataGroupMagField> magField = new DataGroupMagField();
    magField->setPoint(ephemeris);

    VariantVector result;
    TimeList timeList{epoch};
    timeList.seconds() = {0};
    errc_t rc = magField->calculate(timeList, result);
    EXPECT_EQ(rc, eNoError);

    auto data = result.asSpan<DataGroupMagField::Data>();
    EXPECT_EQ(data.size(), 1);

}




GTEST_MAIN()

