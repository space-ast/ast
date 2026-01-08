///
/// @file      EOP.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      30.11.2025
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
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include <vector>

AST_NAMESPACE_BEGIN

class EOP
{
public:
    struct Entry
    {
        int mjd;
        double x, y;            ///< 极移
        double ut1_utc;         ///< ut1 - utc
        double lod;             ///< length of day
        double dpsi, deps;      
        double dx, dy;
        // int dat;
    };
public:
    EOP() = default;
    ~EOP() = default;

    /// @brief 加载EOP数据
    /// @param filepath 文件路径
    /// @return 错误码
    err_t load(StringView filepath);

    /// @brief 加载EOP数据
    /// @param filepath 文件路径
    /// @param data 数据容器
    /// @return 错误码
    static err_t load(StringView filepath, std::vector<Entry>& data);

    
protected:
    std::vector<Entry> m_data;
};

AST_NAMESPACE_END