///
/// @file      HPOPEquation.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-19
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

#include "HPOPEquation.hpp"
#include "HPOP.hpp"                 // for HPOPForceModel
#include "AstUtil/Logger.hpp"
#include "AstCore/Simulation.hpp"   // for blocks
#include "AstCore/BuiltinFrame.hpp"

AST_NAMESPACE_BEGIN

HPOPEquation::HPOPEquation()
    : propFrame_{}
{

}

HPOPEquation::HPOPEquation(HPOPForceModel&& forceModel)
    : HPOPEquation{}
{
    this->setForceModel(std::move(forceModel));
}



HPOPEquation::~HPOPEquation()
{

}

errc_t HPOPEquation::evaluate(const double* y, double* dy, const double t)
{
    SimTime time;                                   // 仿真时间
    time.setTimePoint(epoch_ + t);                  // 设置仿真时间点
    time.setElapsedTime(t);                         // 设置仿真的相对时间
    dynamicSystem_.fillDerivativeData(0.0);         // 填充导数数据为0
    dynamicSystem_.setStateData(y);                 // 设置状态数据
    errc_t err = dynamicSystem_.run(time);           // 执行动力学系统
    dynamicSystem_.getDerivativeData(dy);           // 获取导数数据
    return err;                                     // 返回错误码
}

int HPOPEquation::getDimension() const
{
    // @fixme
    return dynamicSystem_.getDimension();
}

/// @brief 初始化仿真引擎
errc_t HPOPEquation::initialize()
{
    return this->initializeFromForceModel(this->forceModel_);
}

errc_t HPOPEquation::initBlocks(const HPOPForceModel &forceModel)
{
    if(!this->propFrame_)
    {
        if(auto cb = aGetEarth())
        {
            this->propFrame_ = cb->makeFrameInertial();
            aWarning("propagation frame is not set, using earth inertial frame as the default propagation frame.");
        }
        if(!this->propFrame_)
            aError("failed to set propagation frame to earth inertial frame.");
    }
    // 将力模型配置转换为动力学系统的一个个函数块
    BlockDerivative* derivativeBlock;
    auto& bodyAttraction = forceModel.bodyAttraction();
    auto propFrame = this->propFrame_; 
    auto body = propFrame->getBody();   // 尝试获取预报坐标系原点对应的天体

    // 重置动力学系统
    this->reset();

    // 添加运动学函数块
    derivativeBlock = new BlockMotion();
    this->addBlock(derivativeBlock);

    if(body){
        // 添加重力场函数块
        if(auto gravityPtr = bodyAttraction.asGravityForce()){
            auto& gravity = *gravityPtr;
            if(0 == gravity.maxDegree_){
                GravityFieldHead gfHead;
                errc_t err = gfHead.load(gravity.model_, body->getDirpath());
                if(err != eNoError){
                    aError("Failed to load gravity field head from file: '%s'", gravity.model_.c_str());
                    return err;
                }
                derivativeBlock = new BlockTwoBody(gfHead.getGM());
                this->addBlock(derivativeBlock);
            }else{
                GravityField gravityField;
                errc_t err = gravityField.load(gravity.model_, gravity.maxDegree_, gravity.maxOrder_, body->getDirpath());
                if(err != eNoError){
                    aError("Failed to load gravity field from file: '%s'", gravity.model_.c_str());
                    return err;
                }
                auto propAxes = propFrame->getAxes();
                /// @todo 这里要根据重力场的配置来获取重力场坐标系
                auto gravityAxes = body->getAxesFixed(); 
                /// @todo 这里产生了一次重力场系数复制，有一定的优化空间
                derivativeBlock = new BlockGravity(gravityField, gravity.maxDegree_, gravity.maxOrder_, gravityAxes, propAxes);
                this->addBlock(derivativeBlock);
            }
        }
        // 添加二体引力函数块
        else if(auto pointMassPtr = bodyAttraction.asPointMassForce())
        {
            auto& pointMass = *pointMassPtr;
            double gm{};
            if(pointMass.gmSource_ == EGMSource::eSpecifiedValue)
            {
                gm = pointMass.specifiedGM_;
            }
            else if(pointMass.gmSource_ == EGMSource::eBodyGravity)
            {
                gm = body->getGM();
            }
            else if(pointMass.gmSource_ == EGMSource::eJplDE)
            {
                aWarning("unsupported feature: JPL DE gravity gm source, use body gm instead.");
                gm = body->getGM();
            }else{
                aError("unsupported gm source type: %d, use body gm instead.", pointMass.gmSource_);
                gm = body->getGM();
            }
            this->addBlock(new BlockTwoBody(gm));
        }
        else
        {
            // todo: 处理其他引力模型
            aWarning("the body attraction model is not a gravity force model, no central body gravity force will be added.");
        }
    }else{
        aWarning("the propagation frame's center is not a celestial body, no gravity force will be added.");
    }

    // 添加月球引力函数块
    if(forceModel.useMoonGravity())
    {
        derivativeBlock = new BlockThirdBody(forceModel.moonGravity());
        this->addBlock(derivativeBlock);
    }
    return eNoError;
}

void HPOPEquation::addBlock(FuncBlock *block)
{
    dynamicSystem_.addBlock(block);
}

void HPOPEquation::addBlock(BlockDerivative *block)
{
    dynamicSystem_.addBlock(block);
}

void HPOPEquation::clearBlocks()
{
    dynamicSystem_.clearBlocks();
}

void HPOPEquation::reset()
{
    dynamicSystem_.reset();
}



errc_t HPOPEquation::initializeFromForceModel(const HPOPForceModel &forceModel)
{
    errc_t rc = this->initBlocks(forceModel);
    if(rc) return rc;
    return dynamicSystem_.initialize();
}

errc_t HPOPEquation::setForceModel(HPOPForceModel&& forceModel)
{
    this->forceModel_ = std::move(forceModel);
    return eNoError;
}

errc_t HPOPEquation::setForceModel(const HPOPForceModel &forceModel)
{
    this->forceModel_ = forceModel;
    return eNoError;
}

errc_t HPOPEquation::setPropagationFrame(Frame *frame)
{
    if(!frame) return -1;
    /// @todo 这里还需要检查frame是否是准惯性系
    // if(!frame->isPseudoInertial()) return -1;
    propFrame_ = frame;
    return eNoError;
}

AST_NAMESPACE_END
