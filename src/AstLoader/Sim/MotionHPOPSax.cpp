///
/// @file      MotionHPOPSax.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-19
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

#include "MotionHPOPSax.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstCore/HPOP.hpp"
#include "AstUtil/BKVParser.hpp"

#include "AstMath/RKF78.hpp"
#include "AstMath/RK4.hpp"
#include "AstMath/RKF45.hpp"
#include "AstMath/RKF56.hpp"

AST_NAMESPACE_BEGIN


errc_t _aLoadSolarPressureModel(BKVParser& parser, double massAtEpoch, SolarRadiationPressure& srp)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Coefficient")){
                // srp.coefficient_ = item.value().toDouble();
            }
            else if(aEqualsIgnoreCase(item.key(), "Area")){
                // srp.areaMassRatio_ = item.value().toDouble() / massAtEpoch;
            }
            else if(aEqualsIgnoreCase(item.key(), "ReflectionModel")){
                // @todo 解析反射模型
            }else if(aEqualsIgnoreCase(item.key(), "CorrectionType")){
                // @todo 解析修正类型
            }
        }
        else if(token == BKVParser::eBlockBegin)
        {

        }else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "SolarPressureModel")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


static EAtmDensityModel _aStringToAtmDensityModel(StringView value)
{
    if(aEqualsIgnoreCase(value, "JacchiaRoberts")){
        return EAtmDensityModel::eJacchiaRoberts;
    } else if(aEqualsIgnoreCase(value, "1976Standard")){
        return EAtmDensityModel::e1976Standard;
    } else if(aEqualsIgnoreCase(value, "HarrisPriester")){
        return EAtmDensityModel::eHarrisPriester;
    } else if(aEqualsIgnoreCase(value, "Jacchia70")){
        return EAtmDensityModel::eJacchia70;
    } else if(aEqualsIgnoreCase(value, "Jacchia71")){
        return EAtmDensityModel::eJacchia71;
    }else if(aEqualsIgnoreCase(value, "MSIS00")){
        return EAtmDensityModel::eNRLMSISE2000;
    } else if(aEqualsIgnoreCase(value, "MSIS90")){
        return EAtmDensityModel::eMSISE1990;
    } else if(aEqualsIgnoreCase(value, "MSIS86")){
        return EAtmDensityModel::eMSIS1986;
    } else if(aEqualsIgnoreCase(value, "Jacchia60")){
        return EAtmDensityModel::eJacchia60;
    } else if(aEqualsIgnoreCase(value, "CIRA72")){
        return EAtmDensityModel::eCIRA72;
    } else if(aEqualsIgnoreCase(value, "DTM2012")){
        return EAtmDensityModel::eDTM2012;
    }else {
        return EAtmDensityModel::e1976Standard;
    }
    aWarning("unknown atm density model: '%.*s'", value.size(), value.data());
    return EAtmDensityModel::eNone;
}

