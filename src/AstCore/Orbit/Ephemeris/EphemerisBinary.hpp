///
/// @file      EphemerisBinary.hpp
/// @brief     文件后备星历
/// @details   将星历点以二进制格式存储在磁盘文件中，
///            查询时按需从文件读取，配合固定窗口缓存减少 IO。
/// @author    axel
/// @date      2026-06-29
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
#include "AstCore/Ephemeris.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/Interval.hpp"
#include "AstMath/Vector.hpp"
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief 文件后备星历
/// @details 将星历数据写入二进制文件，查询时从文件读取。
///          使用固定窗口缓存减少磁盘 IO，适合大规模星历存储场景。
///
/// 二进制文件格式:
///   [Header: 56 bytes]
///     magic(4B)        = 0x41535445  ("ASTE")
///     version(4B)      = 1
///     epochInt(8B)     = 历元整数部分 (JD day)
///     epochFrac(8B)    = 历元小数部分 (day fraction)
///     pointCount(8B)   = 星历点数 N
///     startTime(8B)    = 首个星历点相对历元的秒数
///     stopTime(8B)     = 末尾星历点相对历元的秒数
///     frameNameLen(8B) = 坐标系名称长度
///   [FrameName: frameNameLen bytes]  (UTF-8)
///   [Body: N × 56 bytes]
///     time(8B) | px(8B) | py(8B) | pz(8B) | vx(8B) | vy(8B) | vz(8B)
///     (time = 相对历元的秒数, pos = m, vel = m/s)
class AST_CORE_API EphemerisBinary final: public Ephemeris
{
public:
    static EphemerisBinary* New();

    EphemerisBinary() = default;
    ~EphemerisBinary() override = default;

public:
    /// @brief 从源星历保存到二进制文件，保存后对象立即可查询
    /// @param source    源星历 (通常为 EphemerisLagrangeVar)
    /// @param filepath  目标文件路径
    /// @return 错误码
    errc_t saveFrom(const Ephemeris* source, const std::string& filepath);

    /// @brief 从二进制文件打开星历
    /// @param filepath  文件路径
    /// @return 错误码
    errc_t open(const std::string& filepath);

public: // Ephemeris 接口
    Frame*  getFrame()                                    const override;
    errc_t  getPos(const TimePoint& tp, Vector3d& pos)    const override;
    errc_t  getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;
    errc_t  getInterval(TimeInterval& interval)           const override;

private:
    /// @brief 从文件读取一批点填充窗口缓存
    void fillWindow(size_t idx) const;

    /// @brief 在窗口内二分查找时间索引
    int findIndex(double delta) const;

    static constexpr uint32_t MAGIC       = 0x41535445;   // "ASTE"
    static constexpr uint32_t VERSION     = 1;
    static constexpr size_t   WINDOW_SIZE = 256;           // 固定窗口大小
    static constexpr int      INTERP_ORDER = 5;            // Lagrange 插值阶数

    TimePoint           epoch_{};                 ///< 历元时间
    Interval            interval_{};              ///< 星历时间范围，相对于历元时间
    double              averageStep_{60.0};       ///< 平均时间步长，用于估算索引
    size_t              pointCount_{0};           ///< 总星历点数
    std::string         filepath_;                ///< 二进制文件路径
    std::string         frameName_;               ///< 坐标系名称
    mutable SharedPtr<Frame> frame_;              ///< 解析后的坐标系指针

    // 固定窗口缓存
    static constexpr size_t HEADER_BASE = 56;    // 基础头大小 (不含变长帧名)
    static constexpr size_t POINT_BYTES = 56;    // 7 doubles per point
    mutable std::vector<double>   winTimes_;     ///< 窗口内的时间 (秒, 相对历元)
    mutable std::vector<Vector3d> winPos_;       ///< 窗口内的位置
    mutable std::vector<Vector3d> winVel_;       ///< 窗口内的速度
    mutable size_t winStart_{0};                  ///< 窗口在文件中的起始索引
};

/*! @} */

AST_NAMESPACE_END
