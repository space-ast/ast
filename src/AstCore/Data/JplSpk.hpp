///
/// @file      JplSpk.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-26
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "AstCore/TimeInterval.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief  JPL SPK文件
/// @details 用于读取JPL SPK文件
class AST_CORE_API JplSpk
{
public:
    JplSpk();
    explicit JplSpk(StringView spkfile);
    ~JplSpk();

    /// @brief 打开SPK文件
    /// @param filepath SPK文件路径
    /// @return 错误码
    errc_t open(StringView spkfile);

    /// @brief 检查SPK文件是否已打开
    /// @return 是否已打开
    bool isOpen() const {return handle_ != 0;}

    /// @brief 关闭SPK文件
    /// @return 错误码
    errc_t close();

    /// @brief 获取SPK文件路径
    /// @return SPK文件路径
    const std::string& getFilePath() const {return spkfile_;}

    /// @brief 获取SPK文件句柄(CSPICE库返回的文件句柄)
    /// @return SPK文件句柄
    int handle() const{return handle_;}

    /// @brief 获取目标体相对于参考体的时间点的位置(ICRF系下)
    static errc_t getPosICRF(
        const TimePoint& tp, 
        int target,
        int referenceBody,
        Vector3d& pos
    );

    /// @brief 获取目标体相对于参考体的时间点的速度和位置(ICRF系下)
    static errc_t getPosVelICRF(
        const TimePoint& tp, 
        int target,
        int referenceBody,
        Vector3d& pos,
        Vector3d& vel
    );

    /// @brief 获取目标体的名称
    /// @param id 目标体ID
    /// @return 目标体名称
    static std::string getBodyName(int id);

    /// @brief 获取目标体的时间间隔
    errc_t getInterval(
        int target,
        TimeInterval& interval
    ) const;

    /// @brief 获取所有目标体的名称
    /// @param names 目标体名称向量
    /// @return 错误码
    errc_t getBodyNames(
        std::vector<std::string>& names
    ) const;

    /// @brief 获取所有目标体的ID
    /// @param ids 目标体ID向量
    /// @return 错误码
    errc_t getBodyIds(
        std::vector<int>& ids
    ) const;
protected:
    std::string spkfile_{};                  ///< SPK文件路径
    int handle_{0};                         ///< 库句柄
    mutable bool isIntervalCached_{false};  ///< 是否缓存了时间间隔
    mutable TimeInterval intervalCache_{};  ///< 时间间隔缓存
};

/*! @} */

AST_NAMESPACE_END