errc_t MotionHPOPSax::keyValue(StringView key, const ValueView &value)
{
    if(aEqualsIgnoreCase(key, "X")){
        cartState_.x() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "Y")){
        cartState_.y() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "Z")){
        cartState_.z() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "VX")){
        cartState_.vx() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "VY")){
        cartState_.vy() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "VZ")){
        cartState_.vz() = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "GravityFile")){
        forceModel_.gravity().model_ = value.toString();
    }
    else if(aEqualsIgnoreCase(key, "MaxDegree")){
        forceModel_.gravity().maxDegree_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "MaxOrder")){
        forceModel_.gravity().maxOrder_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "UseGravSecularVariations")){
        forceModel_.gravity().useSecularVariations_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "SolidTideType")){
        if(aEqualsIgnoreCase(value, "PermanentOnly")){
            forceModel_.gravity().solidTideType_ = ESolidTideType::ePermanentOnly;
        } else if(aEqualsIgnoreCase(value, "Full")){
            forceModel_.gravity().solidTideType_ = ESolidTideType::eFull;
        } else if(aEqualsIgnoreCase(value, "None")){
            forceModel_.gravity().solidTideType_ = ESolidTideType::eNone;
        }else{
            // 
        }
    }
    else if(aEqualsIgnoreCase(key, "IncludeTimeDependSolidTides")){
        forceModel_.gravity().includeTimeDependentSolidTides_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "MinAmplitudeSolidTides")){
        forceModel_.gravity().minAmplitudeSolidTides_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "TruncateSolidTides")){
        forceModel_.gravity().truncateSolidTides_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "UseOceanTides")){
        forceModel_.gravity().useOceanTides_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "MaxDegreeOceanTides")){
        forceModel_.gravity().maxDegreeOceanTides_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "MaxOrderOceanTides")){
        forceModel_.gravity().maxOrderOceanTides_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "MinAmplitudeOceanTides")){
        forceModel_.gravity().minAmplitudeOceanTides_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "MassAtEpoch")){
        spacecraftParam_.mass_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "UseDrag")){
        forceModel_.useDrag(value.toBool());
    }
    else if(aEqualsIgnoreCase(key, "AtmDensityModel")){
        forceModel_.drag().atmDensityModel_ = _aStringToAtmDensityModel(value.toString());
        if(forceModel_.drag().atmDensityModel_ == EAtmDensityModel::eNone){
            aWarning("use default atm density model 'NRLMSISE2000'");
            forceModel_.drag().atmDensityModel_ = EAtmDensityModel::eNRLMSISE2000;
        }
    }
    else if(aEqualsIgnoreCase(key, "AtmBlendingRange")){
        forceModel_.drag().atmBlendingRange_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "LowAltAtmDensityModel")){
        forceModel_.drag().lowAltAtmDensityModel_ = _aStringToAtmDensityModel(value.toString());
    }
    else if(aEqualsIgnoreCase(key, "UseFluxApFile")){
        forceModel_.drag().useFluxApFile_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "FluxApFile")){
        forceModel_.drag().fluxApFile_ = value.toString();
    }
    else if(aEqualsIgnoreCase(key, "GeoMagneticFluxSource")){
        if(aEqualsIgnoreCase(value, "Kp")){
            forceModel_.drag().geoMagFluxSource_ = EGeoMagFluxSource::eKp;
        } else if(aEqualsIgnoreCase(value, "Ap")){
            forceModel_.drag().geoMagFluxSource_ = EGeoMagFluxSource::eAp;
        }else{
            // @todo 处理其他磁通量源
        }
    }
    else if(aEqualsIgnoreCase(key, "GeoMagneticFluxUpdateMethod")){
        if(aEqualsIgnoreCase(value, "Daily")){
            forceModel_.drag().geoMagFluxUpdateRate_ = EGeoMagFluxUpdateRate::eDaily;
        }else{
            // @todo 处理其他更新方法
        }
        // @todo 处理其他更新方法
    }
    else if(aEqualsIgnoreCase(key, "GeoMagneticFluxInterpSubSamplingRatio")){
        forceModel_.drag().geoMagFluxInterpSubSamplingRatio_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "DragCoefficient")){
        spacecraftParam_.dragCoefficient_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "AreaMassRatio")){
        spacecraftParam_.areaMassRatio_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "DragCorrectionType")){
        // @todo 处理DragCorrectionType
    }
    else if(aEqualsIgnoreCase(key, "AverageF10.7")){
        forceModel_.drag().f10p7Average_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "DailyF10.7")){
        forceModel_.drag().f10p7Daily_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "Kp")){
        forceModel_.drag().kp_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "UseApproxAltForDrag")){
        forceModel_.drag().useApproxAltForDrag_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "UseApparentSunForDrag")){
        if(value.toBool())
            forceModel_.drag().sunPosition_ = ESunPosition::eApparent;
        else
            forceModel_.drag().sunPosition_ = ESunPosition::eTrue;
    }
    else if(aEqualsIgnoreCase(key, "DensityType")){
        // @todo 处理DensityType
    }
    else if(aEqualsIgnoreCase(key, "UseSRP")){
        forceModel_.useSRP(value.toBool());
    }
    else if(aEqualsIgnoreCase(key, "SolarPressureModel")){
        double mass = this->getMass();
        errc_t rc = _aLoadSolarPressureModel(parser_, mass, forceModel_.srp());
        A_UNUSED(rc);
    }
    else if(aEqualsIgnoreCase(key, "SunPosition")){
        if(aEqualsIgnoreCase(value, "ApparentSunToTrueCB")){
            forceModel_.srp().sunPosition_ = ESunPosition::eApparentSunToTrueCB;
        } else if(aEqualsIgnoreCase(value, "Apparent")){
            forceModel_.srp().sunPosition_ = ESunPosition::eApparent;
        } else if(aEqualsIgnoreCase(value, "True")){
            forceModel_.srp().sunPosition_ = ESunPosition::eTrue;
        }
    }
    else if(aEqualsIgnoreCase(key, "DetectShadowBoundaries")){
        forceModel_.srp().detectShadowBoundaries_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "ShadowModel")){
        if(aEqualsIgnoreCase(value, "DualCone")){
            forceModel_.srp().shadowModel_ = EShadowModel::eDualCone;
        } else if(aEqualsIgnoreCase(value, "Cylindrical")){
            forceModel_.srp().shadowModel_ = EShadowModel::eCylindrical;
        } else if(aEqualsIgnoreCase(value, "None")){
            forceModel_.srp().shadowModel_ = EShadowModel::eNone;
        }else{
            // @todo 处理其他阴影模型
        }
    }
    else if(aEqualsIgnoreCase(key, "EclipsingBodies")){
        // @todo 处理EclipsingBodies
    }
    else if(aEqualsIgnoreCase(key, "AtmAltForEclipse")){
        forceModel_.srp().atmAltForEclipse_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "SRPModel")){
        // @todo 处理SRPModel
    }
    else if(aEqualsIgnoreCase(key, "IncludeGenRelCorrection")){
        // @todo 处理IncludeGenRelCorrection
    }
    else if(aEqualsIgnoreCase(key, "IncludeAlbedo")){
        // @todo 处理IncludeAlbedo
    }
    else if(aEqualsIgnoreCase(key, "IncludeThermalRadPressure")){
        // @todo 处理IncludeThermalRadPressure
    }
    else if(aEqualsIgnoreCase(key, "Include2ndOrderStateEstDeriv")){
        // @todo 处理Include2ndOrderStateEstDeriv
    }
    else if(aEqualsIgnoreCase(key, "RadPressureCoefficient")){
        // forceModel_.srp_.coefficient_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "RadPressureArea")){
        // @todo 处理RadPressureArea
    }
    else if(aEqualsIgnoreCase(key, "GroundReflectionModelFile")){
        // @todo 处理GroundReflectionModelFile
    }
    else if(aEqualsIgnoreCase(key, "ThirdBodyGravity")){
        std::vector<StringView> values = aStrSplit(value, ByRepeatedWhitespace(), SkipWhitespace());
        if(values.size() == 0) return 0;
        // 处理三体引力
        auto body = aGetBody(values[0]);
        ThirdBodyForce* thirdBodyForce{nullptr};
        if(body){
            thirdBodyForce = forceModel_.addThirdBody(body);
        }else{
            return 0;
        }
        if(thirdBodyForce)
        {
            thirdBodyForce->setAttractionType(EBodyAttractionType::ePointMass);
            if(values.size() == 1) return 0;
            StringView gmSource = values[1];
            auto& pointMass = thirdBodyForce->pointMass();
            if(aEqualsIgnoreCase(gmSource, "CbValue")){
                pointMass.gmSource_ = EGMSource::eBodyGravity;
            } else if(aEqualsIgnoreCase(gmSource, "JplDE")){
                pointMass.gmSource_ = EGMSource::eJplDE;
            } else if(aEqualsIgnoreCase(gmSource, "SpecifiedValue")){
                pointMass.gmSource_ = EGMSource::eSpecifiedValue;
            }
            if(values.size() == 2) return 0;
            if(pointMass.gmSource_ == EGMSource::eSpecifiedValue){
                pointMass.specifiedGM_ = aParseDouble(values[2]);
            }
        }
    }
    else if(aEqualsIgnoreCase(key, "EphRefFrame")){
        // @todo 处理EphRefFrame
    }
    // 下面是积分器的相关参数
    else if(aEqualsIgnoreCase(key, "IntegMethod")){
        integrator_.method_ = value.toString();
    }
    else if(aEqualsIgnoreCase(key, "UseVOP")){
        // @todo 处理UseVOP
    }
    else if(aEqualsIgnoreCase(key, "PseudoCorrect")){
        // @todo 处理PseudoCorrect
    }
    else if(aEqualsIgnoreCase(key, "UseRegTime")){
        // @todo 处理UseRegTime
    }
    else if(aEqualsIgnoreCase(key, "StepControlMethod")){
        integrator_.stepControlMethod_ = value.toString();
    }
    else if(aEqualsIgnoreCase(key, "ErrorTolerance")){
        integrator_.errorTolerance_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "TimeStep")){
        integrator_.timeStep_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "MinStepSize")){
        integrator_.minStepSize_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "MaxStepSize")){
        integrator_.maxStepSize_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "UseFixedAnomPredict")){
        // @todo 处理UseFixedAnomPredict
    }
    else if(aEqualsIgnoreCase(key, "FixedAnomalyStep")){
        // @todo 处理FixedAnomalyStep
    }
    else if(aEqualsIgnoreCase(key, "FixedAnomEccCutOff")){
        // @todo 处理FixedAnomEccCutOff
    }
    else if(aEqualsIgnoreCase(key, "ReportOnFixedStep")){
        integrator_.reportOnFixedStep_ = value.toBool();
    }
    else if(aEqualsIgnoreCase(key, "ReportKp")){
        // @todo 处理ReportKp
    }
    else if(aEqualsIgnoreCase(key, "InterpolationSamplesM1")){
        integrator_.interpolationSamplesM1_ = value.toInt();
    }
    else if(aEqualsIgnoreCase(key, "InterpolationMethod")){
        integrator_.interpolationMethod_ = value.toString();
    }
    else if(aEqualsIgnoreCase(key, "AltitudeCutOff")){
        integrator_.altitudeCutOff_ = value.toDouble();
    }
    else if(aEqualsIgnoreCase(key, "UsePlugin")){
        // @todo 处理UsePlugin
    }
    else {
        return MotionOrbitDynamicsSax::keyValue(key, value);
    }
    return 0;
}

