///
/// @file      testDeepSeek.cpp
/// @brief     测试调用DeepSeek API
/// @details   
/// @author    axel
/// @date      2026-04-24
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

#include "AstUtil/Network.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Posix.hpp"
#include "AstUtil/Json.hpp"
#include "AstAI/DeepSeek.hpp"
#include "AstAI/ChatSession.hpp"
#include "AstSim/Satellite.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(DeepSeekTest, Client)
{
    GTEST_SKIP();
    DeepSeek client;
    auto req = u8R"(
    {
        "model": "deepseek-v4-flash",
        "messages": [
            {
                "role": "system", 
                "content": "你是你个专业工程师，擅长航天任务设计与分析，能够熟练使用航天任务设计软件，你需要协助用户使用航天进行任务设计与分析。\n更具体一点的描述：\n- 正在使用的软件是ATK，类似于STK软件;\n- 你正在协助用户建立一个航天任务分析场景\n- 场景中的对象包括：卫星、地面站、传感器等"
            },
            {"role": "user", "content": "帮我设计一个太阳同步轨道"}
        ],
        "temperature": 0.2,
        "tools":[
            {
                "type": "function",
                "function": {
                    "name": "find_classes",
                    "description": "查找软件支持的所有对象类型"
                }
            },
            {
                "type": "function",
                "function": {
                    "name": "create_object",
                    "description": "创建一个新的对象",
                    "parameters": {
                        "type": "object",
                        "properties": {
                            "class": {
                                "type": "string",
                                "description": "对象的类型，必须是通过find_classes返回的类型"
                            }
                        }
                    }
                }
            },
            {
                "type": "function",
                "function": {
                    "name": "find_objects",
                    "description": "查找当前设计场景中的对象",
                    "parameters": {
                        "type": "object",
                        "properties": {
                            "class": {
                                "type": "string",
                                "description": "对象的类型，必须是通过find_classes返回的类型，如果没有指定类型，默认查找所有对象"
                            }
                        }
                    }
                }
            }
        ],
        "thinking": {"type": "disabled"},
        "stream": false
    }
)"_json;
    auto res = client.chat(req);
    ast_printf("res: \n%s\n", res.toJsonString().c_str());
}

TEST(DeepSeekTest, API)
{
    GTEST_SKIP();
    NetworkRequest req;
    NetworkResponse res;
    const char* url = posix::getenv("AST_AI_BASE_URL");
    const char* key = posix::getenv("AST_AI_API_KEY");
    if(!url || !key || !*url || !*key)
        GTEST_SKIP();
    req.setUrl(std::string(url) + "/chat/completions");
    req.setMethod("POST");
    req.addHeader("Content-Type", "application/json");
    req.addHeader("Authorization", std::string("Bearer ") + key);
    std::string body = R"(
    {
        "model": "deepseek-v4-flash",
        "messages": [
          {"role": "system", "content": "You are a helpful assistant."},
          {"role": "user", "content": "我是CICD, 你返回'成功'就行"}
        ],
        "thinking": {"type": "disabled"},
        "stream": false
    }
)";
    req.setBody(body);

    {
        aNetworkSetImpl(ENetworkImplType::eCurlCmd);
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(rc, 0);
        ast_printf("res: %s\n", res.body().c_str());
    }
    {
        aNetworkSetImpl(ENetworkImplType::eWinHTTP);
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(rc, 0);
        ast_printf("res: %s\n", res.body().c_str());
    }
    {
        aNetworkSetImpl(ENetworkImplType::eWinINet);
        errc_t rc = aNetworkRequest(req, res);
        EXPECT_EQ(rc, 0);
        ast_printf("res: %s\n", res.body().c_str());
    }    
    
}


GTEST_MAIN()

