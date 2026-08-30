///
/// @file      HPOP.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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

#include "HPOP.hpp"
#include "HPOPEquation.hpp"
#include "AstMath/ODE.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/EventDetector.hpp"
#include "AstCore/StateMapper.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/BlockAstro.hpp"

AST_NAMESPACE_BEGIN


class HPOPStateMapper final: public StateMapper
{
public:
    HPOPStateMapper() = default;
    ~HPOPStateMapper() override = default;

    // ── 配置 ─────────────────────────────────────────────

    /// @brief 设置各分量在状态向量中的索引
    void setFrameAndIndices(
        Frame* frame,
        size_t idxPos, size_t idxVel, 
        size_t idxSTM=-1, size_t idxDragSens=-1, size_t idxSRPSens=-1
    )
    {
        assert(frame != nullptr);
        frame_       = frame;
        idxPos_      = idxPos;
        idxVel_      = idxVel;
        idxSTM_      = idxSTM;
        idxDragSens_ = idxDragSens;
        idxSRPSens_  = idxSRPSens;
    }

    // ── to* — 从状态向量 y 读取到类型化对象（解包） ─────

    /// @name 位置/速度基础访问
    /// @{
    void toPos(const double* y, double* pos) const
    {
        memcpy(pos, &y[idxPos_], 3 * sizeof(double));
    }
    void toVel(const double* y, double* vel) const
    {
        memcpy(vel, &y[idxVel_], 3 * sizeof(double));
    }
    /// @}

    void toState(const double* y, CartState& state) const override
    {
        toPos(y, state.position().data());
        toVel(y, state.velocity().data());
    }
    void toState(const double* y, double x, SpacecraftState& state) const override
    {
        TimePoint time;
        toTime(x, time);
        state.setStateEpoch(time);
        CartState cartState;
        toState(y, cartState);
        state.setState(cartState);
        state.setFrame(frame_);
    }

    errc_t toStateTransitionMatrix(const double* y, Matrix6d& stm) const override
    {
        if (idxSTM_ == (size_t)-1)
            return -1;
        memcpy(&stm, &y[idxSTM_], sizeof(Matrix6d));
        return 0;
    }
    errc_t toStateSensitivityWrtDrag(const double* y, Vector6d& sens) const override
    {
        if (idxDragSens_ == (size_t)-1)
            return -1;
        memcpy(sens.data(), &y[idxDragSens_], 6 * sizeof(double));
        return 0;
    }
    errc_t toStateSensitivityWrtSRP(const double* y, Vector6d& sens) const override
    {
        if (idxSRPSens_ == (size_t)-1)
            return -1;
        memcpy(sens.data(), &y[idxSRPSens_], 6 * sizeof(double));
        return 0;
    }

    // ── from* — 从类型化对象写入状态向量 y（打包） ─────

    /// @name 位置/速度基础访问
    /// @{
    void fromPos(const double* pos, double* y) const
    {
        memcpy(&y[idxPos_], pos, 3 * sizeof(double));
    }
    void fromVel(const double* vel, double* y) const
    {
        memcpy(&y[idxVel_], vel, 3 * sizeof(double));
    }
    /// @}

    void fromState(const CartState& state, double* y) const
    {
        fromPos(state.position().data(), y);
        fromVel(state.velocity().data(), y);
    }
    void fromState(const SpacecraftState& state, double* y, double& x) const override
    {
        TimePoint time;
        state.getStateEpoch(time);
        fromTime(time, x);
        CartState cartState;
        state.getState(cartState);
        fromState(cartState, y);
    }

    void fromStateTransitionMatrix(const Matrix6d& stm, double* y) const
    {
        memcpy(&y[idxSTM_], &stm, sizeof(Matrix6d));
    }
    void fromStateSensitivityWrtDrag(const Vector6d& sens, double* y) const
    {
        memcpy(&y[idxDragSens_], sens.data(), 6 * sizeof(double));
    }
    void fromStateSensitivityWrtSRP(const Vector6d& sens, double* y) const
    {
        memcpy(&y[idxSRPSens_], sens.data(), 6 * sizeof(double));
    }

private:
    size_t idxPos_      = 0;           ///< Pos 在状态向量中的起始索引，默认 0（基本 6-DOF 情况）
    size_t idxVel_      = 3;           ///< Vel 在状态向量中的起始索引，默认 3（基本 6-DOF 情况）
    size_t idxSTM_      = (size_t)-1;  ///< STM 在状态向量中的起始索引
    size_t idxDragSens_ = (size_t)-1;  ///< Drag 敏感度在状态向量中的起始索引
    size_t idxSRPSens_  = (size_t)-1;  ///< SRP 敏感度在状态向量中的起始索引
    Frame* frame_ = nullptr;           ///< 参考坐标系
};

