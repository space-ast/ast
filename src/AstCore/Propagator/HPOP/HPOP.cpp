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
    void toState(const double* y, double x, SpacecraftState& state) const override
    {
        // 设置轨道历元
        TimePoint time;
        toTime(x, time);
        state.setStateEpoch(time);

        // 设置轨道状态
        // @todo 这里需要处理其他一般情况
        CartState* cartState = (CartState*)y;
        state.setState(*cartState);
    }
    void fromState(const SpacecraftState& state, double* y, double& x) const override
    {
        // 获取轨道历元
        TimePoint time;
        state.getStateEpoch(time);
        fromTime(time, x);

        // 获取轨道状态
        // @todo 这里需要处理其他一般情况
        CartState* cartState = (CartState*)y;
        state.getState(*cartState);
    }
    void toState(const double* y, double x, CartState& state) const override
    {
        // 设置轨道状态
        // @todo 这里需要处理其他一般情况
        CartState* cartState = (CartState*)y;
        state = *cartState;
    }
    errc_t toStateTransitionMatrix(const double* y, double x, Matrix6d& stm) const override
    {
        // @todo 这里需要处理其他一般情况
        return 0;
    }
    errc_t toStateSensitivityWrtDrag(const double* y, double x, Vector6d& stateSensWrtDrag) const override
    {
        // @todo 这里需要处理其他一般情况
        return 0;
    }
    errc_t toStateSensitivityWrtSRP(const double* y, double x, Vector6d& stateSensWrtSRP) const override
    {
        // @todo 这里需要处理其他一般情况
        return 0;
    }
};

HPOP::HPOP()
    : stateMapper_(new HPOPStateMapper())
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

    array6d y;
    memcpy(&y[idxPos], position.data(), 3 * sizeof(double));
    memcpy(&y[idxVel], velocity.data(), 3 * sizeof(double));

    err = integrateState(startTime, targetTime, y.data());
    if (err) return err;

    memcpy(position.data(), &y[idxPos], 3 * sizeof(double));
    memcpy(velocity.data(), &y[idxVel], 3 * sizeof(double));
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

    std::array<double, dimexpected> y;
    memcpy(&y[idxPos], state.position().data(), 3 * sizeof(double));
    memcpy(&y[idxVel], state.velocity().data(), 3 * sizeof(double));
    memcpy(&y[idxSTM], &stm, sizeof(Matrix6d));

    err = integrateState(startTime, targetTime, y.data());
    if (err) return err;

    memcpy(state.position().data(), &y[idxPos], 3 * sizeof(double));
    memcpy(state.velocity().data(), &y[idxVel], 3 * sizeof(double));
    memcpy(&stm, &y[idxSTM], sizeof(Matrix6d));
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

    // 打包状态向量
    std::vector<double> y(dim);
    memcpy(&y[idxPos], state.position().data(), 3 * sizeof(double));
    memcpy(&y[idxVel], state.velocity().data(), 3 * sizeof(double));
    memcpy(&y[idxSTM], &stm, sizeof(Matrix6d));
    if (hasB) {
        memcpy(&y[idxDragSens], stateSensWrtDrag.data(), 6 * sizeof(double));
    }
    if (hasK) {
        memcpy(&y[idxSRPSens], stateSensWrtSRP.data(), 6 * sizeof(double));
    }

    err = integrateState(startTime, targetTime, y.data());
    if (err) return err;

    // 解包状态向量
    memcpy(state.position().data(), &y[idxPos], 3 * sizeof(double));
    memcpy(state.velocity().data(), &y[idxVel], 3 * sizeof(double));
    memcpy(&stm, &y[idxSTM], sizeof(Matrix6d));
    if (hasB) {
        memcpy(stateSensWrtDrag.data(), &y[idxDragSens], 6 * sizeof(double));
    }
    if (hasK) {
        memcpy(stateSensWrtSRP.data(), &y[idxSRPSens], 6 * sizeof(double));
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

