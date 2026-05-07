///
/// @file      testJson.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-16
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

#include "AstUtil/JsonValue.hpp"
#include "AstTest/Test.h"
#include "AstTest/TestJson.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/IO.hpp"
#include "AstScript/ValDict.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef AST_WITH_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif
#ifdef AST_WITH_JSONCPP
#include <json/json.h>
#endif

#ifdef AST_WITH_NLOHMANN_JSON

TEST(JsonTest, nlohmann_json)
{
    try{
        nlohmann::json value = 25;
        printf("value: %s\n", value.dump().c_str());
        double dval = value.get<double>();
        printf("dval: %f\n", dval);
        int ival = value.get<int>();
        printf("ival: %d\n", ival);
        bool bval = value.get<bool>();
        printf("bval: %d\n", bval);
        std::string sval = value;
        printf("sval: %s\n", sval.c_str());
    }catch(const nlohmann::json::exception& e){
        printf("error: %s\n", e.what());
    }catch(...){
        printf("error: unknown exception\n");
    }
    try{
        nlohmann::json value = "12.5";
        printf("value: %s\n", value.dump().c_str());
        double dval = value.get<double>();
        printf("dval: %f\n", dval);
        int ival = value.get<int>();
        printf("ival: %d\n", ival);
        bool bval = value.get<bool>();
        printf("bval: %d\n", bval);
        std::string sval = value;
        printf("sval: %s\n", sval.c_str());
    }catch(const nlohmann::json::exception& e){
        printf("error: %s\n", e.what());
    }catch(...){
        printf("error: unknown exception\n");
       }
}

#endif

#ifdef AST_WITH_JSONCPP

TEST(JsonTest, jsoncpp)
{
    {
        Json::Value value = 25;
        printf("value: %s\n", value.asString().c_str());
        double dval = value.asDouble();
        printf("dval: %f\n", dval);
        int ival = value.asInt();
        printf("ival: %d\n", ival);
        bool bval = value.asBool();
        printf("bval: %d\n", bval);
        std::string sval = value.asString();
        printf("sval: %s\n", sval.c_str());
    }
    try
    {
        Json::Value value = "12.5";
        printf("value: %s\n", value.asString().c_str());
        double dval = value.asDouble();
        printf("dval: %f\n", dval);
        int ival = value.asInt();
        printf("ival: %d\n", ival);
        bool bval = value.asBool();
        printf("bval: %d\n", bval);
        std::string sval = value.asString();
        printf("sval: %s\n", sval.c_str());
    }
    catch(const std::exception& e){
        printf("error: %s\n", e.what());
    }catch(...){
        printf("error: unknown exception\n");
    }
}

#endif

AST_USING_NAMESPACE;

TEST(JsonTest, JsonValue)
{
    JsonValue value;
    value.insert("a", 25);
    value.insert("b", "hello");
    value["c"]["d"] = true;
    printf("value: %s\n", value.toString().c_str());
    printf("json: %s\n", value.toJsonString(2).c_str());
}


TEST(JsonTest, VarDict)
{
    ValDict value;
    value.insert("a", 25);
    value.insert("b", "hello");
    value.insert("c", aNewValueDict());
    value["c"].insert("d", true);
    printf("value: %s\n", value.toString().c_str());
    printf("json: %s\n", value.toJsonString(2).c_str());
}


TEST(JsonTest, JsonParse)
{
    {
        JsonValue value;
        std::string json = R"({"a":25,"b":"hello","c":{"d":true}})";
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        errc_t ret = aTestJsonParse(json);
        EXPECT_EQ(ret, 0);
    }
}

TEST(JsonTest, JsonParseTypes)
{
    // 测试各种类型的JSON解析
    {
        JsonValue value;
        std::string json = R"({
            "integer": 42,
            "double": 3.14159,
            "boolean": true,
            "string": "test",
            "null": null
        })";
        printf("\nTesting JSON types parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        
        // 验证解析结果
        EXPECT_EQ(value["integer"].toInt(), 42);
        EXPECT_NEAR(value["double"].toDouble(), 3.14159, 1e-6);
        EXPECT_TRUE(value["boolean"].toBool());
        EXPECT_EQ(value["string"].toString(), "test");
        EXPECT_TRUE(value["null"].isNull());
        printf("Validation passed: all types are correctly parsed\n");
    }
}

