///
/// @file      StateBPlane.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-13
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
/// 您使用本软件所产生的风险，需由您自行承担。

#pragma once

#include "AstGlobal.h"
#include "AstCore/State.hpp"
#include "AstMath/Vector.hpp"
#include "AstCore/OrbitElement.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/
class StateBPlane;
using PStateBPlane = StateBPlane*;
using HStateBPlane = SharedPtr<StateBPlane>;

/// @brief B平面状态
/// @details
/// B平面状态使用入渐近线方向和B矢量描述航天器相对中心天体的状态,
/// 主要用于双曲线到达轨迹(月球或行星借力飞越)。
/// 具体的转换算法和退化处理参见 @ref aCartToBPlane 和 @ref aBPlaneToCart 。
/// @note  该状态需要参考坐标系(提供中心天体与引力常数)和状态历元:
///        默认参考向量(B平面R、T轴的定义依据)为中心天体绕其父天体的轨道法向,
///        需要星历数据支持, 可通过 @ref setRefVector 修改。
class AST_CORE_API StateBPlane final: public State
{
public:
    AST_OBJECT(StateBPlane)
    AST_PROPERT(RA)
    AST_PROPERT(Dec)
    AST_PROPERT(BDotR)
    AST_PROPERT(BDotT)
    AST_PROPERT(C3)
    AST_PROPERT(TrueAnomaly)
    static PStateBPlane New();
    static HStateBPlane MakeShared();
    static PStateBPlane New(const BPlaneElem& bPlaneElem);
    static HStateBPlane MakeShared(const BPlaneElem& bPlaneElem);

    StateBPlane() = default;
    StateBPlane(const BPlaneElem& bPlaneElem);
    StateBPlane(const State& state);
    ~StateBPlane() override = default;
public:
    EStateType getStateType() const override { return EStateType::eBPlane; }
    errc_t getState(CartState& state) const override;
    errc_t getState(ModOrbElem& orbElem) const override;
    errc_t setState(const CartState& state) override;
    errc_t setState(const ModOrbElem& orbElem) override;
public:
    /// @brief 获取B平面根数
    /// @param bPlaneElem 输出B平面根数
    void getState(BPlaneElem& bPlaneElem) const { bPlaneElem = bPlaneElem_; }

    /// @brief 设置B平面根数
    /// @param bPlaneElem B平面根数
    /// @return 错误码，成功返回eNoError
    errc_t setState(const BPlaneElem& bPlaneElem);

    /// @brief 获取B平面参考向量(单位矢量)
    /// @details 参考向量用于确定B平面的R、T轴, 参见 @ref EBPlaneRefVector
    /// @param refVector 输出参考向量
    /// @param cartState 当前笛卡尔状态, 仅当参考向量类型为eSpacecraftOrbitNormal时使用;
    ///        该类型下B平面参数与参考向量相互依赖(B·R恒为0), 因此由B平面参数
    ///        反算直角坐标时无法确定参考向量, 此时本函数返回eErrorInvalidParam
    /// @return 错误码，成功返回eNoError
    errc_t getRefVector(Vector3d& refVector, const CartState* cartState = nullptr) const;

    /// @brief 设置B平面参考向量
    /// @param refVectorType 参考向量类型
    /// @param refVector 自定义参考向量, 仅当refVectorType为eCustom时使用
    void setRefVector(EBPlaneRefVector refVectorType, const Vector3d& refVector = Vector3d::Zero());

    /// @brief 获取参考向量类型
    /// @return EBPlaneRefVector 参考向量类型
    EBPlaneRefVector getRefVectorType() const { return refVectorType_; }
protected:
    /// @brief 获取笛卡尔状态(用于由B平面参数反算)
    /// @param state 输出笛卡尔状态
    /// @return 错误码，成功返回eNoError
    errc_t getCartesianState(CartState& state) const;
PROPERTIES:
    /// @brief 获取入渐近线赤经
    angle_d getRA() const { return bPlaneElem_.ra_; }
    /// @brief 设置入渐近线赤经
    void setRA(angle_d ra) { bPlaneElem_.ra_ = ra; }

    /// @brief 获取入渐近线赤纬
    angle_d getDec() const { return bPlaneElem_.dec_; }
    /// @brief 设置入渐近线赤纬
    void setDec(angle_d dec) { bPlaneElem_.dec_ = dec; }

    /// @brief 获取B矢量在R轴上的分量
    length_d getBDotR() const { return bPlaneElem_.bDotR_; }
    /// @brief 设置B矢量在R轴上的分量
    void setBDotR(length_d bDotR) { bPlaneElem_.bDotR_ = bDotR; }

    /// @brief 获取B矢量在T轴上的分量
    length_d getBDotT() const { return bPlaneElem_.bDotT_; }
    /// @brief 设置B矢量在T轴上的分量
    void setBDotT(length_d bDotT) { bPlaneElem_.bDotT_ = bDotT; }

    /// @brief 获取特征能量
    double getC3() const { return bPlaneElem_.c3_; }
    /// @brief 设置特征能量
    void setC3(double c3) { bPlaneElem_.c3_ = c3; }

    /// @brief 获取真近点角
    angle_d getTrueAnomaly() const { return bPlaneElem_.trueA_; }
    /// @brief 设置真近点角
    void setTrueAnomaly(angle_d trueA) { bPlaneElem_.trueA_ = trueA; }
protected:
    BPlaneElem          bPlaneElem_{};                                          ///< B平面根数
    EBPlaneRefVector    refVectorType_{EBPlaneRefVector::eBodyOrbitNormal};     ///< 参考向量类型
    Vector3d            refVector_{};                                           ///< 自定义参考向量
};



/*! @} */

AST_NAMESPACE_END
