///
/// @file      OperatorBinary.cpp
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

#include "OpBinPredefined.hpp"
#include "AstScript/AllHeaders.hpp"
#include "AstScript/Types.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"
#include "OpBinRegistry.hpp"

#include <tuple>
#include <unordered_map>
#include <functional>

AST_NAMESPACE_BEGIN

class Value;
class ValInt;
class ValDouble;


//  @brief 实现二元运算符的标量-标量版本
//  @param OPNAME 运算符名称
//  @param OP 运算符符号
#define _AST_OPBIN_SCALAR_SCALAR(OPNAME, OP) \
template<typename Left, typename Right> \
Value* opbin_##OPNAME##_scalar_scalar(Value* left, Value* right) \
{ \
    auto leftval  = static_cast<ValScalar<Left>*>(left); \
    auto rightval = static_cast<ValScalar<Right>*>(right); \
    return aNewValue(leftval->value() OP rightval->value()); \
}


//  @brief 实现二元运算符的标量-标量版本
_AST_OPBIN_SCALAR_SCALAR(add, +)
_AST_OPBIN_SCALAR_SCALAR(sub, -)
_AST_OPBIN_SCALAR_SCALAR(mul, *)
_AST_OPBIN_SCALAR_SCALAR(div, /)
// _AST_OPBIN_SCALAR_SCALAR(mod, %)
_AST_OPBIN_SCALAR_SCALAR(eq, ==)
_AST_OPBIN_SCALAR_SCALAR(ne, !=)
_AST_OPBIN_SCALAR_SCALAR(lt, <)
_AST_OPBIN_SCALAR_SCALAR(gt, >)
_AST_OPBIN_SCALAR_SCALAR(le, <=)
_AST_OPBIN_SCALAR_SCALAR(ge, >=)




//  @brief 执行加法运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_add(Value* left, Value* right)
{
    // @todo 实现加法运算
    return nullptr;
}


//  @brief 执行减法运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_sub(Value* left, Value* right)
{
    // @todo 实现减法运算
    return nullptr;
}


//  @brief 执行乘法运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_mul(Value* left, Value* right)
{
    // @todo 实现乘法运算
    return nullptr;
}

//  @brief 执行除法运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_div(Value* left, Value* right)
{
    // @todo 实现除法运算
    return nullptr;
}

//  @brief 执行取模运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_mod(Value* left, Value* right)
{
    // @todo 实现取模运算
    return nullptr;
}

//  @brief 执行乘方运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_pow(Value* left, Value* right)
{
    // @todo 实现乘方运算
    return nullptr;
}

//  @brief 执行按位与运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_and(Value* left, Value* right)
{
    // @todo 实现按位与运算
    return nullptr;
}


//  @brief 执行按位或运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_or(Value* left, Value* right)
{
    // @todo 实现按位或运算
    return nullptr;
}

//  @brief 执行按位异或运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_xor(Value* left, Value* right)
{
    // @todo 实现按位异或运算
    return nullptr;
}

//  @brief 执行判等运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_eq(Value* left, Value* right)
{
    // @todo 实现判等运算
    return nullptr;
}

//  @brief 执行判不等运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_ne(Value* left, Value* right)
{
    // @todo 实现判不等运算
    return nullptr;
}

//  @brief 执行大于运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_gt(Value* left, Value* right)
{
    // @todo 实现大于运算
    return nullptr;
}

//  @brief 执行大于等于运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_ge(Value* left, Value* right)
{
    // @todo 实现大于等于运算
    return nullptr;
}


//  @brief 执行小于运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_lt(Value* left, Value* right)
{
    // @todo 实现小于运算
    return nullptr;
}

//  @brief 执行小于等于运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_le(Value* left, Value* right)
{
    // @todo 实现小于等于运算
    return nullptr;
}



//  @brief 执行按元素乘运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_elem_mul(Value* left, Value* right)
{
    // @todo 实现按元素乘运算
    return nullptr;
}


//  @brief 执行按元素除运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_elem_div(Value* left, Value* right)
{
    // @todo 实现按元素除运算
    return nullptr;
}


//  @brief 执行按元素取模运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_elem_mod(Value* left, Value* right)
{
    // @todo 实现按元素取模运算
    return nullptr;
}

//  @brief 执行按元素乘方运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_elem_pow(Value* left, Value* right)
{
    // @todo 实现按元素乘方运算
    return nullptr;
}

//  @brief 执行按元素与运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_elem_and(Value* left, Value* right)
{
    // @todo 实现按元素与运算
    return nullptr;
}


//  @brief 执行按元素或运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_elem_or(Value* left, Value* right)
{
    // @todo 实现按元素或运算
    return nullptr;
}

//  @brief 执行按元素异或运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_elem_xor(Value* left, Value* right)
{
    // @todo 实现按元素异或运算
    return nullptr;
}

//  @brief 执行按元素相等运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* opbin_elem_eq(Value* left, Value* right)
{
    // @todo 实现按元素相等运算
    return nullptr;
}



