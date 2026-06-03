///
/// @file      ObjectIcons.hpp
/// @brief     对象类型图标提供器
/// @details   根据对象 RTTI 类型名匹配对应图标
/// @author    axel
/// @date      2026-05-24
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

#pragma once

#include "AstGlobal.h"
#include <QIcon>
#include <QCoreApplication>
#include <QString>
#include "AstCore/RunTime.hpp"

AST_NAMESPACE_BEGIN

class Object;

inline QIcon loadIcon(const QString& name)
{
    #ifdef A_WASM
    // wasm 不会存在data目录和exe目录分离的情况，所以直接使用相对路径
    QString path = "data/icons/" + name + ".svg";
    #else
    // 其他平台需要通过aDataDir获取data目录路径，避免其他软件调用ast库时的路径错误
    QString path = QString::fromStdString(aDataDir()) + "/icons/" + name + ".svg";
    #endif
    return QIcon(path);
}

inline QIcon objectIcon(const Object* obj)
{
    if (!obj)
        return loadIcon(QStringLiteral("Object"));

    auto name = QString::fromStdString(obj->typeName());

    if (name.contains(QStringLiteral("Satellite")))
        return loadIcon(QStringLiteral("Satellite"));
    if (name.contains(QStringLiteral("Facility")))
        return loadIcon(QStringLiteral("Facility"));
    if (name.contains(QStringLiteral("Sensor")))
        return loadIcon(QStringLiteral("Sensor"));
    if (name.contains(QStringLiteral("State")))
        return loadIcon(QStringLiteral("OrbitState"));
    if (name.contains(QStringLiteral("Spacecraft")))
        return loadIcon(QStringLiteral("Spacecraft"));
    if (name.contains(QStringLiteral("Force")))
        return loadIcon(QStringLiteral("Force"));
    if (name.contains(QStringLiteral("Body")) || name.contains(QStringLiteral("Ephemeris")))
        return loadIcon(QStringLiteral("Body"));
    if (name.contains(QStringLiteral("Propagat")))
        return loadIcon(QStringLiteral("Propagate"));
    if (name.contains(QStringLiteral("Maneuver")) || name.contains(QStringLiteral("Burn")))
        return loadIcon(QStringLiteral("Maneuver"));
    if (name.contains(QStringLiteral("Sequence")))
        return loadIcon(QStringLiteral("Sequence"));
    if (name.contains(QStringLiteral("LandingSite")))
        return loadIcon(QStringLiteral("LandingSite"));
    if (name.contains(QStringLiteral("Segment")))
        return loadIcon(QStringLiteral("Segment"));
    if (name.contains(QStringLiteral("Targeter")))
        return loadIcon(QStringLiteral("TargeterSequence"));

    if (name.contains(QStringLiteral("Angle")))
        return loadIcon(QStringLiteral("Angle"));
    if (name.contains(QStringLiteral("Axes")))
        return loadIcon(QStringLiteral("Axes"));
    if (name.contains(QStringLiteral("Frame")))
        return loadIcon(QStringLiteral("Frame"));
    if (name.contains(QStringLiteral("Plane")))
        return loadIcon(QStringLiteral("Plane"));
    if (name.contains(QStringLiteral("Point")))
        return loadIcon(QStringLiteral("Point"));
    if (name.contains(QStringLiteral("Vector")))
        return loadIcon(QStringLiteral("Vector"));

    return loadIcon(QStringLiteral("Object"));
}

AST_NAMESPACE_END
