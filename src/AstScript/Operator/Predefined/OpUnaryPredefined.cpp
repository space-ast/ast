///
/// @file      OpUnaryPredefined.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-22
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

#include "OpUnaryPredefined.hpp"
#include "OpUnaryRegister.hpp"
#include "AstScript/AllHeaders.hpp"
#include "AstScript/Types.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"

#include <tuple>
#include <unordered_map>
#include <functional>

AST_NAMESPACE_BEGIN

class Value;
class ValInt;
class ValDouble;
class ValBool;

//  @brief 实现一元运算符的标量版本
//  @param OPNAME 运算符名称
//  @param OP 运算符符号
#define _AST_OPUNARY_SCALAR(OPNAME, OP) \
template<typename T> \
Value* opunary_##OPNAME##_scalar(Value* value) \
{ \
    auto val = static_cast<ValScalar<T>*>(value); \
    return aNewValue(OP val->value()); \
}

//  @brief 实现一元运算符的标量版本
_AST_OPUNARY_SCALAR(not, !)
_AST_OPUNARY_SCALAR(neg, -)
_AST_OPUNARY_SCALAR(bit_not, ~)

#ifdef AST_ENABLE_REDUNDANT
//  @brief 执行逻辑非运算
//  @param value 运算数
//  @return 运算结果
static Value* opunary_not(Value* value)
{
    // @todo 实现逻辑非运算
    return nullptr;
}

//  @brief 执行负号运算
//  @param value 运算数
//  @return 运算结果
static Value* opunary_neg(Value* value)
{
    // @todo 实现负号运算
    return nullptr;
}

//  @brief 执行自增运算
//  @param value 运算数
//  @return 运算结果
static Value* opunary_inc(Value* value)
{
    // @todo 实现自增运算
    return nullptr;
}

//  @brief 执行自减运算
//  @param value 运算数
//  @return 运算结果
static Value* opunary_dec(Value* value)
{
    // @todo 实现自减运算
    return nullptr;
}

//  @brief 执行按位取反运算
//  @param value 运算数
//  @return 运算结果
static Value* opunary_bit_not(Value* value)
{
    // @todo 实现按位取反运算
    return nullptr;
}
#endif

//  @brief 获取全局运算函数注册表
//  @return 全局运算函数注册表引用
OpUnaryRegister& get_opunary_registry() {
    static OpUnaryRegister registry;
    return registry;
}

//  @brief 注册一元运算函数
//  @param op 运算类型
//  @param type 运算数类型
//  @param func 运算函数指针
void opunary_register_func(EOpUnaryType op, Class* type, OpUnaryFunc func) {
    static_assert(sizeof(OpUnaryFunc) == sizeof(void*), "OpUnaryFunc size must be equal to void* size");
    get_opunary_registry().regFunc(op, type, (void*)func);
}

//  @brief 获取一元运算函数指针
//  @param op 运算类型
//  @param type 运算数类型
//  @return 运算函数指针
OpUnaryFunc opunary_get_func(EOpUnaryType op, Class* type) 
{
    static_assert(sizeof(OpUnaryFunc) == sizeof(void*), "OpUnaryFunc size must be equal to void* size");
    return (OpUnaryFunc)get_opunary_registry().getFunc(op, type);
}


//  @brief 执行一元运算
//  @param op 运算类型
//  @param value 运算数
//  @return 运算结果
Value* opunary(EOpUnaryType op, Value* value)
{
    // 首先检查输入是否有效
    if (!value) {
        aError("Invalid input value for unary operation");
        return nullptr;
    }
    
    // 获取操作数类型
    Class* type = value->type();
    
    // 获取对应的运算函数
    OpUnaryFunc func = opunary_get_func(op, type);
    if (func) {
        return func(value);
    }
    
    return nullptr;
}


// 注册标量类型运算函数
#define REGISTER_OPUNARY(op, type, func) \
    opunary_register_func(EOpUnaryType::op, type, (OpUnaryFunc)func)

// 注册标量类型运算函数
void register_scalar_opunary()
{
    // 注册bool类型的一元运算符
    REGISTER_OPUNARY(eNot, &aValBool_Type, (opunary_not_scalar<bool>));
    REGISTER_OPUNARY(eBitNot, &aValBool_Type, (opunary_bit_not_scalar<bool>));
    
    // 注册int类型的一元运算符
    REGISTER_OPUNARY(eNeg, &aValInt_Type, (opunary_neg_scalar<int>));
    REGISTER_OPUNARY(eBitNot, &aValInt_Type, (opunary_bit_not_scalar<int>));
    
    // 注册double类型的一元运算符
    REGISTER_OPUNARY(eNeg, &aValDouble_Type, (opunary_neg_scalar<double>));
}

#undef REGISTER_OPUNARY


//  @brief 初始化一元运算函数注册表
void opunary_init_registry() {
    auto& registry = get_opunary_registry();
    
    // 注册标量类型运算函数
    register_scalar_opunary();
}

// 初始化注册表
auto __opunary_registry_initializer = []() -> bool {
    opunary_init_registry();
    return true;
}();


AST_NAMESPACE_END