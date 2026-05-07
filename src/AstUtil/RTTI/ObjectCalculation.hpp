///
/// @file      ObjectCalculation.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-19
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
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 对象计算基类
/// @details 对象计算基类是所有对象计算类的基类，它定义了对象计算的基本接口
/// 不同类型的对象计算类通过继承自ObjectCalculation类来实现多态行为
/// 参考GMATMathSpec.pdf第三章 Calculation Objects
/// @ingroup RTTI
class AST_UTIL_API ObjectCalculation : public ObjectNamed
{
public:
    AST_OBJECT(ObjectCalculation)

    using ObjectNamed::ObjectNamed;
    ~ObjectCalculation() override = default;
public:
    /// @brief 获取期望输入的对象类型
    /// @return 期望输入的对象类型
    virtual Class* getExpectedType() const = 0;

    /// @brief 计算结果
    /// @param obj 要计算的对象
    /// @param result 计算结果
    /// @warning 该方法不检查对象类型是否符合预期类型，请在调用前进行类型检查或转换。
    /// 如果对象类型不符合预期类型，将导致运行时错误。
    /// @return 错误码
    virtual errc_t calculateNoCheckType(const Object* obj, double& result) = 0;

    /// @brief 计算结果
    /// @param obj 要计算的对象
    /// @param result 计算结果
    /// @return 错误码
    errc_t calculate(const Object* obj, double& result);

    /// @brief 检查对象类型是否符合预期类型
    /// @param obj 要检查的对象
    /// @return 是否符合预期类型
    bool isExpectedType(const Object* obj) const;
};



/*! @} */

AST_NAMESPACE_END
