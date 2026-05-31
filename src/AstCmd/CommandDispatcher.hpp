///
/// @file      CommandManager.hpp
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

#pragma once

#include "AstGlobal.h"
#include "CommandUtil.hpp"
#include "CommandRouting.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/




class AST_CMD_API CommandDispatcher 
{
public:
    using CommandHandlerMap = std::unordered_map<std::string, std::shared_ptr<CommandHandler>>;
    explicit CommandDispatcher(bool whetherInit);
    CommandDispatcher() = default;
    ~CommandDispatcher() = default;
    errc_t execute(StringView command, CommandResult& result) const;
    void addRuleHandler(StringView tmpl, std::shared_ptr<CommandHandler> handler);
    CommandTrie::Node& addRule(StringView tmpl);
private:
    CommandTrie trie_;  ///< 命令路由树
};


#ifndef SWIG

template <typename F, typename Tuple>
class TaggedRule : public CommandHandler 
{
public:
    explicit TaggedRule(F f) 
        : handler_(std::move(f)) 
    {}

    errc_t handle(const CommandParams& params, CommandResult& result) const override {
        if (params.size() != std::tuple_size<Tuple>::value) {
            aError("invalid param count");
            return eErrorInvalidParam;
        }
        return call(params, result, Tuple{});
    }
private:
    // 展开 tuple<arg_pair<Pos,Type>...>
    template <typename... Args>
    errc_t call(const CommandParams& params, CommandResult& result, std::tuple<Args...>) const {
        result.clear();
        try {
            // 调用 handler，并通过 fill_result 自动分发返回值
            return detail::fill_result(result, 
                handler_(detail::convert_token<typename Args::type>(params[Args::pos])...));
        }
        catch(std::exception& e) {
            result.push_back(e.what());
            return eErrorInvalidParam;
        }
        catch (...) {
            return static_cast<errc_t>(-1);  // 异常统一返回 -1
        }
    }
private:
    F handler_;
};



// 新建命令处理器：使用 integral_constant 技巧绕过 decltype(lambda) 的未计算上下文限制
template <typename F, uint64_t Tag, int N>
std::shared_ptr<CommandHandler> makeCommandHandler(
    F handler,
    std::integral_constant<uint64_t, Tag>,
    std::integral_constant<int, N>
)
{
    using ArgTuple = detail::decode_tag<Tag, N>;
    return std::make_shared<TaggedRule<F, ArgTuple>>(std::move(handler));
}

template<uint64_t Tag, int N>
class CommandTrieNodeWrap
{
public:
    CommandTrieNodeWrap(CommandTrie::Node& node) : node_(node) {}

    template<typename F>
    void operator()(F f)
    {
        node_.handler_ = makeCommandHandler(std::move(f), std::integral_constant<uint64_t, Tag>{}, std::integral_constant<int, N>{});
    }
private:
    CommandTrie::Node& node_;
};

#endif

#define _AST_REGISTER_COMMAND(dispatcher, cmd_template, handler) \
    { \
        constexpr uint64_t tag = detail::get_parameter_tag(cmd_template);  \
        constexpr int n   = detail::count_args(cmd_template);         \
        (dispatcher).addRuleHandler(cmd_template, makeCommandHandler(handler, \
            std::integral_constant<uint64_t, tag>{}, \
            std::integral_constant<int, n>{})); \
    }


#define REGISTER_COMMAND(dispatcher, cmd_template, handler) \
    _AST_REGISTER_COMMAND(dispatcher, cmd_template, handler)


#define COMMAND_RULE(dispatcher, cmd_template) CommandTrieNodeWrap<detail::get_parameter_tag(cmd_template), detail::count_args(cmd_template)>(dispatcher.addRule(cmd_template))


/*! @} */

AST_NAMESPACE_END
