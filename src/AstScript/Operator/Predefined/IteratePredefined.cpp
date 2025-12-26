///
/// @file      IteratePredefined.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-26
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

#include "IteratePredefined.hpp"
#include "AstScript/OpUnaryRegister.hpp"
#include "AstScript/OpUnaryPredefined.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/ValRange.hpp"
#include "AstScript/Types.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN


Value* iterate_range(Value* value, int& index)
{
    if(!value)
        return nullptr;
    ValRange* range = static_cast<ValRange*>(value);
    if(index == eIterBegin){
        index = 1;
        return aNewValueDouble(range->start());
    }else{
        if(index >= range->size()){
            return nullptr;
        }
        double next = range->start() + index * range->step();
        index++;
        return aNewValueDouble(next);
    }
}


IterateFunc iterate_get_func(Class *type)
{
    static_assert(sizeof(IterateFunc) == sizeof(void*), "IterateFunc size must be equal to void* size");
    return (IterateFunc)(opunary_get_funcptr(EOpUnaryType::eIterate, type));
}

void iterate_register_func(Class *type, IterateFunc func)
{
    opunary_register_funcptr(EOpUnaryType::eIterate, type, (void*)func);
}


A_THREAD_LOCAL IterateFunc tFunc = nullptr;
A_THREAD_LOCAL Class* tType = nullptr;

Value *iterate_begin(Value* container, int &index)
{
    index = eIterBegin;
    return iterate_next(container, index);
}

Value *iterate_next(Value* container, int &index)
{
    if(A_UNLIKELY(!container)){
        aError("container is null");
        return nullptr;
    }
    IterateFunc cacheFunc = tFunc;
    Class* cacheType = tType;
    if(A_UNLIKELY(container->type() != cacheType)){
        cacheFunc = iterate_get_func(container->type());
        tFunc = cacheFunc;
        tType = container->type();
    }
    if(A_UNLIKELY(!cacheFunc)){
        aError("iterate func is null");
        return nullptr;
    }
    return cacheFunc(container, index);;
}

void iterate_init_registry()
{
    iterate_register_func(&aValRange_Type, iterate_range);
}

auto __iterate_registry_initializer = []() -> bool {
    iterate_init_registry();
    return true;
}();

AST_NAMESPACE_END
