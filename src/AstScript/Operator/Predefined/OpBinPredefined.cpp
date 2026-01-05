///
/// @file      OperatorBinary.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
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
#include <cmath>

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
// _AST_OPBIN_SCALAR_SCALAR(div, /)  // 除法需要特殊处理
_AST_OPBIN_SCALAR_SCALAR(eq, ==)
_AST_OPBIN_SCALAR_SCALAR(ne, !=)
_AST_OPBIN_SCALAR_SCALAR(lt, <)
_AST_OPBIN_SCALAR_SCALAR(gt, >)
_AST_OPBIN_SCALAR_SCALAR(le, <=)
_AST_OPBIN_SCALAR_SCALAR(ge, >=)
_AST_OPBIN_SCALAR_SCALAR(and, &&)
_AST_OPBIN_SCALAR_SCALAR(or, ||)
// 位运算
_AST_OPBIN_SCALAR_SCALAR(bit_and, &)
_AST_OPBIN_SCALAR_SCALAR(bit_or, |)
_AST_OPBIN_SCALAR_SCALAR(bit_xor, ^)
_AST_OPBIN_SCALAR_SCALAR(bit_left_shift, <<)
_AST_OPBIN_SCALAR_SCALAR(bit_right_shift, >>)

//  @brief 执行除法运算
template<typename Left, typename Right> 
Value* opbin_div_scalar_scalar(Value* left, Value* right) 
{
    // 与julia语言一致：除法只允许浮点数类型，防止出现 1/2 得到 0
    auto leftval = static_cast<ValScalar<Left>*>(left); 
    auto rightval = static_cast<ValScalar<Right>*>(right); 
    double leftval_common = (double)leftval->value();
    double rightval_common = (double)rightval->value();
    return aNewValue(leftval_common / rightval_common); 
}

inline double _mod(double left, double right)
{
    return fmod(left, right);
}

inline int _mod(int left, int right)
{
    return (int)fmod(left, right);
}

//  @brief 执行取模运算
template<typename Left, typename Right> 
Value* opbin_mod_scalar_scalar(Value* left, Value* right) 
{ 
    auto leftval = static_cast<ValScalar<Left>*>(left); 
    auto rightval = static_cast<ValScalar<Right>*>(right); 
    using CommonType = typename std::common_type<Left, Right>::type;
    CommonType leftval_common = (CommonType)leftval->value();
    CommonType rightval_common = (CommonType)rightval->value();
    return aNewValue(_mod(leftval_common, rightval_common)); 
}

inline int _pow(int left, int right)
{
    return (int) std::pow(left, right);
}

inline double _pow(double left, double right)
{
    return std::pow(left, right);
}

//  @brief 执行乘方运算
template<typename Left, typename Right>
Value* opbin_pow_scalar_scalar(Value* left, Value* right) 
{ 
    auto leftval = static_cast<ValScalar<Left>*>(left); 
    auto rightval = static_cast<ValScalar<Right>*>(right); 
    using CommonType = typename std::common_type<Left, Right>::type;
    CommonType leftval_common = (CommonType)leftval->value();
    CommonType rightval_common = (CommonType)rightval->value();
    return aNewValue(_pow(leftval_common, rightval_common)); 
}


Value* opbin_mul_quantity_quantity(Value* left, Value* right)
{
    auto leftval = static_cast<ValQuantity*>(left); 
    auto rightval = static_cast<ValQuantity*>(right); 
    return aNewValue(leftval->quantity() * rightval->quantity()); 
}


Value* opbin_div_quantity_quantity(Value* left, Value* right)
{
    auto leftval = static_cast<ValQuantity*>(left); 
    auto rightval = static_cast<ValQuantity*>(right); 
    return aNewValue(leftval->quantity() / rightval->quantity()); 
}

template<typename Right>
Value* opbin_mul_quantity_scalar(Value* left, Value* right)
{
    auto leftval = static_cast<ValQuantity*>(left); 
    auto rightval = static_cast<ValScalar<Right>*>(right); 
    return aNewValue(leftval->quantity() * rightval->value()); 
}

