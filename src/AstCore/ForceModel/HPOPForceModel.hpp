///
/// @file      HPOPForceModel.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-25
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
#include "AstCore/CelestialBody.hpp"
#include "AstCore/GravityForce.hpp"
#include "AstCore/PointMassForce.hpp"
#include "AstCore/DragForce.hpp"
#include "AstCore/ThirdBodyForce.hpp"
#include "AstCore/SolarRadiationPressure.hpp"
#include "AstCore/RelativityCorrection.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/ScopedPtr.hpp"
#include "AstUtil/ClonePtr.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/



/// @brief 高精度轨道预报力模型
class AST_CORE_API HPOPForceModel: public ObjectNamed
{
public:
    HPOPForceModel();
    ~HPOPForceModel();

    using ThirdBodyList = std::vector<ThirdBodyForce>;
    
    bool useDrag() const{return useDrag_;}
    void useDrag(bool useDrag){useDrag_ = useDrag;}

    bool useSRP() const{return useSRP_;}
    void useSRP(bool useSRP){useSRP_ = useSRP;}

    bool useRelativityCorrection() const{return useRelativityCorrection_;}
    void useRelativityCorrection(bool useRelativityCorrection){useRelativityCorrection_ = useRelativityCorrection;}

    /// @brief 获取中心天体
    Body* centralBody() const{return centralBody_.get();}
    /// @brief 设置中心天体
    void setCentralBody(Body* body){centralBody_ = body;}

    /// @brief 获取阻力模型
    DragForce& drag(){return drag_;}

    /// @brief 获取太阳辐射压模型
    SolarRadiationPressure& srp(){return srp_;}

    /// @brief 获取中心天体引力场模型
    /// @warning 调用该接口后，中心天体引力模型将被设置为引力场模型
    GravityForce& gravity();

    /// @brief 获取中心天体点质量引力模型
    /// @warning 调用该接口后，中心天体引力模型将被设置为点质量引力模型
    PointMassForce& pointMass();
    
    /// @brief 获取中心天体引力模型
    /// @return 中心天体引力模型
    BodyAttraction& bodyAttraction();

    /// @brief 获取中心天体引力模型
    const BodyAttraction& bodyAttraction() const;

    /// @brief 获取中心天体引力类型
    EBodyAttractionType bodyAttractionType() const;

    /// @brief 添加三体引力模型
    /// @param body 天体
    ThirdBodyForce* addThirdBody(Body* body);

    /// @brief 添加三体引力模型
    ThirdBodyForce* addThirdBody(ThirdBodyForce& force);

    /// @brief 获取三体引力模型
    /// @param body 天体
    ThirdBodyForce* getThirdBody(Body* body);

    /// @brief 获取三体引力模型
    /// @param bodyName 天体名称
    ThirdBodyForce* getThirdBody(StringView bodyName);

    /// @brief 获取三体引力模型列表
    const ThirdBodyList& getThirdBodies() const{return thirdBodies_;}
private:
    bool                        useDrag_{false};                    ///< 是否使用阻力模型
    bool                        useSRP_{false};                     ///< 是否使用太阳辐射压模型
    bool                        useRelativityCorrection_{false};    ///< 是否使用相对论修正
    SharedPtr<Body>             centralBody_;                       ///< 中心天体
    DragForce                   drag_;                              ///< 大气阻力
    SolarRadiationPressure      srp_;                               ///< 太阳辐射压模型
    ThirdBodyList               thirdBodies_;                       ///< 三体引力 
    ClonePtr<BodyAttraction>    bodyAttraction_;                    ///< 中心天体引力模型(二体或者引力场)

    bool useMoonGravity_{false};
    double moonGravity_{kMoonGrav};
public:
    bool useMoonGravity() const{return useMoonGravity_;}
    void useMoonGravity(bool useMoonGravity){useMoonGravity_ = useMoonGravity;}
    double moonGravity() const{return moonGravity_;}
    void setMoonGravity(double moonGravity){moonGravity_ = moonGravity;}
};


/*! @} */

AST_NAMESPACE_END
