///
/// @file      TLE.cpp
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

#include "TLE.hpp"
#include "TLELines.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/ParseFormat.hpp"

#include <cmath>

AST_NAMESPACE_BEGIN


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


/// @brief 解析 TLE 科学记数法字段（如二阶导数、BSTAR），返回合值
/// @details 字段格式为 [符号位][5 位小数][指数符号][指数数字]，假设前导小数点。
///          例如 " 10191-3" → 0.10191 × 10⁻³ = 0.00010191。
/// @param raw0 源字符串中指向该字段开头的指针（至少 8 字节）
/// @return 合并后的 double 值
static double parseTLEExpField(const char* raw0)
{
    // 尾数：[0] 符号位, [1-5] 5 位小数，假设前导小数点
    char signChar = raw0[0];
    std::string mantStr("0.");
    mantStr.append(raw0 + 1, 5);
    // TLE 惯例：空格视为 '0'
    for (size_t i = 2; i < mantStr.size(); ++i)
    {
        if (mantStr[i] == ' ') mantStr[i] = '0';
    }
    double mval = aParseDouble(mantStr);
    if (signChar == '-') mval = -mval;

    // 指数：[6] 符号, [7] 数字（空格→0）
    char expDigit = (raw0[7] == ' ') ? '0' : raw0[7];
    int expVal = (expDigit - '0');
    if (raw0[6] == '-') expVal = -expVal;

    return mval * pow(10.0, expVal);
}


TLE TLE::FromLines(const TLELines &lines)
{
    StringView l1 = lines.line1();
    StringView l2 = lines.line2();
    return FromLines(l1, l2);
}

TLE TLE::FromLines(StringView l1, StringView l2)
{
    TLE tle{};
    
    tle.lines_.line1_ = std::string(l1);
    tle.lines_.line2_ = std::string(l2);

    // ---- 解析第1行（69 字符）----
    if (l1.size() >= 69)
    {
        // 列 08: 密级分类 (1-based 8 → 0-based 7)
        tle.classification_ = l1[7];

        // 列 19-20: 历元年份（年份后两位）
        int epochYear = aParseInt(l1.substr(18, 2));

        // 列 21-32: 历元（一年中的第几天及小数部分）
        double epochDays = aParseDouble(l1.substr(20, 12));

        tle.epochTime_ = TimePoint::FromTLEYD(epochYear * 1000.0 + epochDays);

        // 列 34-43: 平均运动的一阶时间导数 [圈/天²]
        tle.meanMotionDotTime_ = aParseDouble(l1.substr(33, 10));

        // 列 45-52: 平均运动的二阶时间导数（科学记数法，假设前导小数点）
        tle.motionDotDot_ = parseTLEExpField(&l1[44]);

        // 列 54-61: BSTAR 阻力项（格式同上）
        tle.bstar_ = parseTLEExpField(&l1[53]);

        // 列 63: 星历表类型
        tle.ephType_ = aParseInt(l1.substr(62, 1));

        // 列 65-68: 轨道根数编号
        tle.elementNumber_ = aParseInt(l1.substr(64, 4));
    }

    // ---- 解析第2行（69 字符）----
    if (l2.size() >= 69)
    {
        // 列 09-16: 轨道倾角 [度] → 弧度
        tle.inclination_ = aParseDouble(l2.substr(8, 8)) * kDegToRad;

        // 列 18-25: 升交点赤经 [度] → 弧度
        tle.rightAscenOfNode_ = aParseDouble(l2.substr(17, 8)) * kDegToRad;

        // 列 27-33: 偏心率（假设前导小数点，7 位数字）
        tle.eccentricity_ = aParseDouble(
            std::string("0.").append(l2.data() + 26, 7));

        // 列 35-42: 近地点幅角 [度] → 弧度
        tle.argOfPerigee_ = aParseDouble(l2.substr(34, 8)) * kDegToRad;

        // 列 44-51: 平近点角 [度] → 弧度
        tle.meanAnomaly_ = aParseDouble(l2.substr(43, 8)) * kDegToRad;

        // 列 53-63: 平均运动 [圈/天] → rad/s
        tle.meanMotion_ = aParseDouble(l2.substr(52, 11)) * kTwoPI / 86400.0;

        // 列 64-68: 历元时刻的已转圈数
        tle.revNumber_ = aParseInt(l2.substr(63, 5));
    }

    return tle;
}

AST_NAMESPACE_END
