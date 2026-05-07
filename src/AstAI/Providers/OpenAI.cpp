///
/// @file      OpenAI.cpp
/// @brief     OpenAI规范API调用接口实现
/// @details   实现OpenAI规范的API调用接口，包括聊天、工具调用等。
/// @author    axel
/// @date      2026-04-23
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


#include "OpenAI.hpp"
#include "AstUtil/Network.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/Posix.hpp"
#include "AstUtil/Logger.hpp"
#include <sstream>
#include <iostream>

AST_NAMESPACE_BEGIN


OpenAI::OpenAI() 
{
    auto apiKey = posix::getenv("AST_AI_API_KEY");
    if(!apiKey || !*apiKey)
        aWarning("empty api key");
    else
        apiKey_ = apiKey;
        
    auto baseUrl = posix::getenv("AST_AI_BASE_URL");
    if(!baseUrl || !*baseUrl)
        aWarning("empty base url");
    else
        baseUrl_ = baseUrl;
}

OpenAI::OpenAI(const std::string& apiKey, const std::string& baseUrl) 
    : apiKey_(apiKey), baseUrl_(baseUrl) 
{
}


#if 0

std::string OpenAI::chat(const std::string& model, const std::vector<ChatMessage>& messages, 
                              const std::vector<AITool>& tools, float temperature) {
    std::string response;
    
    // 构建请求体
    JsonValue request_body;
    
    // 添加模型参数
    request_body["model"] = model;
    request_body["temperature"] = temperature;
    
    auto& messages_array = request_body["messages"];
    
    for (const auto& msg : messages) {
        JsonValue message_obj;
        
        // 角色转换
        std::string role_str;
        switch (msg.role()) {
            case EChatRole::eUser: role_str = "user";
                break;
            case EChatRole::eAssistant: role_str = "assistant";
                break;
            case EChatRole::eSystem: role_str = "system";
                break;
            case EChatRole::eTool: role_str = "tool";
                break;
        }
        
        message_obj["role"] = role_str;
        message_obj["content"] = msg.content();
        
        if (msg.role() == EChatRole::eTool && !msg.toolCallId().empty()) {
            message_obj["tool_call_id"] = msg.toolCallId();
        }
        
        messages_array.append(message_obj);
    }
    
    
    // 添加工具
    if (!tools.empty()) {
        JsonValue& tools_array = request_body["tools"];
        
        for (const auto& tool : tools) {
            JsonValue tool_obj;
            tool_obj["type"] = "function";
            
            JsonValue function_obj;
            function_obj["name"] = tool.name;
            function_obj["description"] = tool.description;
            
            JsonValue parameters_obj;
            parameters_obj["type"] = "object";
            
            JsonValue properties_obj;
            JsonValue required_array;
            
            for (const auto& param : tool.parameters) {
                JsonValue param_obj;
                param_obj["type"] = param.second.type;
                param_obj["description"] = param.second.description;
                
                if (param.second.required) {
                    param_obj["required"] = true;
                    required_array.append(param.first);
                }
                
                properties_obj[param.first] = param_obj;
            }
            
            parameters_obj["properties"] = properties_obj;
            parameters_obj["required"] = required_array;
            function_obj["parameters"] = parameters_obj;
            tool_obj["function"] = function_obj;
            tools_array.append(tool_obj);
        }
        
    }
    
    // 添加温度参数
    
    return network_response.body();
}
#endif


JsonValue OpenAI::chat(const JsonValue &request)
{
    JsonValue response;
    errc_t error = chat(request, response);
    if(error != 0)
    {
        aError("failed to call ai api for chat completions, error: %d", error);   
        return JsonValue();
    }
    return response;
}


errc_t OpenAI::chat(const JsonValue &request, JsonValue &response)
{
    // 确保 baseUrl 结尾没有斜杠
    std::string baseUrl = this->baseUrl();
    if (!baseUrl.empty() && baseUrl.back() == '/') baseUrl.pop_back();
    
    // 构建网络请求
    NetworkRequest networkRequest;
    networkRequest.setMethod(ENetworkRequestMethod::ePost);
    networkRequest.setUrl(baseUrl + "/chat/completions");
    networkRequest.setBody(request.toJsonString());
    
    // ast_printf("request: %s\n", request.toJsonString(2).c_str());

    // 设置请求头
    networkRequest.addHeader("Content-Type", "application/json");
    if(!apiKey().empty())
        networkRequest.addHeader("Authorization", "Bearer " + apiKey());
    
    // 发送请求
    NetworkResponse networkResponse;
    errc_t error = aNetworkRequest(networkRequest, networkResponse);
    if(!error)
    {
        error = networkResponse.toJson(response);
        if(error != 0)
        {
            aError("failed to parse response body, error: %d", error);   
        }
    }
    return error;
}

AST_NAMESPACE_END