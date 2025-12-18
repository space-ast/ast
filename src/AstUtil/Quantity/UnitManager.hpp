///
/// @file      UnitManager.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
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
#include <unordered_map>
#include <string>


AST_NAMESPACE_BEGIN

class Unit;

/// @brief 获取单位
/// @param name 单位名称
/// @return 单位
AST_UTIL_API Unit* aUnitGet(StringView name);


/// @brief 添加单位
/// @param unit 要添加的单位
/// @return err_t 错误码
AST_UTIL_API err_t aUnitAdd(const Unit& unit);


/// @brief 添加单位
/// @param name 单位名称
/// @param unit 要添加的单位
/// @return err_t 错误码
AST_UTIL_API err_t aUnitAdd(StringView name, const Unit& unit);


/// @brief 单位管理器
class AST_UTIL_API UnitManager
{
public:
    /// @brief 获取单位管理器默认实例
    /// @return 单位管理器默认实例  
    static UnitManager& Instance();

    UnitManager();

    ~UnitManager();

    /// @brief 添加单位
    /// @param unit 要添加的单位
    err_t addUnit(const Unit& unit);

    /// @brief 添加单位
    /// @param name 单位名称
    /// @param scale 单位缩放因子
    err_t addUnit(StringView name, const Unit& unit);

    /// @brief 获取单位
    /// @param name 单位名称
    /// @return 单位
    Unit* getUnit(StringView name);

protected:

    /// @brief 添加单位
    /// @param name 单位名称
    /// @param unit 要添加的单位
    /// @return err_t 错误码
    err_t addUnit(const std::string& name, const Unit& unit);

protected:
    std::unordered_map<std::string, Unit*> units_;       ///< 单位映射表
};


AST_NAMESPACE_END