template<typename Right>
Value* opbin_div_quantity_scalar(Value* left, Value* right)
{
    auto leftval = static_cast<ValQuantity*>(left); 
    auto rightval = static_cast<ValScalar<Right>*>(right); 
    return aNewValue(leftval->quantity() / rightval->value()); 
}

template<typename Left>
Value* opbin_mul_scalar_quantity(Value* left, Value* right)
{
    auto leftval = static_cast<ValScalar<Left>*>(left); 
    auto rightval = static_cast<ValQuantity*>(right); 
    return aNewValue(leftval->value() * rightval->quantity()); 
}

template<typename Left>
Value* opbin_div_scalar_quantity(Value* left, Value* right)
{
    auto leftval = static_cast<ValScalar<Left>*>(left); 
    auto rightval = static_cast<ValQuantity*>(right); 
    return aNewValue(leftval->value() / rightval->quantity()); 
}


Value* opbin_add_quantity_quantity(Value* left, Value* right)
{
    auto leftval = static_cast<ValQuantity*>(left); 
    auto rightval = static_cast<ValQuantity*>(right); 
    return aNewValue(leftval->quantity() + rightval->quantity()); 
}


Value* opbin_sub_quantity_quantity(Value* left, Value* right)
{
    auto leftval = static_cast<ValQuantity*>(left); 
    auto rightval = static_cast<ValQuantity*>(right); 
    return aNewValue(leftval->quantity() - rightval->quantity()); 
}



#ifdef AST_ENABLE_REDUNDANT

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

#endif

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
void opbin_register_func(EOpBinType op, Class* leftType, Class* rightType, OpBinFunc func) {
    static_assert(sizeof(OpBinFunc) == sizeof(void*), "OpBinFunc size must be equal to void* size");
    get_opbin_registry().regFunc(op, leftType, rightType, (void*)func);
}

//  @brief 获取二进制运算函数指针
//  @param op 运算类型
//  @param leftType 左运算数类型
//  @param rightType 右运算数类型
//  @return 运算函数指针
OpBinFunc opbin_get_func(EOpBinType op, Class* leftType, Class* rightType) 
{
    static_assert(sizeof(OpBinFunc) == sizeof(void*), "OpBinFunc size must be equal to void* size");
    return (OpBinFunc)get_opbin_registry().getFunc(op, leftType, rightType);
}


template<typename T>
Class* aValueType();

template<>
Class* aValueType<int>() { return &aValInt_Type; }

template<>
Class* aValueType<double>() { return &aValDouble_Type; }

template<>
Class* aValueType<bool>() { return &aValBool_Type; }

// 注册标量类型运算函数
#define REGISTER_OPBIN(op, left_type, right_type, func) \
    opbin_register_func(EOpBinType::op, left_type, right_type, (OpBinFunc)func)

template<typename SCALAR1, typename SCALAR2>
void _register_scalar_opbin()
{
    REGISTER_OPBIN(eAdd, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_add_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eSub, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_sub_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eMul, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_mul_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eDiv, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_div_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eEq, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_eq_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eNe, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_ne_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eLt, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_lt_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eGt, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_gt_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eLe, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_le_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eGe, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_ge_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eAnd, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_and_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eOr, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_or_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(eMod, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_mod_scalar_scalar<SCALAR1, SCALAR2>));
    REGISTER_OPBIN(ePow, aValueType<SCALAR1>(), aValueType<SCALAR2>(), (opbin_pow_scalar_scalar<SCALAR1, SCALAR2>));
    // 位运算只在int类型中单独注册，避免bool和double类型的错误
}

//#undef REGISTER_OPBIN


template<typename SCALAR1, typename SCALAR2>
void register_scalar_opbin()
{
    _register_scalar_opbin<SCALAR1, SCALAR2>();
    if(!std::is_same<SCALAR1, SCALAR2>::value)
    {
        _register_scalar_opbin<SCALAR2, SCALAR1>();
    }
}

