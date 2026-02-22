///
/// @file      SpaceWeather.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-16
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
#include "AstUtil/StringView.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/

/// @brief 空间天气数据
/// @details 
/// 支持加载从Celestrak网站下载的空间天气数据文件。
/// @see https://celestrak.org/SpaceData/SpaceWx-format.php
class AST_CORE_API SpaceWeather
{
public:
    /// @brief 空间天气数据条目
    struct Entry{
        int mjd;
        int BSRN;
        int ND;
        int Kp1, Kp2, Kp3, Kp4, Kp5, Kp6, Kp7, Kp8;
        int KpSum;
        int Ap1, Ap2, Ap3, Ap4, Ap5, Ap6, Ap7, Ap8;
        int ApAvg;
        double Cp;
        int C9;
        int ISN;
        int fluxQualifier;
        double F10p7Adj;
        double F10p7AdjCtr81;
        double F10p7AdjLst81;
        double F10p7Obs;
        double F10p7ObsCtr81;
        double F10p7ObsLst81;
    };
    SpaceWeather() = default;
    ~SpaceWeather() = default;

    /// @brief 加载默认空间天气数据文件
    /// @return 错误码
    err_t loadDefault();

    /// @brief 加载空间天气数据文件
    /// @param filepath 文件路径
    /// @return 错误码
    err_t load(StringView filepath);

    /// @brief 获取指定MJD的空间天气数据条目
    /// @param mjd 简约儒略日
    /// @return 空间天气数据条目指针
    /// @details 如果MJD不存在，则返回nullptr。
    const Entry* getEntry(int mjd) const;

    /// @brief 设置指定MJD的空间天气数据条目
    /// @param mjd 简约儒略日
    /// @param entry 空间天气数据条目
    /// @return 错误码
    err_t setEntry(int mjd, const Entry& entry);
protected:
    static err_t load(StringView filepath, std::vector<Entry>& data);

    void findEntryIndex(double mjdUTC, int& index, double& frac) const;

protected:
    std::vector<Entry> data_;
    int startMJD_ = 0;
    int endMJD_ = 0;
};

/*! @} */

AST_NAMESPACE_END
