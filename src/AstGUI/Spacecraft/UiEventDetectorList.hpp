///
/// @file      UiEventDetectorList.hpp
/// @brief     事件检测器列表管理组件
/// @details   表格显示激活/名称/类型，支持添加删除，即时同步到 Propagate
/// @author    axel
/// @date      2026-05-18
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
#include "AstGUI/UiObject.hpp"
#include <QWidget>

class QTableWidget;
class QPushButton;

AST_NAMESPACE_BEGIN

class Propagate;
class EventDetector;
class UiEventDetectorEditor;

/// @brief 事件检测器列表管理组件
class AST_GUI_API UiEventDetectorList : public UiObject
{
    Q_OBJECT
public:
    explicit UiEventDetectorList(Object* object, QWidget* parent = nullptr);
    explicit UiEventDetectorList(QWidget* parent = nullptr);
    ~UiEventDetectorList() override;

    void setPropagate(Propagate* prop);
    Propagate* getPropagate() const;

signals:
    void detectorSelected(EventDetector* det);

private slots:
    void onAddDetector(const QString& className);
    void onRemoveDetector();
    void onActiveToggled(int row, bool checked);
    void onNameChanged(int row, int col);

private:
    void setupUi();
    void refreshList();
    QString typeDisplayName(EventDetector* det) const;

    QTableWidget* table_ = nullptr;
    UiEventDetectorEditor* editor_ = nullptr;
    QPushButton*  addBtn_ = nullptr;
    QPushButton*  removeBtn_ = nullptr;
};

AST_NAMESPACE_END
