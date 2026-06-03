///
/// @file      ObjectIcons.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-03
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

#include "ObjectIcons.hpp"
#include "AstUtil/Object.hpp"

AST_NAMESPACE_BEGIN

QString iconPath(const QString& name)
{
    #ifdef A_WASM
    // wasm 不会存在data目录和exe目录分离的情况，所以直接使用相对路径
    return "data/icons/" + name + ".svg";
    #else
    // 其他平台需要通过aDataDir获取data目录路径，避免其他软件调用ast库时的路径错误
    return QString::fromStdString(aDataDir()) + "/icons/" + name + ".svg";
    #endif
}

QIcon aUiLoadIcon(const QString& name)
{
    return QIcon(iconPath(name));
}

QIcon aUiObjectIcon(const Object* obj)
{
    if (obj)
    {
        return aUiClassIcon(QString::fromStdString(obj->typeName()));
    }
    return aUiLoadIcon(QStringLiteral("Object"));
}


QIcon aUiClassIcon(const QString& className)
{
    QIcon icon = aUiLoadIcon(className);
    if(!icon.isNull())
        return icon;
    return aUiLoadIcon(QStringLiteral("Object"));
}


AST_NAMESPACE_END

