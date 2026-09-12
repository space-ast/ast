///
/// @file      SensorLoader.cpp
/// @brief     传感器加载器实现
/// @author    axel
/// @date      2026-04-07
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "SensorLoader.hpp"
#include "CommonlyUsedHeaders.hpp"
#include "AstSim/Sensor.hpp"
#include "AstSim/CentroidPosition.hpp"
#include "AstCore/FOVSimpleCone.hpp"
#include "AstCore/FOVConical.hpp"
#include "AstCore/FOVHalfPower.hpp"
#include "AstCore/FOVRectangular.hpp"
#include "AstCore/FOVSAR.hpp"
#include "AstCore/FOVCustom.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN


errc_t _aLoadSimpleCone(BKVParser& parser, FOVSimpleCone& fov)
{
    BKVItemView coneItem;
    BKVParser::EToken coneToken;
    do{
        coneToken = parser.getNext(coneItem);
        if(coneToken == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(coneItem.key(), "ConeAngle")){
                fov.setConeAngle(coneItem.value().toAngleRad());
            }else if(aEqualsIgnoreCase(coneItem.key(), "AngularPatternResolution")){
                fov.setAngularPatternResolution(coneItem.value().toAngleRad());
            }
        }else if(coneToken == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(coneItem.value(), "SimpleCone")){
                break;
            }
        }
    }while(coneToken != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadConical(BKVParser& parser, FOVConical& fov)
{
    BKVItemView coneItem;
    BKVParser::EToken coneToken;
    do{
        coneToken = parser.getNext(coneItem);
        if(coneToken == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(coneItem.key(), "InnerConeAngle")){
                fov.setInnerConeAngle(coneItem.value().toAngleRad());
            }else if(aEqualsIgnoreCase(coneItem.key(), "OuterConeAngle")){
                fov.setOuterConeAngle(coneItem.value().toAngleRad());
            }else if(aEqualsIgnoreCase(coneItem.key(), "MinClockAngle")){
                fov.setMinClockAngle(coneItem.value().toAngleRad());
            }else if(aEqualsIgnoreCase(coneItem.key(), "MaxClockAngle")){
                fov.setMaxClockAngle(coneItem.value().toAngleRad());
            }
        }else if(coneToken == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(coneItem.value(), "Conical")){
                break;
            }
        }
    }while(coneToken != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadHalfPower(BKVParser& parser, FOVHalfPower& fov)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "HalfAngle")){
                fov.setHalfAngle(item.value().toAngleRad());
            }else if(aEqualsIgnoreCase(item.key(), "Frequency")){
                fov.setFrequency(item.value().toDouble());
            }else if(aEqualsIgnoreCase(item.key(), "AntennaDiameter")){
                fov.setAntennaDiameter(item.value().toDouble());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "HalfPower")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadRectangular(BKVParser& parser, FOVRectangular& fov)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "SideToSideAngle")){
                fov.setHorizontalHalfAngle(item.value().toAngleRad());
            }else if(aEqualsIgnoreCase(item.key(), "UpDownAngle")){
                fov.setVerticalHalfAngle(item.value().toAngleRad());
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Rectangular")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadSAR(BKVParser& parser, FOVSAR& fov)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "MinElevAngle")){
                fov.setMinElevAngle(item.value().toAngleRad());
            }else if(aEqualsIgnoreCase(item.key(), "MaxElevAngle")){
                fov.setMaxElevAngle(item.value().toAngleRad());
            }else if(aEqualsIgnoreCase(item.key(), "FwdExclAngle")){
                fov.setForwardExcludeAngle(item.value().toAngleRad());
            }else if(aEqualsIgnoreCase(item.key(), "AftExclAngle")){
                fov.setBackwardExcludeAngle(item.value().toAngleRad());
            }else if(aEqualsIgnoreCase(item.key(), "Altitude")){
                fov.setAltitude(item.value().toDouble());
            }else if(aEqualsIgnoreCase(item.key(), "UpdateMode")){
                FOVSAR::EUpdateMode updateMode = FOVSAR::eConstant;
                if(aEqualsIgnoreCase(item.value(), "Constant")){
                    updateMode = FOVSAR::eConstant;
                }else if(aEqualsIgnoreCase(item.value(), "Dynamic")){
                    updateMode = FOVSAR::eDynamic;
                }else{
                    aError(_("无效的 UpdateMode 值: '%s'"), item.value().toString().c_str());
                }
                fov.setUpdateMode(updateMode);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "SAR")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t _aLoadCustom(BKVParser& parser, FOVCustom& fov)
{
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Custom")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器配置
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorPattern(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    ScopedPtr<FieldOfView> fov;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "DetailedTerrainHorizon")){
                // @todo 设置详细地形地平线
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "SimpleCone")){
                FOVSimpleCone* simpleCone = new FOVSimpleCone();
                _aLoadSimpleCone(parser, *simpleCone);
                fov = simpleCone;
            }else if(aEqualsIgnoreCase(item.value(), "Conical")){
                FOVConical* conical = new FOVConical();
                _aLoadConical(parser, *conical);
                fov = conical;
            }else if(aEqualsIgnoreCase(item.value(), "HalfPower")){
                FOVHalfPower* halfPower = new FOVHalfPower();
                _aLoadHalfPower(parser, *halfPower);
                fov = halfPower;
            }else if(aEqualsIgnoreCase(item.value(), "Rectangular")){
                FOVRectangular* rectangular = new FOVRectangular();
                _aLoadRectangular(parser, *rectangular);
                fov = rectangular;
            }else if(aEqualsIgnoreCase(item.value(), "SAR")){
                FOVSAR* sar = new FOVSAR();
                _aLoadSAR(parser, *sar);
                fov = sar;
            }else if(aEqualsIgnoreCase(item.value(), "Custom")){
                FOVCustom* custom = new FOVCustom();
                _aLoadCustom(parser, *custom);
                fov = custom;
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Pattern")){
                if(fov){
                    sensor.setFieldOfView(fov.release());
                }
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器指向信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorPointing(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Fixed")){
                BKVItemView fixedItem;
                BKVParser::EToken fixedToken;
                do{
                    fixedToken = parser.getNext(fixedItem);
                    if(fixedToken == BKVParser::eKeyValue){
                        if(aEqualsIgnoreCase(fixedItem.key(), "PointingMethod")){
                            // @todo 设置指向方法
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Sequence")){
                            // @todo 设置序列
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qx")){
                            // @todo 设置四元数x
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qy")){
                            // @todo 设置四元数y
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qz")){
                            // @todo 设置四元数z
                        }else if(aEqualsIgnoreCase(fixedItem.key(), "Qs")){
                            // @todo 设置四元数s
                        }
                    }else if(fixedToken == BKVParser::eBlockEnd){
                        if(aEqualsIgnoreCase(fixedItem.value(), "Fixed")){
                            break;
                        }
                    }
                }while(fixedToken != BKVParser::eEOF);
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Pointing")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器光学信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorOptics(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "FocalLength")){
                // @todo 设置焦距
            }else if(aEqualsIgnoreCase(item.key(), "DetectorPitch")){
                // @todo 设置探测器间距
            }else if(aEqualsIgnoreCase(item.key(), "IQConstant")){
                // @todo 设置IQ常数
            }else if(aEqualsIgnoreCase(item.key(), "GsdCoefficient")){
                // @todo 设置GSD系数
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Optics")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器位置信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorLocation(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Location")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器扩展信息
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t _aLoadSensorExtensions(BKVParser& parser, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "ExternData")){
                // @todo 处理外部数据
            }else if(aEqualsIgnoreCase(item.value(), "ADFFileData")){
                // @todo 处理ADF文件数据
            }else if(aEqualsIgnoreCase(item.value(), "AccessConstraints")){
                // @todo 处理访问约束
            }else if(aEqualsIgnoreCase(item.value(), "ObjectCoverage")){
                // @todo 处理对象覆盖
            }else if(aEqualsIgnoreCase(item.value(), "Desc")){
                // @todo 处理描述
            }else if(aEqualsIgnoreCase(item.value(), "Refraction")){
                // @todo 处理折射
            }else if(aEqualsIgnoreCase(item.value(), "Crdn")){
                // @todo 处理坐标系统
            }else if(aEqualsIgnoreCase(item.value(), "Graphics")){
                // @todo 处理图形
            }else if(aEqualsIgnoreCase(item.value(), "Swath")){
                // @todo 处理扫描带
            }else if(aEqualsIgnoreCase(item.value(), "VO")){
                // @todo 处理VO
            }else if(aEqualsIgnoreCase(item.value(), "DIS")){
                // @todo 处理DIS
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "Extensions")){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}


