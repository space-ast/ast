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

#include "OperatorBinary.hpp"
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


//  @brief 实现二元运算符的标量-标量版本
//  @param OPNAME 运算符名称
//  @param OP 运算符符号
#define _AST_BINOP_SCALAR_SCALAR(OPNAME, OP) \
template<typename Left, typename Right> \
Value* binop_##OPNAME##_scalar_scalar(Value* left, Value* right) \
{ \
    auto leftval  = static_cast<ValScalar<Left>*>(left); \
    auto rightval = static_cast<ValScalar<Right>*>(right); \
    return aNewValue(leftval->value() OP rightval->value()); \
}


//  @brief 实现二元运算符的标量-标量版本
_AST_BINOP_SCALAR_SCALAR(add, +)
_AST_BINOP_SCALAR_SCALAR(sub, -)
_AST_BINOP_SCALAR_SCALAR(mul, *)
_AST_BINOP_SCALAR_SCALAR(div, /)
// _AST_BINOP_SCALAR_SCALAR(mod, %)
_AST_BINOP_SCALAR_SCALAR(eq, ==)
_AST_BINOP_SCALAR_SCALAR(ne, !=)
_AST_BINOP_SCALAR_SCALAR(lt, <)
_AST_BINOP_SCALAR_SCALAR(gt, >)
_AST_BINOP_SCALAR_SCALAR(le, <=)
_AST_BINOP_SCALAR_SCALAR(ge, >=)




//  @brief 执行加法运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_add(Value* left, Value* right)
{
    // @todo 实现加法运算
    return nullptr;
}


//  @brief 执行减法运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_sub(Value* left, Value* right)
{
    // @todo 实现减法运算
    return nullptr;
}


//  @brief 执行乘法运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_mul(Value* left, Value* right)
{
    // @todo 实现乘法运算
    return nullptr;
}

//  @brief 执行除法运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_div(Value* left, Value* right)
{
    // @todo 实现除法运算
    return nullptr;
}

//  @brief 执行取模运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_mod(Value* left, Value* right)
{
    // @todo 实现取模运算
    return nullptr;
}

//  @brief 执行乘方运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_pow(Value* left, Value* right)
{
    // @todo 实现乘方运算
    return nullptr;
}

//  @brief 执行按位与运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_and(Value* left, Value* right)
{
    // @todo 实现按位与运算
    return nullptr;
}


//  @brief 执行按位或运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_or(Value* left, Value* right)
{
    // @todo 实现按位或运算
    return nullptr;
}

//  @brief 执行按位异或运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_xor(Value* left, Value* right)
{
    // @todo 实现按位异或运算
    return nullptr;
}

//  @brief 执行判等运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_eq(Value* left, Value* right)
{
    // @todo 实现判等运算
    return nullptr;
}

//  @brief 执行判不等运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_ne(Value* left, Value* right)
{
    // @todo 实现判不等运算
    return nullptr;
}

//  @brief 执行大于运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_gt(Value* left, Value* right)
{
    // @todo 实现大于运算
    return nullptr;
}

//  @brief 执行大于等于运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_ge(Value* left, Value* right)
{
    // @todo 实现大于等于运算
    return nullptr;
}


//  @brief 执行小于运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_lt(Value* left, Value* right)
{
    // @todo 实现小于运算
    return nullptr;
}

//  @brief 执行小于等于运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_le(Value* left, Value* right)
{
    // @todo 实现小于等于运算
    return nullptr;
}



//  @brief 执行按元素乘运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_elem_mul(Value* left, Value* right)
{
    // @todo 实现按元素乘运算
    return nullptr;
}


//  @brief 执行按元素除运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_elem_div(Value* left, Value* right)
{
    // @todo 实现按元素除运算
    return nullptr;
}


//  @brief 执行按元素取模运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_elem_mod(Value* left, Value* right)
{
    // @todo 实现按元素取模运算
    return nullptr;
}

//  @brief 执行按元素乘方运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_elem_pow(Value* left, Value* right)
{
    // @todo 实现按元素乘方运算
    return nullptr;
}

//  @brief 执行按元素与运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_elem_and(Value* left, Value* right)
{
    // @todo 实现按元素与运算
    return nullptr;
}


//  @brief 执行按元素或运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_elem_or(Value* left, Value* right)
{
    // @todo 实现按元素或运算
    return nullptr;
}

//  @brief 执行按元素异或运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_elem_xor(Value* left, Value* right)
{
    // @todo 实现按元素异或运算
    return nullptr;
}

//  @brief 执行按元素相等运算
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
static Value* binop_elem_eq(Value* left, Value* right)
{
    // @todo 实现按元素相等运算
    return nullptr;
}




//  @brief 运算函数注册表键类型
typedef std::tuple<OpBinType, Class*, Class*> BinOpKey;