TEST(JsonTest, JsonParseArray)
{
    // 测试JSON数组解析
    {
        JsonValue value;
        std::string json = R"({
            "array": [1, 2, 3, 4, 5],
            "nested_array": [[1, 2], [3, 4], [5, 6]]
        })";
        printf("\nTesting JSON array parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        
        // 验证数组解析结果
        const auto& array = value["array"].getArray();
        EXPECT_EQ(array.size(), 5);
        for (int i = 0; i < 5; i++) {
            EXPECT_EQ(array[i].toInt(), i + 1);
        }
        
        // 验证嵌套数组
        const auto& nestedArray = value["nested_array"].getArray();
        EXPECT_EQ(nestedArray.size(), 3);
        EXPECT_EQ(nestedArray[0].getArray()[0].toInt(), 1);
        EXPECT_EQ(nestedArray[0].getArray()[1].toInt(), 2);
        EXPECT_EQ(nestedArray[1].getArray()[0].toInt(), 3);
        EXPECT_EQ(nestedArray[1].getArray()[1].toInt(), 4);
        EXPECT_EQ(nestedArray[2].getArray()[0].toInt(), 5);
        EXPECT_EQ(nestedArray[2].getArray()[1].toInt(), 6);
        printf("Validation passed: arrays are correctly parsed\n");
    }
}

TEST(JsonTest, JsonParseNested)
{
    // 测试嵌套JSON对象解析
    {
        JsonValue value;
        std::string json = R"({
            "level1": {
                "level2": {
                    "level3": {
                        "value": "deep nested"
                    }
                }
            }
        })";
        printf("\nTesting nested JSON parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        
        // 验证嵌套对象解析结果
        EXPECT_EQ(value["level1"]["level2"]["level3"]["value"].toString(), "deep nested");
        printf("Validation passed: nested objects are correctly parsed\n");
    }
}

