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
#include "HPOP.hpp"                         // for HPOPForceModel
#include "AstUtil/Logger.hpp"
#include "AstUtil/Math.hpp"
#include "AstCore/Simulation.hpp"           // for blocks
#include "AstCore/BuiltinFrame.hpp"         //
#include "AstCore/RunTimeSolarSystem.hpp"   // for aGetSun()

#include "AstCore/NRLMSIS00.hpp"
#include "AstCore/MSISE90.hpp"
#include "AstCore/MSIS86.hpp"
#include "AstCore/USSA1976.hpp"
#include "AstCore/JacchiaRoberts.hpp"

#include "AstCore/NoneEclipseCalculator.hpp"
#include "AstCore/ConeEclipseCalculator.hpp"
#include "AstCore/CylindricalEclipseCalculator.hpp"

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
    // @bug setStateData 必须要求状态量和导数量的排列和维度与输入数据的一致
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
    return this->initializeFromForceModel(this->forceModel_, this->spacecraftParam_);
}

/// @brief 获取重力场坐标系
/// @param gravityField 重力场系数
/// @param body 中心天体
/// @return 重力场坐标系
static Axes* aGetGravityAxes(const GravityField& gravityField, const Body& body)
{
    // 如果指定了重力场参考系，使用该参考系
    if(!gravityField.referenceFrame().empty())
    {
        Axes* axes = body.getAxes(gravityField.referenceFrame());
        if(axes != nullptr)
            return axes;
        aWarning("gravity field reference frame '%s' not found in body '%s'.", gravityField.referenceFrame().c_str(), body.getName().c_str());
    }
    // 如果是月球，使用月球的惯性主轴PrincipalAxes
    // 月球的固连系一般为MeanEarth系，与PrincipalAxes存在偏置
    if(body.isLuna())
    {
        auto axes = body.getAxes("PrincipalAxes");
        if(axes != nullptr)
            return axes;
        aWarning("PrincipalAxes not found in body '%s'.", body.getName().c_str());
    }
    return body.getAxesFixed();
}

static Point* aGetBodyEphemeris(const Body& body, EEphemerisSource ephemerisSource)
{
    return body.getEphemeris(ephemerisSource);
}

static double clamp_f10p7(double value)
{
    if(value < 40.0)
    {
        aWarning("f10p7 is less than 40, using 40 as default.");
        return 40.0;
    }
    else if(value > 10000.0)
    {
        aWarning("f10p7 is greater than 10000, using 10000 as upper bound.");
        return 10000.0;
    }
    return value;
}

static Atmosphere* aNewAtmosphere(const DragForce& drag)
{
    AtmosphereBase* atmosphere = nullptr;

    // 目前的大气模型都是地球的大气模型
    auto earth = aGetEarth();
    if(!earth)
        return nullptr;
    
    BodyShape* shape = earth->getShape();
    Frame* frame = earth->getFrameFixed();

    double f10p7Daily   = clamp_f10p7(drag.f10p7Daily_);
    double f10p7Average = clamp_f10p7(drag.f10p7Average_);

    


    auto atmDensityModel = drag.atmDensityModel_;
    if(atmDensityModel == EAtmDensityModel::eMSIS1986)
    {
        atmosphere = new MSIS86(frame, shape, f10p7Daily, f10p7Average, drag.ap());
    }
    else if(atmDensityModel == EAtmDensityModel::eMSISE1990)
    {
        atmosphere = new MSISE90(frame, shape, f10p7Daily, f10p7Average, drag.ap());
    }
    else if(atmDensityModel == EAtmDensityModel::eNRLMSISE2000)
    {
        atmosphere = new NRLMSIS00(frame, shape, f10p7Daily, f10p7Average, drag.ap());
    }
    else if(atmDensityModel == EAtmDensityModel::e1976Standard)
    {
        atmosphere = new USSA1976(frame, shape);
    }
    else if(atmDensityModel == EAtmDensityModel::eJacchiaRoberts)
    {
        auto jr = new JacchiaRoberts(frame, shape, aGetSun(), f10p7Daily, f10p7Average, drag.kp_);
        jr->setSunPosition(drag.sunPosition_);
        atmosphere = jr;
    }
    else
    {
        aWarning("atmosphere '%d' is not supported, using default model 'NRLMSIS00'.", atmDensityModel);
        atmosphere = new NRLMSIS00(frame, shape, f10p7Daily, f10p7Average, drag.ap());
    }
    atmosphere->setUseApproximateAltitude(drag.useApproxAltForDrag_);
    return atmosphere;
}

