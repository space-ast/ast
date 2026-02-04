///
/// @file      TypeTrait.hpp
/// @brief     类型特征
/// @details   
/// @author    axel
/// @date      2026-02-04
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#pragma once

#include "AstGlobal.h"
#include <type_traits>

AST_NAMESPACE_BEGIN


/// @brief 判断类型是否可调用
template<typename F, typename... Args>
struct is_callable {
private:
    template<typename U>
    static auto test(int) -> decltype(
        std::declval<U>()(std::declval<Args>()...), // 可调用
        std::true_type{}
    );
    
    template<typename>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<F>(0))::value;
};



AST_NAMESPACE_END