HPOP::HPOP()
    : equation_(), integrator_(), stateMapper_(new HPOPStateMapper())
{

}

HPOP::~HPOP() = default;

errc_t HPOP::setForceModel(HPOPForceModel&& forcemodel)
{
    return equation()->setForceModel(std::move(forcemodel));
}

errc_t HPOP::setForceModel(const HPOPForceModel& forcemodel)
{
    return equation()->setForceModel(forcemodel);
}

HPOPForceModel& HPOP::forceModel()
{
    return equation()->forceModel();
}

void HPOP::setSpacecraftParam(const SpacecraftParam& spacecraftParam)
{
    equation()->setSpacecraftParam(spacecraftParam);
}

const SpacecraftParam& HPOP::spacecraftParam() const
{
    return equation()->spacecraftParam();
}

Frame* HPOP::propagationFrame() const
{
    return equation_->getPropagationFrame();
}

errc_t HPOP::setPropagationFrame(Frame *frame)
{
    return equation_->setPropagationFrame(frame);
}

void HPOP::setIntegrator(ODEIntegrator *integrator)
{
    if(integrator)
        integrator_ = integrator;
}

ODEIntegrator *HPOP::getIntegrator() const
{
    if(A_UNLIKELY(!integrator_)){
        // const_cast<HPOP*>
        (this)->integrator_ = new RKF78();
    }
    return integrator_;
}

errc_t HPOP::propagate(const TimePoint &startTime, TimePoint &targetTime, Vector3d &position, Vector3d &velocity)
{
    errc_t err = this->initialize();
    if (err)
        return err;
    int dim = equation_->getDimension();
    if (dim != 6){
        aError("dimension of equation is not 6");
        return -1;
    }

    const auto& dynSys = equation_->dynamicSystem();
    const size_t idxPos = dynSys.getStateIndex(kIdentifierPos);
    const size_t idxVel = dynSys.getStateIndex(kIdentifierVel);
    if (idxPos == (size_t)-1 || idxVel == (size_t)-1) {
        aError("failed to find Pos or Vel in state vector");
        return -1;
    }

    stateMapper_->setFrameAndIndices(propagationFrame(), idxPos, idxVel);

    array6d y;
    stateMapper_->fromPos(position.data(), y.data());
    stateMapper_->fromVel(velocity.data(), y.data());

    err = integrateState(startTime, targetTime, y.data());
    if (err) return err;

    stateMapper_->toPos(y.data(), position.data());
    stateMapper_->toVel(y.data(), velocity.data());
    return err;
}

errc_t HPOP::propagate(const TimePoint &startTime, TimePoint &targetTime, CartState &state, Matrix6d &stm)
{
    errc_t err = this->initialize();
    if (err)
        return err;
    const int dim = equation_->getDimension();
    constexpr int dimexpected = 6 + 36;
    if (dim != dimexpected){
        aError("dimension of equation is not 42");
        return -1;
    }

    const auto& dynSys = equation_->dynamicSystem();
    const size_t idxPos = dynSys.getStateIndex(kIdentifierPos);
    const size_t idxVel = dynSys.getStateIndex(kIdentifierVel);
    const size_t idxSTM = dynSys.getStateIndex(kIdentifierSTM);
    if (idxPos == (size_t)-1 || idxVel == (size_t)-1 || idxSTM == (size_t)-1) {
        aError("failed to find Pos, Vel or STM in state vector");
        return -1;
    }

    stateMapper_->setFrameAndIndices(propagationFrame(), idxPos, idxVel, idxSTM);

    std::array<double, dimexpected> y;
    stateMapper_->fromState(state, y.data());
    stateMapper_->fromStateTransitionMatrix(stm, y.data());

    err = integrateState(startTime, targetTime, y.data());
    if (err) return err;

    stateMapper_->toState(y.data(), state);
    stateMapper_->toStateTransitionMatrix(y.data(), stm);
    return err;
}