//  @brief 获取全局运算函数注册表
//  @return 全局运算函数注册表引用
OpBinRegistry& get_opbin_registry() {
    static OpBinRegistry registry;
    return registry;
}

//  @brief 注册二进制运算函数
//  @param op 运算类型
//  @param leftType 左运算数类型
//  @param rightType 右运算数类型
//  @param func 运算函数指针
void opbin_register_func(OpBinType op, Class* leftType, Class* rightType, OpBinFunc func) {
    get_opbin_registry().regFunc(op, leftType, rightType, func);
}

//  @brief 获取二进制运算函数指针
//  @param op 运算类型
//  @param leftType 左运算数类型
//  @param rightType 右运算数类型
//  @return 运算函数指针
OpBinFunc opbin_get_func(OpBinType op, Class* leftType, Class* rightType) 
{
    return (OpBinFunc)get_opbin_registry().getFunc(op, leftType, rightType);
}

//  @brief 初始化二进制运算函数注册表
void opbin_init_registry() {
    auto& registry = get_opbin_registry();
    
    // 注册标量类型运算函数
    #define REGISTER_OPBIN(op, left_type, right_type, func) \
        opbin_register_func(OpBinType::op, left_type, right_type, (OpBinFunc)func)
    
    // 注册整数-整数运算
    REGISTER_OPBIN(eAdd, &aValInt_Type, &aValInt_Type, (opbin_add_scalar_scalar<int, int>));
    REGISTER_OPBIN(eSub, &aValInt_Type, &aValInt_Type, (opbin_sub_scalar_scalar<int, int>));
    REGISTER_OPBIN(eMul, &aValInt_Type, &aValInt_Type, (opbin_mul_scalar_scalar<int, int>));
    REGISTER_OPBIN(eDiv, &aValInt_Type, &aValInt_Type, (opbin_div_scalar_scalar<int, int>));
    //REGISTER_OPBIN(eMod, &aValInt_Type, &aValInt_Type, (opbin_mod_scalar_scalar<int, int>));
    REGISTER_OPBIN(eEq, &aValInt_Type, &aValInt_Type, (opbin_eq_scalar_scalar<int, int>));
    REGISTER_OPBIN(eNe, &aValInt_Type, &aValInt_Type, (opbin_ne_scalar_scalar<int, int>));
    REGISTER_OPBIN(eLt, &aValInt_Type, &aValInt_Type, (opbin_lt_scalar_scalar<int, int>));
    REGISTER_OPBIN(eGt, &aValInt_Type, &aValInt_Type, (opbin_gt_scalar_scalar<int, int>));
    REGISTER_OPBIN(eLe, &aValInt_Type, &aValInt_Type, (opbin_le_scalar_scalar<int, int>));
    REGISTER_OPBIN(eGe, &aValInt_Type, &aValInt_Type, (opbin_ge_scalar_scalar<int, int>));
    
    // 注册整数-双精度运算
    REGISTER_OPBIN(eAdd, &aValInt_Type, &aValDouble_Type, (opbin_add_scalar_scalar<int, double>));
    REGISTER_OPBIN(eSub, &aValInt_Type, &aValDouble_Type, (opbin_sub_scalar_scalar<int, double>));
    REGISTER_OPBIN(eMul, &aValInt_Type, &aValDouble_Type, (opbin_mul_scalar_scalar<int, double>));
    REGISTER_OPBIN(eDiv, &aValInt_Type, &aValDouble_Type, (opbin_div_scalar_scalar<int, double>));
    //REGISTER_OPBIN(eMod, &aValInt_Type, &aValDouble_Type, (opbin_mod_scalar_scalar<int, double>));
    REGISTER_OPBIN(eEq, &aValInt_Type, &aValDouble_Type, (opbin_eq_scalar_scalar<int, double>));
    REGISTER_OPBIN(eNe, &aValInt_Type, &aValDouble_Type, (opbin_ne_scalar_scalar<int, double>));
    REGISTER_OPBIN(eLt, &aValInt_Type, &aValDouble_Type, (opbin_lt_scalar_scalar<int, double>));
    REGISTER_OPBIN(eGt, &aValInt_Type, &aValDouble_Type, (opbin_gt_scalar_scalar<int, double>));
    REGISTER_OPBIN(eLe, &aValInt_Type, &aValDouble_Type, (opbin_le_scalar_scalar<int, double>));
    REGISTER_OPBIN(eGe, &aValInt_Type, &aValDouble_Type, (opbin_ge_scalar_scalar<int, double>));
    
    // 注册双精度-整数运算
    REGISTER_OPBIN(eAdd, &aValDouble_Type, &aValInt_Type, (opbin_add_scalar_scalar<double, int>));
    REGISTER_OPBIN(eSub, &aValDouble_Type, &aValInt_Type, (opbin_sub_scalar_scalar<double, int>));
    REGISTER_OPBIN(eMul, &aValDouble_Type, &aValInt_Type, (opbin_mul_scalar_scalar<double, int>));
    REGISTER_OPBIN(eDiv, &aValDouble_Type, &aValInt_Type, (opbin_div_scalar_scalar<double, int>));
    //REGISTER_OPBIN(eMod, &aValDouble_Type, &aValInt_Type, (opbin_mod_scalar_scalar<double, int>));
    REGISTER_OPBIN(eEq, &aValDouble_Type, &aValInt_Type, (opbin_eq_scalar_scalar<double, int>));
    REGISTER_OPBIN(eNe, &aValDouble_Type, &aValInt_Type, (opbin_ne_scalar_scalar<double, int>));
    REGISTER_OPBIN(eLt, &aValDouble_Type, &aValInt_Type, (opbin_lt_scalar_scalar<double, int>));
    REGISTER_OPBIN(eGt, &aValDouble_Type, &aValInt_Type, (opbin_gt_scalar_scalar<double, int>));
    REGISTER_OPBIN(eLe, &aValDouble_Type, &aValInt_Type, (opbin_le_scalar_scalar<double, int>));
    REGISTER_OPBIN(eGe, &aValDouble_Type, &aValInt_Type, (opbin_ge_scalar_scalar<double, int>));
    
    // 注册双精度-双精度运算
    REGISTER_OPBIN(eAdd, &aValDouble_Type, &aValDouble_Type, (opbin_add_scalar_scalar<double, double>));
    REGISTER_OPBIN(eSub, &aValDouble_Type, &aValDouble_Type, (opbin_sub_scalar_scalar<double, double>));
    REGISTER_OPBIN(eMul, &aValDouble_Type, &aValDouble_Type, (opbin_mul_scalar_scalar<double, double>));
    REGISTER_OPBIN(eDiv, &aValDouble_Type, &aValDouble_Type, (opbin_div_scalar_scalar<double, double>));
    //REGISTER_OPBIN(eMod, &aValDouble_Type, &aValDouble_Type, (opbin_mod_scalar_scalar<double, double>));
    REGISTER_OPBIN(eEq, &aValDouble_Type, &aValDouble_Type, (opbin_eq_scalar_scalar<double, double>));
    REGISTER_OPBIN(eNe, &aValDouble_Type, &aValDouble_Type, (opbin_ne_scalar_scalar<double, double>));
    REGISTER_OPBIN(eLt, &aValDouble_Type, &aValDouble_Type, (opbin_lt_scalar_scalar<double, double>));
    REGISTER_OPBIN(eGt, &aValDouble_Type, &aValDouble_Type, (opbin_gt_scalar_scalar<double, double>));
    REGISTER_OPBIN(eLe, &aValDouble_Type, &aValDouble_Type, (opbin_le_scalar_scalar<double, double>));
    REGISTER_OPBIN(eGe, &aValDouble_Type, &aValDouble_Type, (opbin_ge_scalar_scalar<double, double>));
    
    // 注册布尔-布尔运算
    REGISTER_OPBIN(eEq, &aValBool_Type, &aValBool_Type, (opbin_eq_scalar_scalar<bool, bool>));
    REGISTER_OPBIN(eNe, &aValBool_Type, &aValBool_Type, (opbin_ne_scalar_scalar<bool, bool>));
    
    #undef REGISTER_OPBIN
}

