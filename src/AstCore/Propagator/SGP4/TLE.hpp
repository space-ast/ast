///
/// @file      TLE.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-18
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
#include "AstCore/TimePoint.hpp"
#include "AstCore/TLELines.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


enum class ETLESource
{
    eFile,      ///< 来自文件
    eEdited,    ///< 来自编辑
    eServer,    ///< 来自服务器
};

enum class ESwitchMethod
{
    eEpoch,
};



/// @brief 两行根数（TLE）
/// @details 存储从 TLE 文本解析出的轨道根数及元数据。
class AST_CORE_API TLE
{
public:
    TLE() = default;
    ~TLE() = default;

    TLE(const TLE&) = default;
    TLE& operator=(const TLE&) = default;
    TLE(TLE&&) = default;
    TLE& operator=(TLE&&) = default;

    /// @brief 从 TLE 行文本创建 TLE 对象
    /// @details 解析 TLE 行文本，提取轨道根数及元数据。
    /// @param lines TLE 行文本
    /// @return TLE 对象
    static TLE FromLines(const TLELines& lines);
    static TLE FromLines(StringView l1, StringView l2);

    // -- 元数据访问 --

    bool        enabled() const { return enabled_; }
    void        setEnabled(bool v) { enabled_ = v; }

    ETLESource  source() const { return source_; }
    void        setSource(ETLESource v) { source_ = v; }

    ESwitchMethod switchMethod() const { return switch_method_; }
    void          setSwitchMethod(ESwitchMethod v) { switch_method_ = v; }

    const TimePoint& switchEpoch() const { return switchEpoch_; }
    void             setSwitchEpoch(const TimePoint& v) { switchEpoch_ = v; }

    const TLELines& lines() const { return lines_; }
    TLELines&       lines() { return lines_; }

    const TimePoint& epochTime() const { return epochTime_; }
    void             setEpochTime(const TimePoint& v) { epochTime_ = v; }

    // -- 轨道根数访问 --

    double meanMotionDot() const { return meanMotionDotTime_; }
    void   setMeanMotionDot(double v) { meanMotionDotTime_ = v; }

    double motionDotDot() const { return motionDotDot_; }
    void   setMotionDotDot(double v) { motionDotDot_ = v; }

    double bstar() const { return bstar_; }
    void   setBstar(double v) { bstar_ = v; }

    double inclination() const { return inclination_; }
    void   setInclination(double v) { inclination_ = v; }

    double rightAscenOfNode() const { return rightAscenOfNode_; }
    void   setRightAscenOfNode(double v) { rightAscenOfNode_ = v; }

    double eccentricity() const { return eccentricity_; }
    void   setEccentricity(double v) { eccentricity_ = v; }

    double argOfPerigee() const { return argOfPerigee_; }
    void   setArgOfPerigee(double v) { argOfPerigee_ = v; }

    double meanAnomaly() const { return meanAnomaly_; }
    void   setMeanAnomaly(double v) { meanAnomaly_ = v; }

    double meanMotion() const { return meanMotion_; }
    void   setMeanMotion(double v) { meanMotion_ = v; }

    int  revNumber() const { return revNumber_; }
    void setRevNumber(int v) { revNumber_ = v; }

    int  ephType() const { return ephType_; }
    void setEphType(int v) { ephType_ = v; }

    int  noradId() const { return noradId_; }
    void setNoradId(int v) { noradId_ = v; }

    int  elementNumber() const { return elementNumber_; }
    void setElementNumber(int v) { elementNumber_ = v; }

    char classification() const { return classification_; }
    void setClassification(char v) { classification_ = v; }

private:
    bool enabled_{true};              ///< 是否启用
    ETLESource source_                ///< 来源类型
        {ETLESource::eFile};
    ESwitchMethod switch_method_      ///< 切换方法
        {ESwitchMethod::eEpoch};
    TimePoint switchEpoch_{};         ///< 切换历元
    TLELines lines_{};                ///< TLE 原始行文本
    TimePoint epochTime_{};           ///< 历元时间
    double meanMotionDotTime_{0.0};   ///< 平均运动导数
    double motionDotDot_{0.0};        ///< 平均运动二阶导数
    double bstar_{0.0};               ///< B* 阻力系数
    double inclination_{0.0};         ///< 倾角
    double rightAscenOfNode_{0.0};    ///< 升交点赤经
    double eccentricity_{0.0};        ///< 偏心率
    double argOfPerigee_{0.0};        ///< 近地点幅角
    double meanAnomaly_{0.0};         ///< 平近点角
    double meanMotion_{0.0};          ///< 平均运动
    int revNumber_{0};                ///< 轨道数
    int ephType_{0};                  ///< 星历类型
    int elementNumber_{0};            ///< 元素编号
    int noradId_{0};                  ///< NORAD 目录编号
    char classification_{'U'};        ///< 分类
};