errc_t HPOP::propagate(const TimePoint& startTime, TimePoint& targetTime,
                       CartState& state, Matrix6d& stm,
                       Vector6d& stateSensWrtDrag, Vector6d& stateSensWrtSRP)
{
    errc_t err = this->initialize();
    if (err)
        return err;
    const int dim = equation_->getDimension();

    const auto& dynSys = equation_->dynamicSystem();

    // 通过 getStateIndex 查找各分量在状态向量中的索引，不假定排列顺序
    const size_t idxPos = dynSys.getStateIndex(kIdentifierPos);
    const size_t idxVel = dynSys.getStateIndex(kIdentifierVel);
    const size_t idxSTM = dynSys.getStateIndex(kIdentifierSTM);
    if (idxPos == (size_t)-1 || idxVel == (size_t)-1 || idxSTM == (size_t)-1) {
        aError("failed to find Pos, Vel or STM in state vector");
        return -1;
    }

    const bool hasB = forceModel().useDragSensitivity();
    const bool hasK = forceModel().useSRPSensitivity();
    const int expectedDim = 42 + (hasB ? 6 : 0) + (hasK ? 6 : 0);
    if (dim != expectedDim) {
        aError("dimension mismatch: expected %d, got %d", expectedDim, dim);
        return -1;
    }

    size_t idxDragSens = (size_t)-1;
    size_t idxSRPSens = (size_t)-1;
    if (hasB) {
        idxDragSens = dynSys.getStateIndex(kIdentifierStateSensitivityWrtDrag);
        if (idxDragSens == (size_t)-1) {
            aError("failed to find StateSensitivityWrtDrag in state vector");
            return -1;
        }
    }
    if (hasK) {
        idxSRPSens = dynSys.getStateIndex(kIdentifierStateSensitivityWrtSRP);
        if (idxSRPSens == (size_t)-1) {
            aError("failed to find StateSensitivityWrtSRP in state vector");
            return -1;
        }
    }

    stateMapper_->setFrameAndIndices(propagationFrame(), idxPos, idxVel, idxSTM, idxDragSens, idxSRPSens);

    std::vector<double> y(dim);
    stateMapper_->fromState(state, y.data());
    stateMapper_->fromStateTransitionMatrix(stm, y.data());
    if (hasB) {
        stateMapper_->fromStateSensitivityWrtDrag(stateSensWrtDrag, y.data());
    }
    if (hasK) {
        stateMapper_->fromStateSensitivityWrtSRP(stateSensWrtSRP, y.data());
    }

    err = integrateState(startTime, targetTime, y.data());
    if (err) return err;

    stateMapper_->toState(y.data(), state);
    stateMapper_->toStateTransitionMatrix(y.data(), stm);
    if (hasB) {
        stateMapper_->toStateSensitivityWrtDrag(y.data(), stateSensWrtDrag);
    }
    if (hasK) {
        stateMapper_->toStateSensitivityWrtSRP(y.data(), stateSensWrtSRP);
    }

    return err;
}

errc_t HPOP::integrateState(const TimePoint& startTime, TimePoint& targetTime, double* y)
{
    equation_->setEpoch(startTime);
    stateMapper_->setEpoch(startTime);
    double duration = targetTime - startTime;
    double t = 0;
    errc_t err = integrator_->integrate(*equation_, y, t, duration);
    if (t != duration && !err) {
        targetTime = startTime + t;
    }
    return err;
}

errc_t HPOP::initialize()
{
    if (!equation_){
        equation_ = new HPOPEquation();
    }
    if (!integrator_){
        integrator_ = new RKF78();
    }
    return equation_->initialize();
    // err |= integrator_->initialize(equation_);
}

StateMapper* HPOP::stateMapper() const
{
    return stateMapper_.get();
}

void HPOP::addEventDetector(EventDetector* eventDetector)
{
    if(!eventDetector) return;
    this->getIntegrator()->addEventDetector(eventDetector->newODEEventDetector(stateMapper_.get()));
}

void HPOP::clearEventDetectors()
{
    this->getIntegrator()->clearEventDetectors();
}


HPOPEquation* HPOP::equation() const
{
    if(!equation_){
        equation_ = new HPOPEquation();
    }
    return equation_;
}

AST_NAMESPACE_END