// 初始化注册表
auto __opbin_registry_initializer = []() -> bool {
    opbin_init_registry();
    return true;
}();


//  @brief 执行二进制运算
//  @param op 运算类型
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
Value* opbin(OpBinType op, Value* left, Value* right)
{
    // 首先检查输入是否有效
    if (!left || !right) {
        aError("Invalid input values for binary operation");
        return nullptr;
    }
    
    // 获取操作数类型
    Class* leftType = left->type();
    Class* rightType = right->type();
    
    // 获取对应的运算函数
    OpBinFunc func = opbin_get_func(op, leftType, rightType);
    if (func) {
        return func(left, right);
    }
    
    // 如果没有找到对应的运算函数，使用默认实现 
    switch (op)
    {
    case OpBinType::eAdd:
        return opbin_add(left, right);
    case OpBinType::eSub:
        return opbin_sub(left, right);
    case OpBinType::eMul:
        return opbin_mul(left, right);
    case OpBinType::eDiv:
        return opbin_div(left, right);
    case OpBinType::eMod:
        return opbin_mod(left, right);
    case OpBinType::ePow:
        return opbin_pow(left, right);
    case OpBinType::eEq:
        return opbin_eq(left, right);
    case OpBinType::eNe:
        return opbin_ne(left, right);
    case OpBinType::eGt:
        return opbin_gt(left, right);
    case OpBinType::eGe:
        return opbin_ge(left, right);
    case OpBinType::eLt:
        return opbin_lt(left, right);
    case OpBinType::eLe:
        return opbin_le(left, right);
    default:
        aError("Unsupported binary operator: %d", op);
        return nullptr;
    }
}



AST_NAMESPACE_END