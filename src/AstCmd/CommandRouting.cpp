///
/// @file      CommandRouting.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-11
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

#include "CommandRouting.hpp"
#include "AstUtil/IO.hpp"
#include <cassert>

AST_NAMESPACE_BEGIN


/// @brief 每次调用从 text 中提取并返回下一个 token，同时将 text 前移至剩余部分
/// @param text 输入文本视图，包含所有 token
/// @return 下一个 token 视图，data() 为空表示无更多 token
/// @note 该函数会修改 text，将其前移至剩余部分
StringView getNextToken(StringView& text) {
    const char* p = text.data();
    const char* const end = p + text.size();

    // 1. 跳过前导空白
    while (p < end && std::isspace(static_cast<unsigned char>(*p))) {
        ++p;
    }

    if (p == end) {                // 已无任何有效内容
        text = StringView{};
        return {};
    }

    // 2. 根据是否遇到双引号分别处理
    if (*p == '"') {
        ++p;                        // 越过左引号
        const char* const token_start = p;

        // 查找右引号（若没有则直接到达 end）
        while (p < end && *p != '"') {
            ++p;
        }

        // 提取 token（无论是否找到右引号）
        const std::size_t token_len = p - token_start;
        StringView token(token_start, token_len);

        // 越过右引号（如果存在）
        if (p < end) { ++p; }

        // 更新剩余视图
        text = StringView(p, end - p);
        return token;
    } else {
        // 普通 token：一直取到下一个空白或引号为止
        const char* const token_start = p;
        while (p < end && 
               !std::isspace(static_cast<unsigned char>(*p)) &&
               *p != '"') {
            ++p;
        }

        StringView token(token_start, p - token_start);
        text = StringView(p, end - p);
        return token;
    }
}

CommandTrie::Node& CommandTrie::addRule(StringView tmpl)
{
    const char* p = tmpl.data();
    const char* end = p + tmpl.size();

    Node* node = &root_;
    int lastNodeParams = node->numParams_;
    node->numParams_ = 0;

    while (p < end) {
        // 跳过前导空白
        while (p < end && std::isspace(static_cast<unsigned char>(*p))) {
            ++p;
        }
        if (p >= end) break;
        if(*p == '<')
        {
            while(p < end && *p != '>'){
                ++p;
            }
            node->numParams_++;
            ++p; // 跳过右尖括号
        }
        else{
            StringView token;
            if (*p == '"') {
                ++p; // 跳过左引号
                const char* start = p;
                while (p < end && *p != '"') {
                    ++p;
                }
                // 获取引号内的内容
                token = StringView(start, p - start);
                ++p; // 跳过右引号
            } 
            else 
            {
                const char* start = p;
                while (p < end && !std::isspace(static_cast<unsigned char>(*p)) && *p != '"') {
                    ++p;
                }
                // 获取文本节点
                token = StringView(start, p - start);
            }
            // 确保不同命令相同前缀部分的参数数量一致
            if(lastNodeParams != -1 && node->numParams_ != lastNodeParams)
            {
                assert(false && "param count is inconsistent");
                aError(_("参数数量不一致: %d vs %d"), node->numParams_, lastNodeParams);
            }

            node = &node->ensureChild(token);
            lastNodeParams = node->numParams_;
            node->numParams_ = 0;
        }
    }
    return *node;
}

void CommandTrie::addRule(StringView tmpl, std::shared_ptr<CommandHandler> handler)
{
    addRule(tmpl).handler_ = handler;
}

errc_t CommandTrie::find(StringView text, RoutingHandleResult& result) const
{
    const Node* node = &root_;
    while(1){
        StringView token = getNextToken(text);
        // data() 为空表示无更多 token
        if(token.data() == nullptr)
            break;
        node = node->findChild(token);
        if(!node)
        {
            // aError("failed to find match handle for input command: '%.*s'", text.size(), text.data());
            return eErrorInvalidParam;
        }
        for(int i=0;i<node->numParams_;i++)
        {
            token = getNextToken(text);
            if(token.data() == nullptr)
            {
                // 参数不足
                aError(_("参数不足"));
                return eErrorInvalidParam;
            }
            result.params().push_back(token);
        }
    };
    result.setHandler(node->handler_.get());
    return eNoError;
}


AST_NAMESPACE_END
