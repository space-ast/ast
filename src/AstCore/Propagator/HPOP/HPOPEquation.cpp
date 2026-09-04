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
#include "AstUtil/ScopedPtr.hpp"

#include "AstCore/Simulation.hpp"           // for blocks
#include "AstCore/BuiltinFrame.hpp"         //
#include "AstCore/RunTimeSolarSystem.hpp"   // for aGetSun()
#include "AstCore/SpaceWeather.hpp"
#include "AstCore/ConstantSpaceWeather.hpp"

#include "AstCore/NRLMSIS00.hpp"
#include "AstCore/MSISE90.hpp"
#include "AstCore/MSIS86.hpp"
#include "AstCore/USSA1976.hpp"
#include "AstCore/JacchiaRoberts.hpp"
#include "AstCore/HarrisPriester.hpp"
#include "AstCore/DTM2012.hpp"

#include "AstUtil/Constants.hpp"

#include "AstCore/NoneEclipseCalculator.hpp"
#include "AstCore/ConeEclipseCalculator.hpp"
#include "AstCore/CylindricalEclipseCalculator.hpp"
#include "AstCore/AstroBlockFactory.hpp"

AST_NAMESPACE_BEGIN

HPOPEquation::HPOPEquation()
    : dynamicSystem_()
{

}

HPOPEquation::~HPOPEquation()
{

}

errc_t HPOPEquation::evaluate(const double* y, double* dy, const double t)
{
    SimTime time;                                   // 仿真时间
    time.setTimePoint(epoch_ + t);                  // 设置仿真时间点
    time.setElapsedTime(t);                         // 设置仿真的相对时间
    // @bug setStateData 必须要求状态量和导数量的排列和维度与输入数据的一致
    dynamicSystem_.setStateData(y);                 // 设置状态数据
    errc_t err = dynamicSystem_.run(time);          // 执行动力学系统
    dynamicSystem_.getDerivativeData(dy);           // 获取导数数据
    return err;                                     // 返回错误码
}

int HPOPEquation::getDimension() const
{
    // @fixme
    return dynamicSystem_.getDimension();
}