errc_t HPOPEquation::initBlocks(const HPOPForceModel &forceModel, const SpacecraftParam &spacecraftParam)
{
    // 检查中心天体
    auto body = this->getCentralBody();
    if(!body)
    {
        body = aGetEarth();
        assert(body);
        aWarning("central body is not set, using 'Earth' as default.");
        if(!body)
            return eErrorNullInput;
    }

    // 检查预报坐标系是否有效
    auto propFrame = this->getPropagationFrame(); 
    if(!propFrame)
    {
        propFrame = body->getFrameInertial();
        assert(propFrame);
        aWarning("propagation frame is not set, using the inertial frame of '%s' as the default propagation frame.", body->getName().c_str());
        if(!propFrame)
            return eErrorNullInput;
    }
    else
    {
        if(propFrame->getOrigin() != body)
        {
            aError("propagation frame '%s' origin is not central body '%s'.", propFrame->getName().c_str(), body->getName().c_str());
            return eErrorInvalidParam;
        }
    }
    // 将力模型配置转换为动力学系统的一个个函数块
    BlockDerivative* derivativeBlock;
    auto& bodyAttraction = forceModel.bodyAttraction();

    // 重置动力学系统
    this->reset();

    // 添加运动学函数块
    // 先添加运动学函数块，再添加其他块，确保Pos状态在状态量列表的开头
    derivativeBlock = new BlockMotion();
    this->addBlock(derivativeBlock);

    
    if(body && forceModel.useCentralBodyAttraction()){
        // 添加重力场函数块
        if(auto gravityPtr = bodyAttraction.asGravityForce()){
            auto& gravity = *gravityPtr;
            // 如果重力场的阶数为0，说明是二体引力，直接添加二体引力函数块即可
            if(0 == gravity.maxDegree_){
                double gm;
                errc_t err = aGetGravityParameter(*body, gravity.model_, gm);
                if(err != eNoError)
                    return err;
                derivativeBlock = new BlockTwoBody(gm);
                this->addBlock(derivativeBlock);
            }else{
                GravityField gravityField;
                errc_t err = gravityField.load(gravity.model_, gravity.maxDegree_, gravity.maxOrder_, body->getDirpath());
                if(err != eNoError){
                    aError("Failed to load gravity field from file: '%s'", gravity.model_.c_str());
                    return err;
                }
                auto propAxes = propFrame->getAxes();
                auto gravityAxes = aGetGravityAxes(gravityField, *body); 
                /// @todo 这里产生了一次重力场系数复制，有一定的优化空间
                BlockGravity* blockGravity = new BlockGravity(gravityField, gravity.maxDegree_, gravity.maxOrder_, gravityAxes, propAxes);
                // 设置是否考虑重力场系数变化
                blockGravity->setConsiderVariations(gravity.useSecularVariations_);
                this->addBlock(blockGravity);
            }
        }
        // 添加二体引力函数块
        else if(auto pointMassPtr = bodyAttraction.asPointMassForce())
        {
            double gm = pointMassPtr->getGM(body);
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




    BlockMass* blockMass = nullptr;  // 质量函数块指针，用于标识是否添加了质量函数块

    // 添加大气阻力函数块
    if(forceModel.useDrag())
    {
        // 添加质量函数块
        blockMass = new BlockMass(spacecraftParam.mass());
        this->addBlock(blockMass);
        
        Atmosphere* atmosphere = aNewAtmosphere(forceModel.drag());
        // atmosphere 的所有权转移给 blockDrag
        derivativeBlock = new BlockDrag(atmosphere, spacecraftParam.cd(), spacecraftParam.dragArea(), propFrame);
        this->addBlock(derivativeBlock);
    }

    // 添加太阳辐射压力函数块
    if(forceModel.useSRP())
    {
        // 如果前面没有添加质量函数块（如未使用阻力模型），则需要在此添加
        auto sun = aGetSun();
        if(sun)
        {
            if(!blockMass)
            {
                blockMass = new BlockMass(spacecraftParam.mass());
                this->addBlock(blockMass);
            }
            auto shadowModel = forceModel.srp().shadowModel_;
            EclipseCalculator* eclipseCalculator;
            if(shadowModel == EShadowModel::eDualCone)
            {
                eclipseCalculator = new ConeEclipseCalculator();
            }
            else if(shadowModel == EShadowModel::eCylindrical)
            {
                eclipseCalculator = new CylindricalEclipseCalculator();
            }
            else //if(shadowModel == EShadowModel::eNone)
            {
                eclipseCalculator = new NoneEclipseCalculator();
            }

            eclipseCalculator->setLightSource(sun);     // 设置光源体
            if(forceModel.srp().eclipsingBodies_.empty())
            {
                if(body != sun && body != nullptr)
                {
                    eclipseCalculator->addOccultingBody(body);
                }
            }
            else
            {
                eclipseCalculator->setOccultingBodies(forceModel.srp().eclipsingBodies_); // 设置遮挡体列表
            }
            // eclipseCalculator 的所有权转移给 blockSRP
            BlockSRP* blockSRP = new BlockSRP(eclipseCalculator, spacecraftParam.cr(), spacecraftParam.srpArea(), propFrame);
            blockSRP->setSunPosition(forceModel.srp().sunPosition_); // 设置太阳位置
            this->addBlock(blockSRP);
        }
        else
        {
            aWarning("there is no sun in the system, no solar radiation pressure force will be added.");
        }
    }

    // 添加三体引力函数块
    {
        auto& thirdBodies = forceModel.getThirdBodies();
        for(auto& thirdBody: thirdBodies)
        {
            auto body3rd = thirdBody.body();
            if(body3rd == nullptr)
            {
                aWarning("third body pointer is null, skipping this third body.");
                continue;
            }
            Point* bodyEphemeris = aGetBodyEphemeris(*body3rd, thirdBody.ephemerisSource());
            if(thirdBody.bodyAttractionType() == EBodyAttractionType::eGravity)
            {
                // 三体使用球谐重力场
                auto& gravity = thirdBody.gravity();
                // @todo 如果 0 == gravity.maxDegree_，可以使用点质量引力简化计算逻辑
                GravityField gravityField;
                errc_t err = gravityField.load(gravity.model_, gravity.maxDegree_, gravity.maxOrder_, body3rd->getDirpath());
                if(err != eNoError)
                {
                    aError("Failed to load third body gravity field from file: '%s'", gravity.model_.c_str());
                    return err;
                }
                auto gravityAxes = aGetGravityAxes(gravityField, *body3rd);
                auto* block = new BlockThirdBodyGravity(bodyEphemeris, std::move(gravityField),
                                                        gravity.maxDegree_, gravity.maxOrder_,
                                                        gravityAxes, propFrame);
                block->setConsiderVariations(gravity.useSecularVariations_);
                this->addBlock(block);
            }
            else
            {
                // 三体使用点质量引力（默认行为）
                double gm = thirdBody.pointMass().getGM(body3rd);
                derivativeBlock = new BlockThirdBodyPointMass(bodyEphemeris, gm, propFrame);
                this->addBlock(derivativeBlock);
            }
        }
    }

    // [月球特定的逻辑，待废弃]
    // 添加月球引力函数块
    if(forceModel.useMoonGravity())
    {
        derivativeBlock = new BlockThirdBodyPointMass(aGetMoon(), forceModel.moonGravity(), propFrame);
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



errc_t HPOPEquation::initializeFromForceModel(const HPOPForceModel &forceModel, const SpacecraftParam &spacecraftParam)
{
    errc_t rc = this->initBlocks(forceModel, spacecraftParam);
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

void HPOPEquation::setSpacecraftParam(const SpacecraftParam &spacecraftParam)
{
    this->spacecraftParam_ = spacecraftParam;
}

errc_t HPOPEquation::setPropagationFrame(Frame *frame)
{
    if(!frame) return -1;
    /// @todo 这里还需要检查frame是否是准惯性系
    /// @todo 这里要注意处理propFrame_和centralBody_不一致的情况
    // if(!frame->isPseudoInertial()) return -1;
    propFrame_ = frame;
    return eNoError;
}

Frame* HPOPEquation::getPropagationFrame() const
{
    if(propFrame_)
        return propFrame_;
    else if(auto body = getCentralBody()){
        return body->getFrameInertial();
    }
    return nullptr;
}


Body* HPOPEquation::getCentralBody() const
{
    return forceModel().centralBody();
}



AST_NAMESPACE_END
