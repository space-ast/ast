///
/// @file      UiUtil.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-12
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
#include "AstUtil/JsonValue.hpp"
#include <QMetaObject>
#include <QApplication>
#include <QAbstractItemModel>
#include <functional>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 添加一个排队执行的回调函数
/// @param func 要排队执行的函数
/// @param type 连接类型，默认 Qt::QueuedConnection
AST_UIUTIL_CAPI void aAddQueued(std::function<void()> func, Qt::ConnectionType type);


/// @brief 添加一个排队执行的回调函数
/// @param func 要排队执行的函数
template<typename Func>
inline void addQueued(Func &&func) {
    const auto type = Qt::QueuedConnection;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    // Qt 5.10+: 直接使用 functor 重载
    QMetaObject::invokeMethod(qApp, std::forward<Func>(func), type);
#else
    // Qt 5.7 兼容
    aAddQueued(std::forward<Func>(func), type);
#endif
}


template<typename Func>
inline void addBlockingQueued(Func &&func) {
    const auto type = Qt::BlockingQueuedConnection;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    // Qt 5.10+: 直接使用 functor 重载
    QMetaObject::invokeMethod(qApp, std::forward<Func>(func), type);
#else
    // Qt 5.7 兼容
    aAddQueued(std::forward<Func>(func), type);
#endif
}

/// @brief 通过 ItemPath 获取 QModelIndex
/// @details 支持以下 ItemPath 格式（均为 JSON 表示）：
///   - number:                    列表项位置, 等价于 model->index(n, 0)
///   - [row, col]:                表格单元格, 等价于 model->index(row, col)
///   - [[row, col]]:              统一二维数组格式（单层）, 等价于 model->index(row, col)
///   - [[r0,c0], [r1,c1], ...]:   统一二维数组格式（多层）, 逐级 model->index(row, col, parent)
///   - number[]:                  树节点纯数字路径, 等价于每级 col=0 的统一格式
///   - string[]:                  树节点名称路径, 通过 Qt::DisplayRole 文字匹配行（文本必须唯一，否则返回无效索引）
///   - [a, b, ..., [row, col]]:   末元素为 [row, col] 时, 前面各级按行导航到目标节点,
///                                最后一级定位该节点上第 row 行第 col 列（树形表格取单元格值）
///                                例: [0,1,0,2,[3,1]] 表示 节点0→节点1→节点0→节点2→该节点上第3行第1列
///   - mixed[]:                   混合类型路径, 元素可为 number (行), string (文字匹配), [row, col] (显式行列)
/// @note 统一二维数组格式 [[row,col], ...] 可表达任意视图：列表 [1]≈[[1,0]], 表格 [1,1]≈[[1,1]], 树 [1,1]≈[1,[1,0]]
/// @param model 数据模型指针
/// @param itemPath ItemPath 的 JSON 表示
/// @return 对应的 QModelIndex, 路径无效时返回 QModelIndex()
AST_UIUTIL_API QModelIndex modelIndexFromItemPath(QAbstractItemModel* model, const JsonValue& itemPath);

/*! @} */

AST_NAMESPACE_END