/// @brief 解析 5 位 NORAD 目录编号（支持 Alpha-5 字母扩展）
/// @see https://www.space-track.org/documentation#tle-alpha5
/// @details Alpha-5 将 5 位编号的首位替换为字母（A-Z，跳过 I/O），字母值 10-33，
///          最终编号 = 首位值 × 10000 + 后 4 位数字。
///          首位为数字或空格时按传统整数解析（< 100000 不受影响）。
/// @param str 5 字符字段（TLE/SATCAT 目录编号列）
/// @return NORAD 目录编号；解析失败返回 0
AST_CORE_CAPI int aParseNoradId(StringView str);


/// @brief 将 TLE 转换为经典轨道根数
/// @param tle TLE 根数
/// @param gm 引力常数 [m^3/s^2]
/// @param elem 输出经典轨道根数
AST_CORE_API void aTLEToOrbElem(const TLE& tle, double gm, OrbElem& elem);
AST_CORE_API OrbElem aTLEToOrbElem(const TLE& tle, double gm);

/// @brief 将 TLE 转换为修改后的轨道根数
/// @param tle TLE 根数
/// @param gm 引力常数 [m^3/s^2]
/// @param elem 输出修改后的轨道根数
AST_CORE_API void aTLEToModOrbElem(const TLE& tle, double gm, ModOrbElem& elem);
AST_CORE_API ModOrbElem aTLEToModOrbElem(const TLE& tle, double gm);


/*
参考：https://celestrak.org/NORAD/documentation/tle-fmt.php

NORAD 两行轨道根数格式
每颗卫星的数据由三行组成，格式如下：
AAAAAAAAAAAAAAAAAAAAAAAA
1 NNNNNU NNNNNAAA NNNNN.NNNNNNNN +.NNNNNNNN +NNNNN-N +NNNNN-N N NNNNN
2 NNNNN NNN.NNNN NNN.NNNN NNNNNNN NNN.NNNN NNN.NNNN NN.NNNNNNNNNNNNNN
列号索引：
123456789111111111122222222223333333333444444444455555555556666666666
         012345678901234567890123456789012345678901234567890123456789

第0行是24个字符的卫星名称（与NORAD卫星目录中的名称长度一致）。

第1行和第2行是标准两行轨道根数格式，与NORAD和NASA使用的格式相同。格式说明如下：

第1行
列       说明
01       轨道根数行号
03-07    卫星编号
08       密级分类（U=非保密）
10-11    国际编号（发射年份后两位）
12-14    国际编号（当年发射序号）
15-17    国际编号（此次发射的部件编号）
19-20    历元年份（年份后两位）
21-32    历元（一年中的第几天及小数部分）
34-43    平均运动的一阶时间导数
45-52    平均运动的二阶时间导数（假设前导小数点）
54-61    BSTAR阻力项（假设前导小数点）
63       星历表类型
65-68    轨道根数编号
69       校验和（模10）
（字母、空格、句点、加号 = 0；减号 = 1）

第2行
列       说明
01       轨道根数行号
03-07    卫星编号
09-16    轨道倾角 [度]
18-25    升交点赤经 [度]
27-33    偏心率（假设前导小数点）
35-42    近地点幅角 [度]
44-51    平近点角 [度]
53-63    平均运动 [圈/天]
64-68    历元时刻的已转圈数 [圈]
69       校验和（模10）

其余各列均为空白或固定值。

示例：
123456789111111111122222222223333333333444444444455555555556666666666
         012345678901234567890123456789012345678901234567890123456789
NOAA 14
1 23455U 94089A   97320.90946019  .00000140  00000-0  10191-3 0  2621
2 23455  99.0090 272.6745 0008546 223.1686 136.8816 14.11711747148495
NOAA 6
1 11416U          86 50.28438588 0.00000140           67960-4 0  5293
2 11416  98.5105  69.3305 0012788  63.2828 296.9658 14.24899292346978

*/


/*! @} */

AST_NAMESPACE_END
