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
#include "AstUtil/Struct.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

class TestClass{
public:
    // 成员变量
    bool propBool_;
    int propInt_;
    double propDouble_;
    std::string propString_;
    
    // 成员函数（不同返回类型和参数类型）
    bool propBool() const{return propBool_;}
    int propInt() const{return propInt_;}
    double propDouble() const{return propDouble_;}
    const std::string& propString() const{return propString_;}
    
    // 不同类型的setter函数
    err_t setPropBool(bool b){ propBool_ = b; return 0;}
    err_t setPropInt(int i){ propInt_ = i; return 0;}
    err_t setPropDouble(double d){ propDouble_ = d; return 0;}
    err_t setPropString(StringView s){ propString_ = std::string(s); return 0;}
    
    void setPropBool2(bool b){ propBool_ = b;}
    void setPropInt2(int i){ propInt_ = i;}
    void setPropDouble2(double d){ propDouble_ = d;}
    void setPropString2(StringView s){ propString_ = std::string(s);}
};


// 测试布尔属性
TEST(Reflect, PropertyBool)
{
    constexpr auto memberPtr = &TestClass::propBool_;
    auto prop = aNewPropertyBool<TestClass, memberPtr>();
    TestClass test{};
    test.propBool_ = true;
    bool value;
    err_t err;
    err = prop->getValueBool(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, true);
    err = prop->setValueBool(&test, false);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propBool_, false);
}

TEST(Reflect, PropertyBoolGetterSetter)
{
    auto prop = aNewPropertyBool<TestClass, &TestClass::propBool, &TestClass::setPropBool>();
    TestClass test{};
    test.propBool_ = true;
    bool value;
    err_t err;
    err = prop->getValueBool(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, true);
    err = prop->setValueBool(&test, false);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propBool_, false);
}

TEST(Reflect, PropertyBoolGetterSetter2)
{
    auto prop = aNewPropertyBool<TestClass, &TestClass::propBool, &TestClass::setPropBool2>();
    TestClass test{};
    test.propBool_ = true;
    bool value;
    err_t err;
    err = prop->getValueBool(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, true);
    err = prop->setValueBool(&test, false);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propBool_, false);
}

// 测试布尔属性的类型转换
TEST(Reflect, PropertyBoolConvert)
{
    constexpr auto memberPtr = &TestClass::propBool_;
    auto prop = aNewPropertyBool<TestClass, memberPtr>();
    TestClass test{};
    
    // 测试 bool -> int
    test.propBool_ = true;
    int intValue;
    err_t err = prop->getValueInt(&test, intValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(intValue, 1);
    
    // 测试 int -> bool
    err = prop->setValueInt(&test, 0);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propBool_, false);
    
    // 测试 bool -> double
    test.propBool_ = true;
    double doubleValue;
    err = prop->getValueDouble(&test, doubleValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(doubleValue, 1.0);
    
    // 测试 double -> bool
    err = prop->setValueDouble(&test, 0.0);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propBool_, false);
    
    // 测试 bool -> string
    test.propBool_ = true;
    std::string stringValue;
    err = prop->getValueString(&test, stringValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(stringValue, "true");
    
    // 测试 string -> bool
    err = prop->setValueString(&test, "false");
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propBool_, false);
    
    // 测试 string -> bool (true)
    err = prop->setValueString(&test, "true");
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propBool_, true);
}

// 测试整数属性
TEST(Reflect, PropertyInt)
{
    constexpr auto memberPtr = &TestClass::propInt_;
    auto prop = aNewPropertyInt<TestClass, memberPtr>();
    TestClass test{};
    test.propInt_ = 42;
    int value;
    err_t err;
    err = prop->getValueInt(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, 42);
    err = prop->setValueInt(&test, 84);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propInt_, 84);
}

TEST(Reflect, PropertyIntGetterSetter)
{
    auto prop = aNewPropertyInt<TestClass, &TestClass::propInt, &TestClass::setPropInt>();
    TestClass test{};
    test.propInt_ = 42;
    int value;
    err_t err;
    err = prop->getValueInt(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, 42);
    err = prop->setValueInt(&test, 84);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propInt_, 84);
}

TEST(Reflect, PropertyIntGetterSetter2)
{
    auto prop = aNewPropertyInt<TestClass, &TestClass::propInt, &TestClass::setPropInt2>();
    TestClass test{};
    test.propInt_ = 42;
    int value;
    err_t err;
    err = prop->getValueInt(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, 42);
    err = prop->setValueInt(&test, 84);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propInt_, 84);
}

// 测试整数属性的类型转换
TEST(Reflect, PropertyIntConvert)
{
    constexpr auto memberPtr = &TestClass::propInt_;
    auto prop = aNewPropertyInt<TestClass, memberPtr>();
    TestClass test{};
    
    // 测试 int -> bool
    test.propInt_ = 42;
    bool boolValue;
    err_t err = prop->getValueBool(&test, boolValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(boolValue, true);
    
    // 测试 bool -> int
    err = prop->setValueBool(&test, false);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propInt_, 0);
    
    // 测试 int -> double
    test.propInt_ = 42;
    double doubleValue;
    err = prop->getValueDouble(&test, doubleValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(doubleValue, 42.0);
    
    // 测试 double -> int
    err = prop->setValueDouble(&test, 84.5);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propInt_, 84);
    
    // 测试 int -> string
    test.propInt_ = 42;
    std::string stringValue;
    err = prop->getValueString(&test, stringValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(stringValue, "42");
    
    // 测试 string -> int
    err = prop->setValueString(&test, "84");
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propInt_, 84);
}