errc_t MotionHPOPSax::getMotion(ScopedPtr<MotionProfile> &motion)
{
    PMotionHPOP motionHPOP = MotionHPOP::New();
    if(!motionHPOP){
        return eErrorInvalidParam;
    }
    
    SharedPtr<ODEIntegrator> integrator;
    // 设置积分器
    if(!integrator_.method_.empty())
    {
        if(aEqualsIgnoreCase(integrator_.method_, "RKF78")){
            integrator = new RKF78();
        }
        // else if(aEqualsIgnoreCase(integrator_.method_, "RK4FixedAnomaly")){}
        // else if(aEqualsIgnoreCase(integrator_.method_, "RKV89Efficient")){}
        // else if(aEqualsIgnoreCase(integrator_.method_, "BulirschStoer")){}
        // else if(aEqualsIgnoreCase(integrator_.method_, "GaussJackson")){}
        else{
            aWarning("unsupported integrator method '%s'", integrator_.method_.c_str());
        }
    }
    auto varIntegrator = aobject_cast<ODEVarStepIntegrator*>(integrator.get());
    auto fixedIntegrator = aobject_cast<ODEFixedStepIntegrator*>(integrator.get());
    if(varIntegrator){
        if(integrator_.stepControlMethod_ == "Fixed")
        {
            varIntegrator->setUseFixedStep(true);
        }
        else if(integrator_.stepControlMethod_ == "RelativeError")
        {
            varIntegrator->setUseFixedStep(false);
        }
        varIntegrator->setMaxRelErr(integrator_.errorTolerance_);
        varIntegrator->setUseMaxStep(true);
        varIntegrator->setMaxStepSize(integrator_.maxStepSize_);
        varIntegrator->setUseMinStep(true);
        varIntegrator->setMinStepSize(integrator_.minStepSize_);
    }
    if(fixedIntegrator){
        fixedIntegrator->setStepSize(integrator_.timeStep_);
    }

    // 这里可以安全地设置为空指针
    motionHPOP->setIntegrator(integrator);

    // 设置航天器参数

    SpacecraftParam scParam{};
    double mass = spacecraftParam_.mass_;
    scParam.setFuelMass(0);
    scParam.setDryMass(mass);
    scParam.setCd(spacecraftParam_.dragCoefficient_);
    scParam.setDragArea(spacecraftParam_.areaMassRatio_ * mass);
    motionHPOP->setSpacecraftParam(scParam);
    
    auto body = vehiclePathData_.centralBody_; AST_CHECK_NULLPTR(body);
    auto bodyInertial = body->makeFrameInertial();
    
    // 设置力模型
    motionHPOP->setForceModel(forceModel_);

    // 设置初始状态
    auto cartState = StateCartesian::New();
    cartState->setState(cartState_);
    cartState->setStateEpoch(makeStateEpoch());
    cartState->setFrame(bodyInertial);

    motionHPOP->setInitialState(cartState);
    motionHPOP->setInterval(makeInterval());
    motionHPOP->setPropagationFrame(bodyInertial);
    
    motion = motionHPOP;
    return eNoError;
}

AST_NAMESPACE_END