//  @brief 运算函数注册表哈希函数
struct BinOpKeyHash {
    std::size_t operator()(const BinOpKey& key) const {
        // 组合哈希值
        std::size_t h1 = std::hash<int>()(static_cast<int>(std::get<0>(key)));
        std::size_t h2 = std::hash<const void*>()(std::get<1>(key));
        std::size_t h3 = std::hash<const void*>()(std::get<2>(key));
        // 使用位运算组合哈希值
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

//  @brief 运算函数注册表类型
typedef std::unordered_map<BinOpKey, OpBinFunc, BinOpKeyHash> BinOpRegistry;

//  @brief 获取全局运算函数注册表
//  @return 全局运算函数注册表引用
BinOpRegistry& get_binop_registry() {
    static BinOpRegistry registry;
    return registry;
}

//  @brief 注册二进制运算函数
//  @param op 运算类型
//  @param leftType 左运算数类型
//  @param rightType 右运算数类型
//  @param func 运算函数指针
void binop_register_func(OpBinType op, Class* leftType, Class* rightType, OpBinFunc func) {
    get_binop_registry()[{op, leftType, rightType}] = func;
}

//  @brief 获取二进制运算函数指针
//  @param op 运算类型
//  @param leftType 左运算数类型
//  @param rightType 右运算数类型
//  @return 运算函数指针
OpBinFunc binop_get_func(OpBinType op, Class* leftType, Class* rightType) 
{
    auto& registry = get_binop_registry();
    auto key = BinOpKey{op, leftType, rightType};
    auto it = registry.find(key);
    if (it != registry.end()) {
        return it->second;
    }
    return nullptr;
}

//  @brief 初始化二进制运算函数注册表
void binop_init_registry() {
    auto& registry = get_binop_registry();
    
    // 注册标量类型运算函数
    #define REGISTER_BINOP(op, left_type, right_type, func) \
        binop_register_func(OpBinType::op, left_type, right_type, (OpBinFunc)func)
    
    // 注册整数-整数运算
    REGISTER_BINOP(eAdd, &aValInt_Type, &aValInt_Type, (binop_add_scalar_scalar<int, int>));
    REGISTER_BINOP(eSub, &aValInt_Type, &aValInt_Type, (binop_sub_scalar_scalar<int, int>));
    REGISTER_BINOP(eMul, &aValInt_Type, &aValInt_Type, (binop_mul_scalar_scalar<int, int>));
    REGISTER_BINOP(eDiv, &aValInt_Type, &aValInt_Type, (binop_div_scalar_scalar<int, int>));
    //REGISTER_BINOP(eMod, &aValInt_Type, &aValInt_Type, (binop_mod_scalar_scalar<int, int>));
    REGISTER_BINOP(eEq, &aValInt_Type, &aValInt_Type, (binop_eq_scalar_scalar<int, int>));
    REGISTER_BINOP(eNe, &aValInt_Type, &aValInt_Type, (binop_ne_scalar_scalar<int, int>));
    REGISTER_BINOP(eLt, &aValInt_Type, &aValInt_Type, (binop_lt_scalar_scalar<int, int>));
    REGISTER_BINOP(eGt, &aValInt_Type, &aValInt_Type, (binop_gt_scalar_scalar<int, int>));
    REGISTER_BINOP(eLe, &aValInt_Type, &aValInt_Type, (binop_le_scalar_scalar<int, int>));
    REGISTER_BINOP(eGe, &aValInt_Type, &aValInt_Type, (binop_ge_scalar_scalar<int, int>));
    
    // 注册整数-双精度运算
    REGISTER_BINOP(eAdd, &aValInt_Type, &aValDouble_Type, (binop_add_scalar_scalar<int, double>));
    REGISTER_BINOP(eSub, &aValInt_Type, &aValDouble_Type, (binop_sub_scalar_scalar<int, double>));
    REGISTER_BINOP(eMul, &aValInt_Type, &aValDouble_Type, (binop_mul_scalar_scalar<int, double>));
    REGISTER_BINOP(eDiv, &aValInt_Type, &aValDouble_Type, (binop_div_scalar_scalar<int, double>));
    //REGISTER_BINOP(eMod, &aValInt_Type, &aValDouble_Type, (binop_mod_scalar_scalar<int, double>));
    REGISTER_BINOP(eEq, &aValInt_Type, &aValDouble_Type, (binop_eq_scalar_scalar<int, double>));
    REGISTER_BINOP(eNe, &aValInt_Type, &aValDouble_Type, (binop_ne_scalar_scalar<int, double>));
    REGISTER_BINOP(eLt, &aValInt_Type, &aValDouble_Type, (binop_lt_scalar_scalar<int, double>));
    REGISTER_BINOP(eGt, &aValInt_Type, &aValDouble_Type, (binop_gt_scalar_scalar<int, double>));
    REGISTER_BINOP(eLe, &aValInt_Type, &aValDouble_Type, (binop_le_scalar_scalar<int, double>));
    REGISTER_BINOP(eGe, &aValInt_Type, &aValDouble_Type, (binop_ge_scalar_scalar<int, double>));
    
    // 注册双精度-整数运算
    REGISTER_BINOP(eAdd, &aValDouble_Type, &aValInt_Type, (binop_add_scalar_scalar<double, int>));
    REGISTER_BINOP(eSub, &aValDouble_Type, &aValInt_Type, (binop_sub_scalar_scalar<double, int>));
    REGISTER_BINOP(eMul, &aValDouble_Type, &aValInt_Type, (binop_mul_scalar_scalar<double, int>));
    REGISTER_BINOP(eDiv, &aValDouble_Type, &aValInt_Type, (binop_div_scalar_scalar<double, int>));
    //REGISTER_BINOP(eMod, &aValDouble_Type, &aValInt_Type, (binop_mod_scalar_scalar<double, int>));
    REGISTER_BINOP(eEq, &aValDouble_Type, &aValInt_Type, (binop_eq_scalar_scalar<double, int>));
    REGISTER_BINOP(eNe, &aValDouble_Type, &aValInt_Type, (binop_ne_scalar_scalar<double, int>));
    REGISTER_BINOP(eLt, &aValDouble_Type, &aValInt_Type, (binop_lt_scalar_scalar<double, int>));
    REGISTER_BINOP(eGt, &aValDouble_Type, &aValInt_Type, (binop_gt_scalar_scalar<double, int>));
    REGISTER_BINOP(eLe, &aValDouble_Type, &aValInt_Type, (binop_le_scalar_scalar<double, int>));
    REGISTER_BINOP(eGe, &aValDouble_Type, &aValInt_Type, (binop_ge_scalar_scalar<double, int>));
    
    // 注册双精度-双精度运算
    REGISTER_BINOP(eAdd, &aValDouble_Type, &aValDouble_Type, (binop_add_scalar_scalar<double, double>));
    REGISTER_BINOP(eSub, &aValDouble_Type, &aValDouble_Type, (binop_sub_scalar_scalar<double, double>));
    REGISTER_BINOP(eMul, &aValDouble_Type, &aValDouble_Type, (binop_mul_scalar_scalar<double, double>));
    REGISTER_BINOP(eDiv, &aValDouble_Type, &aValDouble_Type, (binop_div_scalar_scalar<double, double>));
    //REGISTER_BINOP(eMod, &aValDouble_Type, &aValDouble_Type, (binop_mod_scalar_scalar<double, double>));
    REGISTER_BINOP(eEq, &aValDouble_Type, &aValDouble_Type, (binop_eq_scalar_scalar<double, double>));
    REGISTER_BINOP(eNe, &aValDouble_Type, &aValDouble_Type, (binop_ne_scalar_scalar<double, double>));
    REGISTER_BINOP(eLt, &aValDouble_Type, &aValDouble_Type, (binop_lt_scalar_scalar<double, double>));
    REGISTER_BINOP(eGt, &aValDouble_Type, &aValDouble_Type, (binop_gt_scalar_scalar<double, double>));
    REGISTER_BINOP(eLe, &aValDouble_Type, &aValDouble_Type, (binop_le_scalar_scalar<double, double>));
    REGISTER_BINOP(eGe, &aValDouble_Type, &aValDouble_Type, (binop_ge_scalar_scalar<double, double>));
    
    // 注册布尔-布尔运算
    REGISTER_BINOP(eEq, &aValBool_Type, &aValBool_Type, (binop_eq_scalar_scalar<bool, bool>));
    REGISTER_BINOP(eNe, &aValBool_Type, &aValBool_Type, (binop_ne_scalar_scalar<bool, bool>));
    
    #undef REGISTER_BINOP
}

// 初始化注册表
auto __binop_registry_initializer = []() -> bool {
    binop_init_registry();
    return true;
}();


//  @brief 执行二进制运算
//  @param op 运算类型
//  @param left 左运算数
//  @param right 右运算数
//  @return 运算结果
Value* binop(OpBinType op, Value* left, Value* right)
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
    OpBinFunc func = binop_get_func(op, leftType, rightType);
    if (func) {
        return func(left, right);
    }
    
    // 如果没有找到对应的运算函数，使用默认实现 
    switch (op)
    {
    case OpBinType::eAdd:
        return binop_add(left, right);
    case OpBinType::eSub:
        return binop_sub(left, right);
    case OpBinType::eMul:
        return binop_mul(left, right);
    case OpBinType::eDiv:
        return binop_div(left, right);
    case OpBinType::eMod:
        return binop_mod(left, right);
    case OpBinType::ePow:
        return binop_pow(left, right);
    case OpBinType::eEq:
        return binop_eq(left, right);
    case OpBinType::eNe:
        return binop_ne(left, right);
    case OpBinType::eGt:
        return binop_gt(left, right);
    case OpBinType::eGe:
        return binop_ge(left, right);
    case OpBinType::eLt:
        return binop_lt(left, right);
    case OpBinType::eLe:
        return binop_le(left, right);
    default:
        aError("Unsupported binary operator: %d", op);
        return nullptr;
    }
}



AST_NAMESPACE_END