/// @brief 加载传感器对象
/// @param parser BKV解析器
/// @param sensorType 传感器类型
/// @param sensor 传感器引用
/// @return 错误码
errc_t aLoadSensor(BKVParser &parser, StringView sensorType, Sensor &sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Name")){
                sensor.setName(item.value());
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "Pattern")){
                if(errc_t rc = _aLoadSensorPattern(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Location")){
                if(errc_t rc = _aLoadSensorLocation(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Pointing")){
                if(errc_t rc = _aLoadSensorPointing(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Optics")){
                if(errc_t rc = _aLoadSensorOptics(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "Extensions")){
                if(errc_t rc = _aLoadSensorExtensions(parser, sensor)){
                    return rc;
                }
            }else if(aEqualsIgnoreCase(item.value(), "SubObjects")){
                if(errc_t rc = _aLoadSubObjects(parser, &sensor)){
                    return rc;
                }
            }
        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), sensorType)){
                return eNoError;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

/// @brief 加载传感器对象
/// @param parser BKV解析器
/// @param sensor 传感器引用
/// @return 错误码
errc_t aLoadSensor(BKVParser& parser, Sensor& sensor)
{
    return aLoadSensor(parser, "Sensor", sensor);
}

/// @brief 加载传感器对象
/// @param filepath 文件路径
/// @param sensorType 传感器类型
/// @param sensor 传感器引用
/// @return 错误码
errc_t aLoadSensor(StringView filepath, StringView sensorType, Sensor& sensor)
{
    BKVItemView item;
    BKVParser::EToken token;
    BKVParser parser(filepath);
    
    if(!parser.isOpen()){
        aError(_("打开文件 '%.*s' 失败"), (int)filepath.size(), filepath.data());
        return eErrorInvalidFile;
    }
    
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            // 处理文件开头的版本信息和作者信息
            // @todo 处理版本信息
        }
        else if(token == BKVParser::eBlockBegin){
            // 第一个BEGIN END块的名称就是对象的类型
            StringView objectType = item.value();
            return aLoadSensor(parser, objectType, sensor);
        }
    }while(token != BKVParser::eEOF);
    
    return eNoError;
}

/// @brief 加载传感器对象
/// @param filepath 文件路径
/// @param sensor 传感器引用
/// @return 错误码
errc_t aLoadSensor(StringView filepath, Sensor& sensor)
{
    return aLoadSensor(filepath, "Sensor", sensor);
}

AST_NAMESPACE_END