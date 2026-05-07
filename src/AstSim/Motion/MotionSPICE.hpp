///
/// @file      MotionSPICE.hpp
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
#include "AstCore/EventInterval.hpp"
#include "AstCore/BodyEphemerisSPK.hpp"
#include "AstCore/JplSpk.hpp"
#include "AstUtil/SPKParser.hpp"
#include "AstSim/MotionProfile.hpp"
#include "AstSim/MotionWithIntervalStep.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief SPICE星历预报器/运动模型类
/// @details  读取SPICE星历文件，生成星历数据
class AST_SIM_API MotionSPICE: public MotionWithIntervalStep
{
public:
    static MotionSPICE* New();
    MotionSPICE() : spiceIndex_() {}
    ~MotionSPICE() {}
public:
    errc_t makeEphemerisSpec(ScopedPtr<Ephemeris>& eph) const override;
    errc_t makeEphemerisSimple(ScopedPtr<Ephemeris>& eph) const override;
    void accept(MotionProfileVisitor& visitor) override;

    const std::string& getSpiceFile() const {return spk_.getFilePath();}
    errc_t setSpiceFile(StringView spiceFile);

    int getSpiceIndex() const {return spiceIndex_;}
    void setSpiceIndex(int spiceIndex){spiceIndex_ = spiceIndex;}
protected:
    int spiceIndex_{};                    ///< SPICE星历索引号
private:
    JplSpk spk_;                        ///< SPICE星历接口
};

/*! @} */

AST_NAMESPACE_END