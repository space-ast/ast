///
/// @file      UnitConverter.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-15
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Unit.hpp"

AST_NAMESPACE_BEGIN


/// @brief 单位转换器
/// @ingroup Quantity
class UnitConverter
{
public:
    UnitConverter():fromUnit_(Unit()), toUnit_(Unit()), conversionFactor_(1.0){}

    UnitConverter(Unit fromUnit, Unit toUnit)
        : fromUnit_(fromUnit), toUnit_(toUnit), conversionFactor_(fromUnit.getScale() / toUnit.getScale()){}

    /// @brief 转换单位
    /// @param value 待转换的值
    /// @return 转换后的值
    double convert(double value) const{return value * conversionFactor_;}

    /// @brief 获取转换前的单位
    /// @return 转换前的单位
    Unit getFromUnit() const{return fromUnit_;}
    
    /// @brief 获取转换后的单位
    /// @return 转换后的单位
    Unit getToUnit() const{return toUnit_;}

    /// @brief 设置转换前的单位
    void setFromUnit(Unit fromUnit){fromUnit_ = fromUnit; syncConversionFactor();}
    
    /// @brief 设置转换后的单位
    void setToUnit(Unit toUnit){toUnit_ = toUnit; syncConversionFactor();}

    /// @brief 获取转换因子
    /// @return 转换因子
    double getConversionFactor() const{return conversionFactor_;}

protected:
    /// @brief 同步转换因子
    void syncConversionFactor(){conversionFactor_ = fromUnit_.getScale() / toUnit_.getScale();}
    
protected:
    Unit fromUnit_;                 ///< 转换前的单位
    Unit toUnit_;                   ///< 转换后的单位
    double conversionFactor_;       ///< 转换因子
};

AST_NAMESPACE_END
