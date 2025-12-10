///
/// @file      testReflect.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-10
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
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstUtil/ReflectAPI.hpp"
#include "AstUtil/Property.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

class TestClass{
public:
    double propDouble_;
    double propDouble() const{return propDouble_;}
    err_t setPropDouble(double d){ propDouble_ = d; return 0;}
    void setPropDouble2(double d){ propDouble_ = d;}

};




TEST(Reflect, PropertyDouble)
{
    constexpr auto memberPtr = &TestClass::propDouble_;
    auto prop = aNewPropertyDouble<TestClass, memberPtr>();
    TestClass test{};
    test.propDouble_ = 1.0204;
    double value;
    err_t err;
    err = prop->getValueDouble(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, 1.0204);
    err = prop->setValueDouble(&test, 2.0408);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propDouble_, 2.0408);
}



TEST(Reflect, PropertyDoubleGetterSetter)
{
    auto prop = aNewPropertyDouble<TestClass, &TestClass::propDouble, &TestClass::setPropDouble>();
    TestClass test{};
    test.propDouble_ = 1.0204;
    double value;
    err_t err;
    err = prop->getValueDouble(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, 1.0204);
    err = prop->setValueDouble(&test, 2.0408);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propDouble_, 2.0408);
}


TEST(Reflect, PropertyDoubleGetterSetter2)
{
    auto prop = aNewPropertyDouble<TestClass, &TestClass::propDouble, &TestClass::setPropDouble2>();
    TestClass test{};
    test.propDouble_ = 1.0204;
    double value;
    err_t err;
    err = prop->getValueDouble(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, 1.0204);
    err = prop->setValueDouble(&test, 2.0408);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propDouble_, 2.0408);
}


GTEST_MAIN()

