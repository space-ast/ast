///
/// @file      EphemerisSPK.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-06
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
#include "BodyEphemeris.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/JplSpk.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup SolarSystem
    @{
*/


class AST_CORE_API BodyEphemerisSPK : public BodyEphemeris
{
public:
    AST_OBJECT(BodyEphemerisSPK)
    AST_PROPERT(SpiceIndex)
    BodyEphemerisSPK() = default;

    BodyEphemerisSPK(const BodyEphemerisSPK&) = delete;
    BodyEphemerisSPK& operator=(const BodyEphemerisSPK&) = delete;
    
    BodyEphemerisSPK(int spiceIndex)
        : spiceIndex_(spiceIndex)
        , spk_()
    {}
    BodyEphemerisSPK(CelestialBody* body)
        : body_(body)
        , spiceIndex_(body->getJplSpiceId())
        , spk_()
    {}

    ~BodyEphemerisSPK() override = default;

    errc_t openSPKFile(StringView spkfile);

    const std::string& getFilePath() const;

    errc_t getPosICRF(const TimePoint& tp, Vector3d& pos) const override;

    errc_t getPosVelICRF(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;

    errc_t getInterval(TimeInterval &interval) const override;
PROPERTIES:
    int getSpiceIndex() const;

    void setSpiceIndex(int spiceIndex){spiceIndex_ = spiceIndex;}
protected:
    CelestialBody* body_{nullptr};
    int            spiceIndex_{-1};
    JplSpk         spk_{};
};


/*! @} */

AST_NAMESPACE_END
