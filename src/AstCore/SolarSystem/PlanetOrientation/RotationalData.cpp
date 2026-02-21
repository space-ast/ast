///
/// @file      RotationalData.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-20
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

#include "RotationalData.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/String.hpp"
#include "AstMath/Euler.hpp"

AST_NAMESPACE_BEGIN

err_t RotationalData::load(StringView filepath)
{
    BKVParser parser(filepath);
    if(!parser.isOpen()){
        aError("failed to open file %.*s", (int)filepath.size(), filepath.data());
        return eErrorInvalidFile;
    }
    BKVItemView item;
    BKVParser::EToken token;
    RotationalData data;
    bool foundRightAscension = false;
    bool foundDeclination = false;
    bool foundRotation = false;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "RotationEpoch"))
            {
                double epoch = 0.0;
                err_t rc = aParseDouble(item.value(), epoch);
                if(rc){
                    aError("failed to parse RotationEpoch");
                    return rc;
                }else{
                    data.rotationEpoch_ = TimePoint::FromImpreciseJDTT(epoch);
                }
            }
            else if(aEqualsIgnoreCase(item.key(), "ModelName"))
            {
                // pass
            }
            else if(aEqualsIgnoreCase(item.key(), "Description"))
            {
                // pass
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "RotationalData"))
            {
                // pass
            }
            else if(aEqualsIgnoreCase(item.value(), "RotationalElement"))
            {
                token = parser.getNext(item);
                if(token == BKVParser::eKeyValue && aEqualsIgnoreCase(item.key(), "Type"))
                {
                    if(aEqualsIgnoreCase(item.value(), "SpinAxisRightAscension"))
                    {
                        foundRightAscension = true;
                        err_t rc = data.rightAscension_.load(parser);
                        if(rc){
                            aError("failed to load SpinAxisRightAscension");
                            return rc;
                        }
                    }
                    else if(aEqualsIgnoreCase(item.value(), "SpinAxisDeclination"))
                    {
                        foundDeclination = true;
                        err_t rc = data.declination_.load(parser);
                        if(rc){
                            aError("failed to load SpinAxisDeclination");
                            return rc;
                        }
                    }
                    else if(aEqualsIgnoreCase(item.value(), "Rotation"))
                    {
                        foundRotation = true;
                        err_t rc = data.rotation_.load(parser);
                        if(rc){
                            aError("failed to load Rotation");
                            return rc;
                        }
                    }
                }else{
                    aError("invalid key '%.*s', expect 'Type'", (int)item.key().size(), item.key().data());
                    return eErrorParse;
                }
            }
            // pass
        }else if(token == BKVParser::eBlockEnd){
            
            // pass
        }
    }while(token != BKVParser::eEOF);
    if(!(foundRightAscension && foundDeclination && foundRotation)){
        aError("missing RotationElement, expect SpinAxisRightAscension, SpinAxisDeclination, Rotation");
        return eErrorParse;
    }
    *this = std::move(data);
    return eNoError;
}

void RotationalData::getICRFToFixedMatrix(const TimePoint &tp, Matrix3d &matrix) const
{
    double d = tp.daysFrom(rotationEpoch_);
    double t = d / kDaysPerJulianCentury;
    double rightAscension = rightAscension_.evaluate(t, t, d);
    double declination = declination_.evaluate(t, t, d);
    double rotation = rotation_.evaluate(d, t, d);
    aEuler313ToMatrix({kHalfPI + rightAscension, kHalfPI - declination, rotation}, matrix);
}

void RotationalData::getICRFToFixedTransform(const TimePoint &tp, Rotation &rotation) const
{
    this->getICRFToFixedMatrix(tp, rotation.getMatrix());
}

void RotationalData::getICRFToInertialMatrix(const TimePoint &tp, Matrix3d &matrix) const
{
    double rightAscension = rightAscension_.evaluateZero();
    double declination = declination_.evaluateZero();
    double rotation = 0;
    aEuler313ToMatrix({kHalfPI + rightAscension, kHalfPI - declination, rotation}, matrix);
}

void RotationalData::getICRFToInertialTransform(const TimePoint &tp, Rotation &rot) const
{
    this->getICRFToInertialMatrix(tp, rot.getMatrix());
}

void RotationalData::getICRFToTODMatrix(const TimePoint &tp, Matrix3d &matrix) const
{
    double d = tp.daysFrom(rotationEpoch_);
    double t = d / kDaysPerJulianCentury;
    double rightAscension = rightAscension_.evaluate(t, t, d);
    double declination = declination_.evaluate(t, t, d);
    double rotation = 0;
    aEuler313ToMatrix({kHalfPI + rightAscension, kHalfPI - declination, rotation}, matrix);
}

void RotationalData::getICRFToTODTransform(const TimePoint &tp, Rotation &rot) const
{
    this->getICRFToTODMatrix(tp, rot.getMatrix());
}

void RotationalData::getICRFToMODMatrix(const TimePoint &tp, Matrix3d &matrix) const
{
    double d = tp.daysFrom(rotationEpoch_);
    double t = d / kDaysPerJulianCentury;
    double rightAscension = rightAscension_.evaluatePoly(t, t);
    double declination = declination_.evaluatePoly(t, t);
    double rotation = 0;
    aEuler313ToMatrix({kHalfPI + rightAscension, kHalfPI - declination, rotation}, matrix);
}

void RotationalData::getICRFToMODTransform(const TimePoint &tp, Rotation &rot) const
{
    this->getICRFToMODMatrix(tp, rot.getMatrix());
}

AST_NAMESPACE_END