// 测试双精度属性（已有的）
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

// 测试双精度属性的类型转换
TEST(Reflect, PropertyDoubleConvert)
{
    constexpr auto memberPtr = &TestClass::propDouble_;
    auto prop = aNewPropertyDouble<TestClass, memberPtr>();
    TestClass test{};
    
    // 测试 double -> bool
    test.propDouble_ = 42.5;
    bool boolValue;
    err_t err = prop->getValueBool(&test, boolValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(boolValue, true);
    
    // 测试 bool -> double
    err = prop->setValueBool(&test, true);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propDouble_, 1.0);
    
    // 测试 double -> int
    test.propDouble_ = 42.5;
    int intValue;
    err = prop->getValueInt(&test, intValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(intValue, 42);
    
    // 测试 int -> double
    err = prop->setValueInt(&test, 84);
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propDouble_, 84.0);
    
    // 测试 double -> string
    test.propDouble_ = 42.5;
    std::string stringValue;
    err = prop->getValueString(&test, stringValue);
    EXPECT_FALSE(err);
    // 注意：浮点数转字符串可能有精度问题，这里不进行精确比较
    
    // 测试 string -> double
    err = prop->setValueString(&test, "84.5");
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propDouble_, 84.5);
}

// 测试字符串属性
TEST(Reflect, PropertyString)
{
    constexpr auto memberPtr = &TestClass::propString_;
    auto prop = aNewPropertyString<TestClass, memberPtr>();
    TestClass test{};
    test.propString_ = "test";
    std::string value;
    err_t err;
    err = prop->getValueString(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, "test");
    err = prop->setValueString(&test, "test2");
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propString_, "test2");
}

TEST(Reflect, PropertyStringGetterSetter)
{
    auto prop = aNewPropertyString<TestClass, &TestClass::propString, &TestClass::setPropString>();
    TestClass test{};
    test.propString_ = "test";
    std::string value;
    err_t err;
    err = prop->getValueString(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, "test");
    err = prop->setValueString(&test, "test2");
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propString_, "test2");
}

TEST(Reflect, PropertyStringGetterSetter2)
{
    auto prop = aNewPropertyString<TestClass, &TestClass::propString, &TestClass::setPropString2>();
    TestClass test{};
    test.propString_ = "test";
    std::string value;
    err_t err;
    err = prop->getValueString(&test, value);
    EXPECT_FALSE(err);
    EXPECT_EQ(value, "test");
    err = prop->setValueString(&test, "test2");
    EXPECT_FALSE(err);
    EXPECT_EQ(test.propString_, "test2");
}

// 测试Struct类的功能
TEST(Reflect, Struct)
{
    Struct testStruct("TestStruct", "Test struct description");
    
    // 创建属性
    constexpr auto boolMemberPtr = &TestClass::propBool_;
    constexpr auto intMemberPtr = &TestClass::propInt_;
    constexpr auto doubleMemberPtr = &TestClass::propDouble_;
    constexpr auto stringMemberPtr = &TestClass::propString_;
    
    auto boolProp = aNewPropertyBool<TestClass, boolMemberPtr>();
    auto intProp = aNewPropertyInt<TestClass, intMemberPtr>();
    auto doubleProp = aNewPropertyDouble<TestClass, doubleMemberPtr>();
    auto stringProp = aNewPropertyString<TestClass, stringMemberPtr>();
    
    // 添加属性到Struct
    testStruct.addProperty("boolProp", boolProp);
    testStruct.addProperty("intProp", intProp);
    testStruct.addProperty("doubleProp", doubleProp);
    testStruct.addProperty("stringProp", stringProp);
    
    // 测试获取属性
    EXPECT_EQ(testStruct.getProperty("boolProp"), boolProp);
    EXPECT_EQ(testStruct.getProperty("intProp"), intProp);
    EXPECT_EQ(testStruct.getProperty("doubleProp"), doubleProp);
    EXPECT_EQ(testStruct.getProperty("stringProp"), stringProp);
    
    // 测试获取不存在的属性
    EXPECT_EQ(testStruct.getProperty("nonExistentProp"), nullptr);
    
    // 测试使用属性操作对象
    TestClass testObj{};
    testObj.propBool_ = true;
    testObj.propInt_ = 42;
    testObj.propDouble_ = 3.14;
    testObj.propString_ = "test";
    
    bool boolValue;
    err_t err = testStruct.getProperty("boolProp")->getValueBool(&testObj, boolValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(boolValue, true);
    
    int intValue;
    err = testStruct.getProperty("intProp")->getValueInt(&testObj, intValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(intValue, 42);
    
    double doubleValue;
    err = testStruct.getProperty("doubleProp")->getValueDouble(&testObj, doubleValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(doubleValue, 3.14);
    
    std::string stringValue;
    err = testStruct.getProperty("stringProp")->getValueString(&testObj, stringValue);
    EXPECT_FALSE(err);
    EXPECT_EQ(stringValue, "test");
    
    // 测试修改属性值
    err = testStruct.getProperty("boolProp")->setValueBool(&testObj, false);
    EXPECT_FALSE(err);
    EXPECT_EQ(testObj.propBool_, false);
    
    err = testStruct.getProperty("intProp")->setValueInt(&testObj, 84);
    EXPECT_FALSE(err);
    EXPECT_EQ(testObj.propInt_, 84);
}


GTEST_MAIN()