template<typename SCALAR>
void register_scalar_opbin()
{
    register_scalar_opbin<SCALAR, SCALAR>();
}


//  @brief 初始化二进制运算函数注册表
void opbin_init_registry() {
    auto& registry = get_opbin_registry();
    
    // 注册标量类型运算函数
    register_scalar_opbin<bool>();
    register_scalar_opbin<int>();
    register_scalar_opbin<double>();
    
    register_scalar_opbin<bool, int>();
    register_scalar_opbin<bool, double>();
    register_scalar_opbin<int, double>();
    
    // 单独为int类型注册位运算（只支持int类型的位运算）
    opbin_register_func(EOpBinType::eBitAnd, &aValInt_Type, &aValInt_Type, (OpBinFunc)opbin_bit_and_scalar_scalar<int, int>);
    opbin_register_func(EOpBinType::eBitOr, &aValInt_Type, &aValInt_Type, (OpBinFunc)opbin_bit_or_scalar_scalar<int, int>);
    opbin_register_func(EOpBinType::eBitXor, &aValInt_Type, &aValInt_Type, (OpBinFunc)opbin_bit_xor_scalar_scalar<int, int>);
    opbin_register_func(EOpBinType::eBitLeftShift, &aValInt_Type, &aValInt_Type, (OpBinFunc)opbin_bit_left_shift_scalar_scalar<int, int>);
    opbin_register_func(EOpBinType::eBitRightShift, &aValInt_Type, &aValInt_Type, (OpBinFunc)opbin_bit_right_shift_scalar_scalar<int, int>);

    // 注册数量类型运算函数
    opbin_register_func(EOpBinType::eAdd, &aValQuantity_Type, &aValQuantity_Type, (OpBinFunc)opbin_add_quantity_quantity);
    opbin_register_func(EOpBinType::eSub, &aValQuantity_Type, &aValQuantity_Type, (OpBinFunc)opbin_sub_quantity_quantity);
    opbin_register_func(EOpBinType::eMul, &aValQuantity_Type, &aValQuantity_Type, (OpBinFunc)opbin_mul_quantity_quantity);
    opbin_register_func(EOpBinType::eDiv, &aValQuantity_Type, &aValQuantity_Type, (OpBinFunc)opbin_div_quantity_quantity);

    // 注册数量类型与标量类型运算函数
    opbin_register_func(EOpBinType::eMul, &aValQuantity_Type, aValueType<double>(), (OpBinFunc)opbin_mul_quantity_scalar<double>);
    opbin_register_func(EOpBinType::eDiv, &aValQuantity_Type, aValueType<double>(), (OpBinFunc)opbin_div_quantity_scalar<double>);
    opbin_register_func(EOpBinType::eMul, &aValQuantity_Type, aValueType<int>(), (OpBinFunc)opbin_mul_quantity_scalar<int>);
    opbin_register_func(EOpBinType::eDiv, &aValQuantity_Type, aValueType<int>(), (OpBinFunc)opbin_div_quantity_scalar<int>);
    opbin_register_func(EOpBinType::eMul, aValueType<double>(),  &aValQuantity_Type, (OpBinFunc)opbin_mul_scalar_quantity<double>);
    opbin_register_func(EOpBinType::eDiv, aValueType<double>(),  &aValQuantity_Type, (OpBinFunc)opbin_div_scalar_quantity<double>);
    opbin_register_func(EOpBinType::eMul, aValueType<int>(),  &aValQuantity_Type, (OpBinFunc)opbin_mul_scalar_quantity<int>);
    opbin_register_func(EOpBinType::eDiv, aValueType<int>(),  &aValQuantity_Type, (OpBinFunc)opbin_div_scalar_quantity<int>);
  
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
Value* opbin(EOpBinType op, Value* left, Value* right)
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
    return nullptr;
}



AST_NAMESPACE_END