/// @brief 初始化仿真引擎
/// @details 幂等：仅在建图尚未完成（初次调用）时执行。配置变更需由调用方重建方程（HPOP 通过置空 equation_ 实现）。
errc_t HPOPEquation::initialize(const HPOPForceModel &forceModel, const SpacecraftParam &spacecraftParam, Frame *frame)
{
    errc_t rc = this->initBlocks(forceModel, spacecraftParam, frame);
    if(rc) return rc;
    return dynamicSystem_.initialize();
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

/// @brief 根据 GravityForce 配置加载重力场系数，并施加永久潮汐修正（如适用）
/// @param gravity 重力场配置
/// @param body 中心天体
/// @param[out] outGravityField 输出的重力场
/// @return 错误码
static errc_t aLoadGravityField(const GravityForce& gravity, const Body& body, GravityField& outGravityField)
{
    errc_t err = outGravityField.load(gravity.model_, gravity.maxDegree_, gravity.maxOrder_, body.getDirpath());
    if(err != eNoError)
    {
        aError("Failed to load gravity field '%s' for body '%s'.", gravity.model_.c_str(), body.getName().c_str());
        return err;
    }
    // 施加永久固体潮汐修正（仅对地球无潮汐模型且 solidTideType_==ePermanentOnly 时生效，
    // 非地球天体或已是零潮汐模型则在 applyPermanentTideC20Correction 内部跳过）
    // 参考: IERS 2010 TN36 第6章 第6.2.2节 公式(6.13)-(6.14) (p.88)
    if(body.isEarth())
    {
        if(gravity.solidTideType_ == ESolidTideType::ePermanentOnly)
        {
            outGravityField.applyPermanentTideC20Correction(kEarthK20LoveNumber);
        }
    }
    return eNoError;
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

    // 目前的大气模型都是地球的大气模型
    auto earth = aGetEarth();
    if(!earth)
        return nullptr;
    
    BodyShape* shape = earth->getShape();
    Frame* frame = earth->getFrameFixed();

    double f10p7Daily = clamp_f10p7(drag.f10p7Daily_);
    double f10p7Average = clamp_f10p7(drag.f10p7Average_);


    ScopedPtr<SpaceWeatherProvider> spaceWeather;
    // 伪循环用于执行后备方案
    do {
        if (drag.useFluxApFile_)
        {
            auto swfile = new SpaceWeather();
            spaceWeather = swfile;
            // @todo: 这里可以添加缓存机制，避免重复加载相同的文件
            errc_t err = swfile->load(drag.fluxApFile_);
            if (!err)
            {
                break;
            }
            else
            {
                aWarning("failed to load space weather file '%s', using constant space weather instead.", drag.fluxApFile_.c_str());
            }
        }
        // 后备方案
        {
            spaceWeather = new ConstantSpaceWeather(f10p7Daily, f10p7Average, drag.ap(), drag.kp_);
        }
    }while(false);
    

    auto geoMagFluxUpdateRate = drag.geoMagFluxUpdateRate_;
    AtmosphereBase* atmosphere = nullptr;
    auto atmDensityModel = drag.atmDensityModel_;
    {
        if(atmDensityModel == EAtmDensityModel::eMSIS1986)
        {
            auto misi86 = new MSIS86(frame, shape, spaceWeather.release());
            misi86->setUseDailyAp(geoMagFluxUpdateRate==EGeoMagFluxUpdateRate::eDaily);
            atmosphere = misi86;
        }
        else if(atmDensityModel == EAtmDensityModel::eMSISE1990)
        {
            auto misi90 = new MSISE90(frame, shape, spaceWeather.release());
            misi90->setUseDailyAp(geoMagFluxUpdateRate==EGeoMagFluxUpdateRate::eDaily);
            atmosphere = misi90;
        }
        else if(atmDensityModel == EAtmDensityModel::eNRLMSISE2000)
        {
            auto misi000 = new NRLMSIS00(frame, shape, spaceWeather.release());
            misi000->setUseDailyAp(geoMagFluxUpdateRate==EGeoMagFluxUpdateRate::eDaily);
            atmosphere = misi000;
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
        else if(atmDensityModel == EAtmDensityModel::eHarrisPriester)
        {
            auto hp = new HarrisPriester(frame, shape, aGetSun(), f10p7Average);
            hp->setSunPosition(drag.sunPosition_);
            atmosphere = hp;
        }
        else if(atmDensityModel == EAtmDensityModel::eDTM2012)
        {
            auto dtm = new DTM2012(frame, shape, f10p7Daily, f10p7Average, drag.kp_,
                                earth->getDirpath() + "/DTM/dtm_2012_NF.dat");
            if (!dtm->isInitialized())
            {
                aWarning("DTM2012 initialization failed");
                delete dtm;
            }
            else
            {
                atmosphere = dtm;
            }
        }
    }

    if(!atmosphere)
    {
        aWarning("atmosphere '%d' is not supported, using default model 'NRLMSIS00'.", atmDensityModel);
        atmosphere = new NRLMSIS00(frame, shape, f10p7Daily, f10p7Average, drag.ap());
    }
    atmosphere->setUseApproximateAltitude(drag.useApproxAltForDrag_);
    return atmosphere;
}

errc_t HPOPEquation::initBlocks(const HPOPForceModel &forceModel, const SpacecraftParam &spacecraftParam, Frame *frame)
{
    // 检查中心天体
    auto body = forceModel.centralBody();
    if(!body)
    {
        body = aGetEarth();
        assert(body);
        aWarning("central body is not set, using 'Earth' as default.");
        if(!body)
            return eErrorNullInput;
    }

    // 解析预报坐标系：未显式指定则用中心天体的惯性系
    if(!frame)
    {
        frame = body->getFrameInertial();
        assert(frame);
        aWarning("propagation frame is not set, using the inertial frame of '%s' as the default propagation frame.", body->getName().c_str());
        if(!frame)
            return eErrorNullInput;
    }
    else
    {
        if(frame->getOrigin() != body)
        {
            aError("propagation frame '%s' origin is not central body '%s'.", frame->getName().c_str(), body->getName().c_str());
            return eErrorInvalidParam;
        }
    }
    // 将力模型配置转换为动力学系统的一个个函数块
    auto& bodyAttraction = forceModel.bodyAttraction();

    // 创建块工厂（根据是否启用 STM 决定 new 哪个类型）
    AstroBlockFactory factory(forceModel.useSTM());

    // 重置动力学系统
    this->reset();

    // 添加运动学函数块
    // 先添加运动学函数块，再添加其他块，确保Pos状态在状态量列表的开头
    this->addBlock(factory.createMotionBlock());

    
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
                this->addBlock(factory.createTwoBodyBlock(gm));
            }else{
                GravityField gravityField;
                errc_t err = aLoadGravityField(gravity, *body, gravityField);
                if(err != eNoError) return err;
                auto propAxes = frame->getAxes();
                auto gravityAxes = aGetGravityAxes(gravityField, *body);
                BlockGravity* blockGravity = factory.createGravityBlock(std::move(gravityField), gravity.maxDegree_, gravity.maxOrder_, gravityAxes, propAxes);
                // @todo 考虑统一处理 useSTM 带来的特殊参数设置
                if(forceModel.useSTM())
                {
                    // @todo 避免类型强制转换
                    BlockGravityPartial* blockGravityPartial = static_cast<BlockGravityPartial*>(blockGravity);
                    blockGravityPartial->setDegreeForPartial(gravity.maxDegreeForPartial_);
                    blockGravityPartial->setOrderForPartial(gravity.maxOrderForPartial_);
                }
                // 设置是否考虑重力场系数变化
                blockGravity->setConsiderVariations(gravity.useSecularVariations_);
                this->addBlock(blockGravity);
            }
        }
        // 添加二体引力函数块
        else if(auto pointMassPtr = bodyAttraction.asPointMassForce())
        {
            double gm = pointMassPtr->getGM(body);
            this->addBlock(factory.createTwoBodyBlock(gm));
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
        blockMass = factory.createMassBlock(spacecraftParam.mass());
        this->addBlock(blockMass);

        Atmosphere* atmosphere = aNewAtmosphere(forceModel.drag());
        // atmosphere 的所有权转移给 blockDrag
        BlockDrag* blockDrag = factory.createDragBlock(atmosphere, spacecraftParam.cd(), spacecraftParam.dragArea(), frame);
        if (forceModel.useSTM()) {
            static_cast<BlockDragPartial*>(blockDrag)->setUseDragSensitivity(forceModel.useDragSensitivity());
        }
        // 记录阻力函数块指针
        this->dragBlock_ = blockDrag;
        this->addBlock(blockDrag);
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
                blockMass = factory.createMassBlock(spacecraftParam.mass());
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
            BlockSRP* blockSRP = factory.createSRPBlock(eclipseCalculator, spacecraftParam.cr(), spacecraftParam.srpArea(), frame);
            blockSRP->setSunPosition(forceModel.srp().sunPosition_); // 设置太阳位置
            if (forceModel.useSTM()) {
                static_cast<BlockSRPPartial*>(blockSRP)->setUseSRPSensitivity(forceModel.useSRPSensitivity());
            }
            // 记录SRP光压函数块指针
            this->srpBlock_ = blockSRP;
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
            if(body3rd == nullptr || body3rd == body)
            {
                if(body3rd == nullptr)
                    aWarning("third body pointer is null");
                else
                    aWarning("third body '%s' is the same as the central body, skipping this third body.", body3rd->name().c_str());
                continue;
            }
            Point* bodyEphemeris = aGetBodyEphemeris(*body3rd, thirdBody.ephemerisSource());
            if(thirdBody.bodyAttractionType() == EBodyAttractionType::eGravity)
            {
                // 三体使用球谐重力场
                auto& gravity = thirdBody.gravity();
                // @todo 如果 0 == gravity.maxDegree_，可以使用点质量引力简化计算逻辑
                GravityField gravityField;
                errc_t err = aLoadGravityField(gravity, *body3rd, gravityField);
                if(err != eNoError) return err;
                auto gravityAxes = aGetGravityAxes(gravityField, *body3rd);
                auto* block = factory.createThirdBodyGravityBlock(
                    bodyEphemeris, std::move(gravityField),
                    gravity.maxDegree_, gravity.maxOrder_,
                    gravityAxes, frame
                );
                block->setConsiderVariations(gravity.useSecularVariations_);
                if(forceModel.useSTM())
                {
                    // @todo 避免类型强制转换
                    BlockThirdBodyGravityPartial* blockPartial = static_cast<BlockThirdBodyGravityPartial*>(block);
                    blockPartial->setDegreeForPartial(gravity.maxDegreeForPartial_);
                    blockPartial->setOrderForPartial(gravity.maxOrderForPartial_);
                }
                this->addBlock(block);
            }
            else
            {
                // 三体使用点质量引力（默认行为）
                double gm = thirdBody.pointMass().getGM(body3rd);
                this->addBlock(factory.createThirdBodyPointMassBlock(bodyEphemeris, gm, frame));
            }
        }
    }

    // [月球特定的逻辑，待废弃]
    // 添加月球引力函数块
    if(forceModel.useMoonGravity())
    {
        this->addBlock(factory.createThirdBodyPointMassBlock(aGetMoon(), forceModel.moonGravity(), frame));
    }

    // 添加 STM 状态转换矩阵函数块
    if(forceModel.useSTM())
    {
        this->addBlock(factory.createStateTransitionMatrixBlock());
        if (forceModel.useDragSensitivity()) {
            this->addBlock(factory.createDragSensitivityBlock());
        }
        if (forceModel.useSRPSensitivity()) {
            this->addBlock(factory.createSRPSensitivityBlock());
        }
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

void HPOPEquation::reset()
{
    this->dragBlock_ = nullptr;
    this->srpBlock_ = nullptr;
    dynamicSystem_.reset();
}




AST_NAMESPACE_END