TEST(JsonTest, JsonParseError)
{
    // 测试错误的JSON解析
    {
        JsonValue value;
        std::string json = R"({"a": 25, "b": "hello",})";
        printf("\nTesting error JSON parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_NE(rc, 0);
        printf("parse error code: %d\n", rc);
    }
}

TEST(JsonTest, JsonParseEmpty)
{
    // 测试空JSON解析
    {
        JsonValue value;
        std::string json = "{}";
        printf("\nTesting empty JSON parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        
        // 验证空对象
        EXPECT_TRUE(value.isObject());
        const auto& object = value.getObject();
        EXPECT_EQ(object.size(), 0);
        printf("Validation passed: empty object is correctly parsed\n");
    }
}

TEST(JsonTest, JsonParseSingleValue)
{
    // 测试单一值的JSON解析
    {
        JsonValue value;
        std::string json = R"("just a string")";
        printf("\nTesting single value JSON parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        EXPECT_EQ(value.toString(), "just a string");
        printf("Validation passed: single string is correctly parsed\n");
    }
    // 测试空字符串解析
    {
        JsonValue value;
        std::string json = R"("")";
        printf("\nTesting empty string parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        EXPECT_EQ(value.toString(), "");
        printf("Validation passed: empty string is correctly parsed\n");
    }
    // 测试单个double解析
    {
        JsonValue value;
        std::string json = R"(-3.14159)";
        printf("\nTesting single double JSON parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        EXPECT_NEAR(value.toDouble(), -3.14159, 1e-6);
        printf("Validation passed: single double is correctly parsed\n");
    }
    // 测试单个int解析
    {
        JsonValue value;
        std::string json = R"(42)";
        printf("\nTesting single int JSON parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        EXPECT_EQ(value.toInt(), 42);
        printf("Validation passed: single int is correctly parsed\n");
    }
    // 测试单个bool解析
    {
        JsonValue value;
        std::string json = R"(true)";
        printf("\nTesting single bool JSON parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        EXPECT_TRUE(value.toBool());
        printf("Validation passed: single bool is correctly parsed\n");
    }
    // 测试单个null解析
    {
        JsonValue value;
        std::string json = R"(null)";
        printf("\nTesting single null JSON parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString(2).c_str());
        EXPECT_TRUE(value.isNull());
        printf("Validation passed: single null is correctly parsed\n");
    }
}

TEST(JsonTest, JsonParseBoundary)
{
    // 测试边界情况
    {
        // 测试大数字
        JsonValue value;
        std::string json = R"(9999999999999999999)";
        printf("\nTesting large number parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("Validation passed: large number is correctly parsed\n");
    }
    
    {
        // 测试小数字
        JsonValue value;
        std::string json = R"(0.0000000001)";
        printf("\nTesting small number parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("Validation passed: small number is correctly parsed\n");
    }
    
    {
        // 测试科学计数法
        JsonValue value;
        std::string json = R"(1.23e-4)";
        printf("\nTesting scientific notation parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("Validation passed: scientific notation is correctly parsed\n");
    }
    
    {
        // 测试长字符串
        JsonValue value;
        std::string longStr(100, 'a');
        std::string json = R"("" )";
        json.insert(1, longStr);
        printf("\nTesting long string parsing:\n");
        printf("input length: %zu characters\n", longStr.length());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed length: %zu characters\n", value.toString().length());
        printf("Validation passed: long string is correctly parsed\n");
    }
    
    {
        // 测试深度嵌套对象
        JsonValue value;
        std::string json = R"({"level1":{"level2":{"level3":{"level4":{"level5":{"value":42}}}}}})";
        printf("\nTesting deeply nested object parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("Validation passed: deeply nested object is correctly parsed\n");
    }
    
    {
        // 测试大型数组
        JsonValue value;
        std::string json = R"([1,2,3,4,5,6,7,8,9,10])";
        printf("\nTesting large array parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        const auto& array = value.getArray();
        EXPECT_EQ(array.size(), 10);
        printf("parsed array size: %zu\n", array.size());
        printf("Validation passed: large array is correctly parsed\n");
    }
}

TEST(JsonTest, JsonParseComplex)
{
    // 测试复杂情况
    {
        // 测试混合类型的数组
        JsonValue value;
        std::string json = R"([1, "string", true, null, 3.14])";
        printf("\nTesting mixed type array parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        const auto& array = value.getArray();
        EXPECT_EQ(array.size(), 5);
        EXPECT_EQ(array[0].toInt(), 1);
        EXPECT_EQ(array[1].toString(), "string");
        EXPECT_TRUE(array[2].toBool());
        EXPECT_TRUE(array[3].isNull());
        EXPECT_NEAR(array[4].toDouble(), 3.14, 1e-6);
        printf("parsed array size: %zu\n", array.size());
        printf("Validation passed: mixed type array is correctly parsed\n");
    }
    
    {
        // 测试包含特殊字符的字符串
        JsonValue value;
        std::string json = R"("special chars: \"\\\/\b\f\n\r\t")";
        printf("\nTesting special characters parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        printf("parsed: %s\n", value.toString().c_str());
        printf("parsed: %s\n", value.toJsonString().c_str());
        printf("Validation passed: special characters are correctly parsed\n");
    }
    
    {
        // 测试布尔值false
        JsonValue value;
        std::string json = R"(false)";
        printf("\nTesting false boolean parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        EXPECT_FALSE(value.toBool());
        printf("parsed: %s\n", value.toString().c_str());
        printf("Validation passed: false boolean is correctly parsed\n");
    }
    
    {
        // 测试空数组
        JsonValue value;
        std::string json = R"([])";
        printf("\nTesting empty array parsing:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        const auto& array = value.getArray();
        EXPECT_EQ(array.size(), 0);
        printf("parsed array size: %zu\n", array.size());
        printf("Validation passed: empty array is correctly parsed\n");
    }
}

TEST(JsonTest, JsonParseErrorCases)
{
    // 测试错误情况
    {
        // 测试缺少逗号
        JsonValue value;
        std::string json = R"({"a":1 "b":2})";
        printf("\nTesting missing comma error:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_NE(rc, 0);
        printf("parse error code: %d\n", rc);
    }
    
    {
        // 测试缺少引号
        JsonValue value;
        std::string json = R"({a:1, b:2})";
        printf("\nTesting missing quotes error:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_NE(rc, 0);
        printf("parse error code: %d\n", rc);
    }
    
    {
        // 测试多余的逗号
        JsonValue value;
        std::string json = R"({"a":1, "b":2,})";
        printf("\nTesting extra comma error:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_NE(rc, 0);
        printf("parse error code: %d\n", rc);
    }
    
    {
        // 测试多个根元素
        JsonValue value;
        std::string json = R"(1 2)";
        printf("\nTesting multiple root elements error:\n");
        printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_NE(rc, 0);
        printf("parse error code: %d\n", rc);
    }
}

TEST(JsonTest, JsonParseChineseAndUnicode)
{
    // 测试中文和Unicode字符
    {
        // 测试中文字符串
        JsonValue value;
        std::string json = R"({"中文": "你好世界", "english": "hello world"})";
        ast_printf("\nTesting Chinese characters parsing:\n");
        ast_printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        ast_printf("parsed: %s\n", value.toString().c_str());
        ast_printf("parsed: %s\n", value.toJsonString(2).c_str());
        EXPECT_EQ(value["中文"].toString(), "你好世界");
        EXPECT_EQ(value["english"].toString(), "hello world");
        ast_printf("Validation passed: Chinese characters are correctly parsed\n");
    }

    {
        #ifdef _WIN32
        // 设置控制台I/O编码为UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        #endif
        // 测试Unicode转义序列
        JsonValue value;
        std::string json = R"({"unicode": "\u4f60\u597d\u4e16\u754c", "emoji": "\ud83d\ude00"})";
        ast_printf("\nTesting Unicode escape sequences parsing:\n");
        ast_printf("input: %s\n", json.c_str());
        errc_t rc = value.parseFromString(json);
        EXPECT_EQ(rc, 0);
        ast_printf("parsed: %s\n", value.toString().c_str());
        ast_printf("parsed: %s\n", value.toJsonString(2).c_str());
        EXPECT_EQ(value["unicode"].toString(), u8"\u4f60\u597d\u4e16\u754c");
        EXPECT_EQ(value["emoji"].toString(), u8"\U0001F600");
        ast_printf("Validation passed: Unicode escape sequences are correctly parsed\n");
    }
}

GTEST_MAIN()